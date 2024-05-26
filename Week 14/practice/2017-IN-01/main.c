#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

const int COMMANDS_COUNT= 5;
const int PIPES_COUNT = 4;

int main(void) {
	// The sevents column of /etc/passwd contains the command interpreters 
	// The bash command that I should execute is the following: 
	// cat /etc/passwd | cut -d ':' -f 7 | sort | uniq -c | sort -n

	// rest in pepperoni
	// 	const char* argv1[] = {"cat", "/etc/passwd", NULL };
	// 	const char* argv2[] = {"cut", "-d", ":", "-f", "7", NULL };
	// 	const char* argv3[] = {"sort", NULL };
	// 	const char* argv4[] = {"uniq", "-c", NULL };
	// 	const char* argv5[] = {"sort", "-n", NULL };
	// 	const char** argument_arrays[] = {argv1, argv2, argv3, argv4 };

	// Create 4 pipes
	// for i = 0 to 5
	// if first command, only point stdout 
	// if last command, only point stdin
	// close all pipe ends that the current command doesn't need 
	// execute the command (using a switch case and execlp)

	// in the parent: close all pipe ends (since the parent doesn't need them) 
	// wait for all 5 children
	int pipefd[PIPES_COUNT][2];
	for(int i = 0; i < PIPES_COUNT; i++) {
		if (pipe(pipefd[i]) < 0) { err(1, "pipe"); }
	}

	for (int i = 0; i < COMMANDS_COUNT; i++) {
		pid_t pid = fork();
		if (pid < 0) { err(4, "fork"); }

		if (pid == 0) {
			int pipe_read = i - 1;
			int pipe_write = i;

			for (int j = 0; j < PIPES_COUNT; j++) {
				if (j != pipe_read && j != pipe_write) {
					close(pipefd[j][0]);
					close(pipefd[j][1]);
				}
			}
			if (i != 0) {
				close(pipefd[pipe_read][1]);
				dup2(pipefd[pipe_read][0], 0);
				close(pipefd[pipe_read][0]);
			}
			if (i < PIPES_COUNT) {
				close(pipefd[pipe_write][0]);
				dup2(pipefd[pipe_write][1], 1);
				close(pipefd[pipe_write][1]);
			}
			
			switch(i){
				case 0:
					execlp("cat", "cat", "/etc/passwd", (char*) NULL);
					err(2, "exec");
				case 1:
					execlp("cut", "cut", "-d", ":", "-f", "7", (char*) NULL);
					err(2, "exec");
				case 2:
					execlp("sort", "sort", (char*) NULL);
					err(2, "exec");
				case 3:
					execlp("uniq", "uniq", "-c", (char*) NULL);
					err(2, "exec");
				case 4:
					execlp("sort", "sort", "-n", (char*) NULL);
					err(2, "exec");
				default:
					errx(3, "execlp switch case");
			}
		}
	}

	for (int i = 0; i < PIPES_COUNT; i++) {
		close(pipefd[i][0]);
		close(pipefd[i][1]);
	}
  	for (int i = 0; i < COMMANDS_COUNT; i++) {
  		int status;
  		if (wait(&status) < 0) { err(6, "wait"); }
  		if (!WIFEXITED(status) || WEXITSTATUS(status)) { warnx("Child didn't exit properly"); }
  	}
}
