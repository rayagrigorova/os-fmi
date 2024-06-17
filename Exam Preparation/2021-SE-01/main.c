#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

// Use echo for the following commands: kill and passwd 
int main(int argc, char* argv[]) {
	const char FILENAME[] = "foo.log";
	int fd = open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0) { err(1, "open"); } 

	// Subtasks:
	// 1. Get the date in the correct format
	// (using gettimeofday and localtime)
	// YY-MM-DD HH:MM:SS.UUUUUUUU
	struct timeval tv;
	// struct timezone tz;
	if (gettimeofday(&tv, NULL) < 0) { err(1, "gettimeofday"); }
	// Only timeval will be useful for us 
	// I will pass time_t tv_seconds to localtime and then append
	// suseconds_t tv_usec tv_usec to the final result 
	struct tm* res = localtime(&tv.tv_sec);
	dprintf(fd, "%d-%d-%d %d:%d:%d.%d\n", res->tm_year + 1900, 
			res->tm_mon, res->tm_mday, res->tm_hour, 
			res->tm_min, res->tm_sec, (int)tv.tv_usec);

	// 2. Get the user id using getuid and getpwuid 
	uid_t user_id = getuid();
	struct passwd* passwd_entry = getpwuid(user_id);
	char* name = passwd_entry->pw_name;
	if(write(fd, name, strlen(name)) < 0) { err(3, "write"); }
	if (write(fd, "\n", 1) < 0) { err(3, "write"); }
	// 3. Get data (from argv) and write everythin found in 1. and 2.
	for(int i = 1; i < argc; i++) {
		if(write(fd, argv[i], strlen(argv[i])) < 0) { err(3, "write"); }
		if (write(fd, " ", 1) < 0) { err(3, "write"); }
	}
	if (write(fd, "\n", 1) < 0) { err(3, "write"); }
	close(fd);

	// 4. Lock the user account by changing the password 
	// (this is done using the command passwd which should be 
	// executed in a child process)
	int pid = fork();
	if (pid < 0) { err(4, "fork"); } 
	if (pid == 0) {
		// the command should be something like "sudo passwd -l <username>"
		execlp("echo", "echo", "s***", "passwd", "-l", name, (char*) NULL);
		err(8, "exec");
	}
	//Parent
	wait(NULL);

	// 5. Using ps, get all the user process id's and 
	// write them to a pipe (pis -o pid=). Read them line by line
	// and using strtol(), convert to a number. Then send sigkill to eahc one in the parent
	int pipefd[2];
	if (pipe(pipefd) < 0) { err(9, "pipe"); }

	pid = fork();
	if (pid < 0) { err(4, "fork"); } 
	if (pid == 0) {
		close(pipefd[0]);
		if(dup2(pipefd[1], 1) < 0) { err(11, "dup"); }
		execlp("ps", "ps", "-o", "pid=", (char*)NULL);
		err(13, "exec");
	}
	//Parent
	close(pipefd[1]);

	char current_pid[4096];
	int len = 0;
	char buff;
	int read_res;
	while((read_res = read(pipefd[0], &buff, sizeof(buff))) > 0) {
		// Check the new symbol 
		if (len >= 4096) { errx(78, "pid too long"); }
		if (buff == '\n') {
			current_pid[len] = '\0';
			len = 0;
			long to_long = strtol(current_pid, NULL, 0);
			// printf("%ld\n", to_long);
			// if (kill(to_long, SIGKILL) < 0) { err(32, "kill"); }
			dprintf(1, "kill %ld %ld\n", to_long, (long)SIGKILL);
		}
		else {
			current_pid[len++] = buff;
		}
	}
	if (read_res < 0) { err(32, "read"); }

	close(pipefd[0]);
	wait(NULL);

}
