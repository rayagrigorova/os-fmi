#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	// printf("bar\n");
	if (argc != 2) { errx(1, "usage"); } 
	// This process is resposible for deleting the pipe 
	// The stdin of the current process should be changed to the pipe using dup2
	// Create a child process that executes the command
	// The parent should wait for the child and then unlink the pipe
	const char PIPE_PATH[] = "pipepath.txt";
	int pipefd = open(PIPE_PATH, O_RDONLY);
	if (pipefd < 0) { err(4, "open"); }

	int pid = fork();
	if (pid < 0) { err(2, "fork"); }
	if (pid == 0) {
		// in child: dup2 and execute the command 
		if(dup2(pipefd, 0) < 0) { err(5, "dup2"); }
		execl(argv[1], argv[1], (char*) NULL);
		err(7, "exec");
	}
	// In the parent
	wait(NULL);
	unlink(PIPE_PATH);
}
