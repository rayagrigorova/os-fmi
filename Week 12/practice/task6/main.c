#include <err.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като параметър 
	// команда (без параметри) и при успешното и изпълнение,
	// извежда на стандартния изход името на командата.
	if (argc != 2) {
		errx(1, "Usage: %s <command_name>", argv[0]);
	}
	// Fork the process, make the child execute the command. Wait for the status of the child and if the child returned 0, print a success message to stdout
	pid_t fork_result = fork();
	if (fork_result < 0) {
		err(2, "Error when attempting to fork");
	}
	if (fork_result == 0) {
		// Execute the command without arguments using execlp
		if (execlp(argv[1], argv[1], (char*) NULL) < 0) {
			err(3, "Error when using execlp");
		}
	}
	else {
		int wait_status;
		pid_t wait_result = wait(&wait_status);
		if (wait_result < 0) {
			err(4, "Error when using wait()");
		}
		if (!WIFEXITED(wait_status)) {
			errx(5, "The child process where the command was executed didn't terminate successfully");
		}
		if (WEXITSTATUS(wait_status) != 0) {
			errx(5, "The child process where the command was executed had a non-zero exit status");
		}
		char message [] = "Successfully executed the following command: ";
		if (write(1, message, sizeof(message)) < 0) {
			err(6, "Error when using write()");
		}
		if (write(1, argv[1], strlen(argv[1])) < 0) {
			err(6, "Error when using write()");
		}
		if (write(1, "\n", 1) < 0) {
			err(6, "Error when using write()");
		}
	}
}
