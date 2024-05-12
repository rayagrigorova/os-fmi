#include <err.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като параметри три 
	// команди (без параметри), изпълнява ги последователно, като изчаква
	// края на всяка и извежда на стандартния изход номера на завършилия 
	// процес, както и неговия код на завършване. 
	if (argc != 4) {
		errx(1, "Usage: %s <command_1> <command_2> <command_3>", argv[0]);
	}	
	// Fork 3 times, each time calling wait() in the parent process 
	// after wait(), check the status of the child process and print 
	// its id (which is returned by wait())
	for (uint8_t i = 0; i < 3; i++) {
		pid_t fork_res = fork();	
		if (fork_res < 0) {
			err(2, "Couldn't fork");
		}
		if (fork_res == 0) {
			// In the child, execute the command 
			if (execlp(argv[i + 1], argv[i + 1], (char*) NULL) < 0) {
				err(3, "Error when using execlp");
			}
		}
		else {
			// Wait for the child process to finish 
			int status;
			pid_t wait_res = wait(&status);
			if (wait_res < 0) {
				err(4, "Error when using wait()");
			}
			if (!WIFEXITED(status)) {
				errx(5, "The child process didn't terminate successfully");
			}
			int child_exit_status = WEXITSTATUS(status); 
			char exit_status_buff[8];
			char process_number[4];
			snprintf(exit_status_buff, sizeof(exit_status_buff), "%d", child_exit_status);
			snprintf(process_number, sizeof(process_number), "%d", i);
			
			if (write(1, "Process number: ", 16) < 0) {
				err(6, "Error when using write()");
			}
			if (write(1, &process_number, strlen(process_number)) < 0) {
				err(6, "Error when using write()");
			}
			if (write(1, " Exit code: ", 12) < 0) {
				err(6, "Error when using write()");
			}
			if (write(1, exit_status_buff, strlen(exit_status_buff)) < 0) {
				err(6, "Error when using write()");
			}
			if (write(1, "\n", 1) < 0) {
				err(6, "Error when using write()");
			}
		}
	}
}
