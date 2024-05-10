#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>

void write_null_terminated_message(const char* msg);
void write_message(const char* msg, int msg_size);

void write_null_terminated_message(const char* msg) {
	if(write(1, msg, strlen(msg)) < 0) {
		err(8, "Error when writing to stdout");
	}
}

void write_message(const char* msg, int msg_size) {
	if(write(1, msg, msg_size) < 0) {
		err(9, "Error when writing to stdout");
	}
}

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която изпълнява 
	// последователно подадените и като параметри
	// команди, като реализира следната функционалност
	// постъпково: 
	// main cmd1 ... cmdN Изпълнява всяка от командите в
	// отделен дъщерен процес 
	// ... При което се запазва броят на изпълнените команди, които са дали грешка и броя на завършилите успешно 
	
	// For each command: wait for it to finish and check
	// its exit status(if it had one)

	int ended_with_error = 0;
	int ended_successfully = 0;

	for(int i = 1; i < argc; i++) {
		pid_t fork_res = fork();	
		if (fork_res < 0) {
			err(1, "Couldn't fork");
		}
		if (fork_res == 0) {
			// in the child process: execute the command
			if (execlp(argv[i], argv[i], (char*) NULL) < 0) {
				err(2, "Error when trying to execute command %s", argv[i]);
			}
		}
		// In the parent process 
		int status;
		pid_t wait_res = wait(&status);
		if (wait_res < 0) {
			err(3, "Error when executing wait");
		}
		// Check what is written in status
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			ended_with_error++;			
		}
		else {
			ended_successfully++;
		}
	}

	char message1 [] = "Number of commands that ended unsuccessfully: ";
	write_null_terminated_message(message1);

	char buff[32];
	snprintf(buff, sizeof(buff), "%d", ended_with_error);
	write_null_terminated_message(buff);
	write_message("\n", 1);
// 	if (write(1, "\n", 1) < 0) {
// 		err(8, "Error when writing to stdout");
// 	}

	char message2 [] = "Number of commands that ended successfully: ";
	write_null_terminated_message(message2);
	snprintf(buff, sizeof(buff), "%d", ended_successfully);
	write_null_terminated_message(buff);
	write_message("\n", 1);
// 	if (write(1, "\n", 1) < 0) {
// 		err(8, "Error when writing to stdout");
// 	}
}
