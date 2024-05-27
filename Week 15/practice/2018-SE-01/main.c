#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

const int PIPE_NUMBER = 3;
const int CHILDREN_NUMBER = 4;

void wait_child(void);
void wait_child(void) {
	int status;
	if (wait(&status) < 0) { err(13, "wait"); }
	if (!WIFEXITED(status)) {
		errx(14, "Ooops somebody killed the child"); 
	}
	if (WEXITSTATUS(status) != 0) {
		errx(15, "The child returned a non-zero exit status"); 
	}
}

int main(int argc, char* argv[]) {
	// Find the most recently changed file in the directory argv[1]
	if (argc != 2) {
		errx(1, "Usage: %s <dir_name>", argv[0]);
	}

	// I will need 3 pipes and 4 forks 
	// find argv[1] -type f -exec stat {} -c "%Y %n" \; | sort -nr | head -n 1 | cut -d ' ' -f 2

	int pipefd[PIPE_NUMBER][2];
	// Init the pipes for all children that will be created later 
	for(int i = 0; i < PIPE_NUMBER; i++) {
		if (pipe(pipefd[i]) < 0) {
			err(2, "pipe");
		}
	}

	for(int i = 0; i < CHILDREN_NUMBER; i++) {
		// find argv[1] -type f -exec stat {} -c "%Y %n" \; | sort -nr | head -n 1 | cut -d ' ' -f 2
		int pid = fork();
		if (pid < 0) { err(3, "fork"); }
		if (pid == 0) {
			// Determine the pipe numbers where the child will write and read 
			int read_from = i - 1;
			int write_to = i;

			if (read_from >= 0) { close(pipefd[read_from][1]); }
			else { close(pipefd[0][0]); }
			if (write_to < PIPE_NUMBER) { close(pipefd[write_to][0]); } 
			else { close(pipefd[PIPE_NUMBER - 1][1]); }

			// close all the pipe ends that the child doesn't need 
			for(int j = 0; j < PIPE_NUMBER; j++) {
				if (j != read_from && j != write_to) {
					close(pipefd[j][0]);
					close(pipefd[j][1]);
				}
			}

			if (i != 0) { // All commands read from a pipe except the first one
				if(dup2(pipefd[read_from][0], 0) < 0) { err(9, "dup2"); }
			}
			if (i < PIPE_NUMBER) { // All commands write to a pipe except the last one
				if(dup2(pipefd[write_to][1], 1) < 0) { err(9, "dup2"); }
			}
			switch(i) {
				// find argv[1] -type f -exec stat -c %Y %n 
				case 0:
					execlp("find", "find", argv[1], "-type", "f", "-exec",
						"stat", "{}", "-c", "%Y %n", ";", (char*) NULL);
					err(6, "exec");
				case 1:
					// sort -nr
					execlp("sort", "sort", "-nr", (char*) NULL);
					err(6, "exec");
				case 2:
					// head -n 1 
					execlp("head", "head", "-n", "1", (char*) NULL);
					err(6, "exec");
				case 3:
					// cut -d ' ' -f 2
					execlp("cut", "cut", "-d", " ", "-f", "2", (char*) NULL);
					err(6, "exec");
			}
		}

	}

	// Close all pipes in the parent since they will not be used 
	for(int i = 0; i < PIPE_NUMBER; i++){
		for(int j = 0; j < 2; j++){
			close(pipefd[i][j]);
		}
	}

	for(int i = 0; i < CHILDREN_NUMBER; i++) {
		wait_child();
	}
}
