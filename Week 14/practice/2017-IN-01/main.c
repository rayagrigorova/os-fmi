#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

int main(void) {
	// The sevents column of /etc/passwd contains the command interpreters 
	// The bash command that I should execute is the following: 
	// cat /etc/passwd | cut -d ':' -f 7 | sort | uniq -c | sort -n
	
	 const char* argv1[] = {"cat", "/etc/passwd", NULL };
	 const char* argv2[] = {"cut", "-d", ":", "-f", "7", NULL };
	 const char* argv3[] = {"sort", NULL };
	 const char* argv4[] = {"uniq", "-c", NULL };
	 const char* argv5[] = {"sort", "-n", NULL };

	 const char** argument_arrays[] = {argv1, argv2, argv3, argv4 };
  	printf("%s\n", argv1[1]);
  	printf("%s\n", argv2[1]);
  	printf("%s\n", argv3[0]);
  	printf("%s\n", argv4[1]);
  	printf("%s\n", argv5[1]);
  	printf("%s\n", argument_arrays[1][2]);

	int pipefd[4][2];
	for(int i = 0; i < 5; i++) {
		if (pipe(pipefd[i]) < 0) { err(3, "pipe"); }

		pid_t pid = fork();
		if (pid < 0) { err(4, "fork()"); } 
		if (pid == 0) {
			if (i != 0) {
				// Not the first command, so read from a pipe
				dup2(pipefd[i-1][0], 0);
			}
			if (i != 4) {
				// Not the last command, so point stdout to the pipe
				dup2(pipefd[i-1][1], 1);
			}
			// Doesn't work :(
			// execvp(argument_arrays[i][0], argument_arrays[i]);
			switch(i) {
				case 0: 
					execvp(argv1[0], argv1);
					err(5, "exec");
					// ...
			}
			err(5, "exec");
		}
		wait(NULL);
	}
}
