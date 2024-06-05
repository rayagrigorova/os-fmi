#include <err.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const int BUFF_SIZE = 512;

void handle_line(char* file_name) {
	// The line length is guaranteed to be okay to use strcat on
	char hash_file_name[BUFF_SIZE];
	strcpy(hash_file_name, file_name);
	strcat(hash_file_name, ".hash");
	int fd_hash = open(hash_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_hash < 0)  { err(35, "Hash file open"); }
	// Then the parent creates a child process that writes 
	// an md5sum of the file to the hash file 
	int pid = fork();
	if (pid < 0) {err(38, "fork"); }
	if (pid == 0) {
		if (dup2(fd_hash, 1) < 0) { err(91, "dup2"); }
		close(fd_hash);
		execlp("md5sum", "md5sum", file_name, (char*) NULL);
		err(71, "md5sum");
	}
	close(fd_hash);
}

void handle_lines(int pipefd) {
	char buff[BUFF_SIZE];
	int read_res;
	int line_length = 0;
	char ch;
	int number_of_lines = 0;
	while((read_res = read(pipefd, &ch, sizeof(ch))) > 0) {
		if (line_length >= BUFF_SIZE - 1 - (int)strlen(".hash")) {
			errx(20, "file name too long");
		}
		if (ch == '\n') {
			buff[line_length] = '\0';
			line_length = 0;
			number_of_lines++;
			// printf("File name: %s\n", buff);
			handle_line(buff);
		}
		else {
			buff[line_length++] = ch;
		}
	}
	if (read_res < 0) { err(17, "read"); }

	int status;
	for(int i = 0 ; i < number_of_lines; i++) {
		if (wait(&status) < 0) { err(89, "wait"); }
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			warnx("Child process couldn't write and calculate md5sum");
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Usage: %s <dirname>", argv[0]);
	}

	// Algorithm: 
	// Find the files using the following command 
	// find argv[1] -type f -not -name '*.hash'
	// Write the result to a pipe 
	// Then the parent parses the file names one by one 
	// by creating a file with the same path but 
	// with an added suffix .hash
	// Then the parent creates a child process that writes 
	// an md5sum of the file to the hash file 

	int pipefd[2];
	if (pipe(pipefd) < 0) { err(2, "pipe"); }
	// find argv[1] -type f -not -name '*.hash'

	int pid = fork();
	if (pid < 0) { err(3, "fork"); }
	if (pid == 0) {
		close(pipefd[0]);	
		if (dup2(pipefd[1], 1) < 0) { err(5, "dup2"); }
		execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", (char*) NULL);
		err(7, "execlp");
	}
	// in the parent 	
	close(pipefd[1]);
	handle_lines(pipefd[0]);
	close(pipefd[0]);
}
