#include <err.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

void execute_command(char* cmd);
void write_to_stdout(pid_t pid);	

void execute_command(char* cmd) {
	pid_t pid = fork();	
	if (pid < 0) {
		err(2, "Error when using fork");
	}
	if (pid == 0) {
		execlp(cmd, cmd, (char*) NULL);
		exit(EXIT_FAILURE);
	}
}

void write_to_stdout(pid_t pid) {
	char message[] = "The first command to finish its execution successfully has a PID ";
	if (write(1, message, strlen(message)) < 0) {
		err(10, "Error when using write on stdout");
	}
	char buff[32];
	snprintf(buff, sizeof(buff), "%d", pid);
	if (write(1, buff, strlen(buff)) < 0) {
		err(10, "Error when using write on stdout");
	}
	if (write(1, "\n", 1) < 0) {
		err(10, "Error when using write on stdout");
	}
}

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като командни 
	// параметри две команди (без параметри). Изпълнява ги 
	// едновременно и извежда на стандартния изход номера 
	// на процеса на първата, завършила успешно. Ако нито 
	// една не завърши успешно, извежда -1.

	if (argc != 3) {
		errx(1, "Usage: %s <command_1> <command_2>", argv[0]);
	}

	execute_command(argv[1]);
	execute_command(argv[2]);

	int status;
	pid_t wait_res = wait(&status);
	if (wait_res < 0) {
		err(6, "Error when executing wait()");
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		wait_res = wait(&status);
		if (wait_res < 0) {
			err(6, "Error when executing wait()");
		}
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			if (write(1, "-1", 2) < 0){
				err(17, "write");
			}
			exit(0);
		}
	}
	write_to_stdout(wait_res);	
}
