#include <err.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като параметри 
	// от команден ред две команди (без параметри).
	// Изпълнява първата. Ако тя е завършила успешно,
	// изпълнява втората. Ако не, завършва с код 42. 
	if (argc != 3) {
		errx(1, "Usage: %s <command_1> <command_2>", argv[0]);
	}
	// Fork the process for the first command and wait 
	// for it to finish. 
	pid_t fork_res = fork();
	if (fork_res < 0) {
		err(2, "Error when calling fork()");
	}
	if (fork_res == 0) {
		// In the child process, so execute the command 
		if (execlp(argv[1], argv[1], (char*) NULL) < 0) {
			err(3, "Error when trying to call %s", argv[1]);
		}
		// the child process will finish its execution after 
		// this point, so the only process that will execute 
		// the following lines is the parent process
	}
	// This is only executed in the parent
	int status;
	pid_t wait_res = wait(&status);
	if (wait_res < 0) {
		err(4, "Error when executing wait()"); 
	}
	// Check how the child terminated and for the exit status
	if (!WIFEXITED(status)) {
		errx(5, "The child process didn't terminate properly");
	}
	if (WEXITSTATUS(status) != 0) {
		exit(42);
	}

	// Fork again, this time for the second command 
	fork_res = fork();
	if (fork_res < 0) {
		err(2, "Error when calling fork()");
	}
	if (fork_res == 0) {
		// In the child process, so execute the command 
		if (execlp(argv[2], argv[2], (char*) NULL) < 0) {
			err(3, "Error when trying to call %s", argv[2]);
		}
	}

	wait_res = wait(&status);
	if (wait_res < 0) {
		err(4, "Error when executing wait()"); 
	}
	// Check how the child terminated and for the exit status
	if (!WIFEXITED(status)) {
		errx(5, "The child process didn't terminate properly");
	}
	if (WEXITSTATUS(status) != 0) {
		errx(6, "The child process didn't return 0");
	}
}
