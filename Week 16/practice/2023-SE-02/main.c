#include <err.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h> // kill
#include <string.h> 
#include <sys/wait.h> 

const int BUFF_SIZE = 4096;
const char FOUND_IT[] = "found it!";

int main(int argc, char* argv[]) {
	// The parent process will be responsible for 
	// creating all the processes 
	// The processes will write to a single pipe 
	// that the parent reads from 
	// If the parent reads "found it!\n"
	// then all processes should be terminated 
	// (using kill) 
	// To save the process pids I will use a malloc-ed 
	// array of size argc - 1
	pid_t* pids = malloc(sizeof(pid_t) * (argc - 1));
	if (pids == NULL) {
		free(pids);
		err(26, "malloc");
	}

	int pipefd[2];
	if (pipe(pipefd) < 0) { 
		free(pids);
		err(26, "pipe"); 
	}
	for(int i = 1; i < argc; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			free(pids);
			err(26, "fork"); 
		}
		if (pid == 0) {
			// In child: execute command
			// The child will not read from the pipe
			close(pipefd[0]); 

			// Any errors shouldn't be printed 
			 int fd_null = open("/dev/null", O_WRONLY);
			 if (fd_null < 0) {
			 	free(pids);
			 	err(26, "dev null failed to open");
			 }
			 if(dup2(fd_null, 2) < 0) {
			 	free(pids);
			 	err(26, "dup2");
			 }
			 close(fd_null);

			if (dup2(pipefd[1], 1) < 0) {
				free(pids);
				err(26, "dup2");
			}
			// Now that fd 1 'points' to where 
			// pipefd[1] is pointing we can close pipefd[1]
			close(pipefd[1]);
			free(pids);
			// Execute the command 
			execl(argv[i], argv[i], (char*) NULL);
			err(26, "exec for %s", argv[i]);
		}
		// In the parent: continue creating children
		// Also, save the pid of the child 
		pids[i - 1] = pid;
	}
	// All children were created and should be working 
	close(pipefd[1]);
	char current_char;
	int read_res;
	char buff[BUFF_SIZE];
	bool found_it = false;

	int current_line_length = 0;
	
	// Read from the pipe line by line and use strcmp to 
	// compare the line to "found it!"
	while((read_res = read(pipefd[0], &current_char, sizeof(current_char))) > 0) {
		if (current_line_length >= BUFF_SIZE) { 
			free(pids);
			errx(26, "the output of one of the commands was too long"); 
		}
		if (current_char == '\n') {
			// Compare the line. If it matches, stop reading from the pipe and start killing processes
			buff[current_line_length] = '\0';	
			current_line_length = 0;
			if (strcmp(buff, FOUND_IT) == 0) {
				found_it = true;	
				break;
			}
		}
		else {
			buff[current_line_length++] = current_char;	
		}
	}
	if (read_res < 0) { 
		free(pids);
		err(26, "read"); 
	}
	close(pipefd[0]);

 	if (found_it) {
 		for(int i = 0; i < argc - 1; i++) {
 			if (kill(pids[i], SIGTERM) < 0
 				&& errno != ESRCH) {
 				// ESRCH is an error for when the process 
 				// we are attempting to kill doesn't exist
 				// (this means that it already terminated and is
 				// a zombie that has to be wait()-ed
 				free(pids);
 				err(26, "kill");	
 			}
 			// Right after kill, wait() the child 
			if(wait(NULL) < 0) {
				free(pids);
				err(26, "wait");
			}
 		}
 // 		for(int i = 0; i < argc - 1; i++) {
 // 			if(wait(NULL) < 0) {
 // 				free(pids);
 // 				err(26, "wait");
 // 			}
 // 		}
 		free(pids);
 		exit(0);
 	}

	// The text 'found it!' wasn't printed by any of the processess
	for(int i = 0; i < argc - 1; i++) {
		if(wait(NULL) < 0) {
			free(pids);
			err(26, "wait");
		}
	}
	free(pids);
	exit(1);
}
