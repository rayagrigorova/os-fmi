#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>

int assert_wait(void) {
	
	int w_status;

	if (wait(&w_status) == -1) {
		err(5, "cannot wait");
	}

	if (WIFEXITED(w_status) == 0) {
		errx(7, "child didnt terminate");
	}

	return w_status;

}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		errx(2, "args");
	}

	int pipefd[2];

	if (pipe(pipefd) == -1) {
		err(3, "cannot pipe");
	}

	int cat_pid = fork();

	//cat fork
	if (cat_pid == 0) {
		
		close(pipefd[0]);

		if (dup2(pipefd[1], 1) == -1) {
			err(6, "cannot dup2");
		}

		close(pipefd[1]);

		if (execlp("cat", "cat", argv[1], NULL) == -1) {
			err(4, "err in cat");
		}
		
	}

	pid_t sort_pid = fork();

	//sort fork
	if (sort_pid == 0) {
		
		close(pipefd[1]);
		
		if (dup2(pipefd[0], 0) == -1) {
			err(6, "err in dup");
		}

		close(pipefd[0]);
	
		if (execlp("sort", "sort", NULL) == -1) {
			err(5, "err in sort");
		}

	}

	close(pipefd[0]);
	close(pipefd[1]);

	assert_wait();
	assert_wait();

}
