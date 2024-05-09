#include <err.h>
#include <unistd.h>

int main(void) {
	int n = 10000;

	pid_t fork_result = fork();

	if (fork_result < 0) {
		err(1, "Error when forking");
	}
	if (fork_result > 0) {
		for (int i = 0; i < n; i++) {
			if (write(1, "F", 1) < 0) {
				err(2, "Error when writing in father process");
			}
		}
	}
	else {
		for (int i = 0; i < n; i++) {
			if (write(1, "S", 1) < 0) {
				err(3, "Error when writing in child process");
			}
		}
	}
	if (write(1, "\n", 1) < 0) {
		err(4, "Error when writing to stdout");
	}
}
