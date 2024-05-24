#include <err.h> 
#include <stdio.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <fcntl.h> 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Usage: %s <text_file_name>", argv[0]);
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) { err(2, "Can't open %s", argv[1]); } 

	// Use cat and then pipe it to sort 
	// Create a child process that executes cat 
	// Then the parent executes sort on the result 
	// The output of cat is linked to a pipe that the 
	// parent process reads from
	int pipefd[2];
	if (pipe(pipefd) < 0) {
		err(7, "pipe");
	}
	pid_t fork_res = fork();
	if (fork_res < 0) { err(3, "fork"); } 
	if (fork_res == 0) {
		// The child writes to the pipe, 
		// so close the read end 
		close(pipefd[0]);
		// Make the file descriptor 1 point to the pipe's write end 
		if (dup2(pipefd[1], 1) < 0) { err (7, "dup"); }
		execlp("cat", "cat", argv[1], (char*) NULL);
		err(3, "execlp");
	}
	// Do I have to use wait here? Since read() is a blocking operation, I don't think so but...

	// The parent will read, so close fd for writing
	close(pipefd[1]);
	// make fd 0 (stdin) point to the 
	// read end of the pipe 
	if (dup2(pipefd[0], 0) < 0) { err (7, "dup"); }
	execlp("sort", "sort", (char*) NULL);
	err(3, "execlp");
}
