#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>

int main(int  argc, char* argv[]) {
	// printf("foo\n");
	// Check if the number of args is 2
	if (argc != 2) {
		errx(1, "usage");
	}
	// Create the pipe, redirect the stdout of 
	// the process to the pipe using dup2 before 
	// using execlp
	 
	if (mkfifo("pipepath.txt", 0666) < 0) { err(2, "pipe"); } 
	int pipefd = open("pipepath.txt", O_WRONLY);
	if (pipefd < 0) { err(3, "open"); }

	if (dup2(pipefd, 1) < 0) {
		err(3, "dup2"); 
	}
	execlp("cat", "cat", argv[1], (char*) NULL);
	err(4, "exec");
}
