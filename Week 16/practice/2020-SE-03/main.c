#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdint.h>

struct file_element{
	char file_name[8];
	uint32_t offset;
	uint32_t length;
}__attribute__((packed));

void handle_child(int pipefd[2], struct file_element* file_entry) {
	// The last byte should be 0x00
	if (file_entry->file_name[7] != 0x00) {
		errx(18, "file name");
	}
	close(pipefd[0]);
	int file = open(file_entry->file_name, O_RDONLY);
	if (file < 0) { err(7, "open"); }

	struct stat statbuf;
	if (fstat(file, &statbuf) < 0) { 
		err(8, "stat");
	}
	int file_size = statbuf.st_size;
	if ((int)(file_entry->offset * 2 + file_entry->length * 2) > file_size) {
		errx(13, "Length of interval");
	}

	if (lseek(file, file_entry->offset * 2, SEEK_SET) < 0) {
		err(12, "lseek");
	}

	uint16_t result = 0;
	uint16_t buff;
	int read_res;
	while((read_res = read(file, &buff, sizeof(buff))) > 0) {
		result ^= buff;
	}
	if (read_res < 0) { err(14, "read in child"); }

	if (write(pipefd[1], &result, sizeof(result)) < 0) {
		err(15, "write to pipe");
	}
	// printf("Child res: %.4X\n", result);
	close(pipefd[1]);
	close(file);
}

int main(int argc, char* argv[]) {
	if (argc != 2) { 
		errx(1, "Usage: %s <file_name>", argv[0]);
	}
	// Checks: file size should be divisible by 
	// 16 and not more than 16 * 8. 
	// The last byte of the file name should be 0x00
	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0) {
		err(2, "stat");
	}
	if (statbuf.st_size % 16 || statbuf.st_size > 16 * 8) {
		errx(3, "Invalid file");
	}

	int number_of_children = 0;
	int read_res;
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) { err(4, "open"); }
	// Create a pipe in the parent that is common for all
	// processes. Then for each file open it, seek to the 
	// needed offset (first checking if it's a valid offset)
	// From there, check if the length is valid. 
	// If yes, procees to read <length> elements. 
	// Finally, the child writes the result to a pipe 
	// The parent reads from the pipe and xors the written numbers
	int pipefd[2];
	struct file_element buff;
	if (pipe(pipefd) < 0) { err(5, "pipe"); }

	while((read_res = read(fd, &buff, sizeof(buff))) > 0) {
		int pid = fork();
		if (pid < 0) { err(5, "fork"); }
		if (pid == 0) {
			handle_child(pipefd, &buff);
			exit(0);
		}
		number_of_children++;
	}

	close(pipefd[1]);
	uint16_t element_buff;
	uint16_t res = 0;
	while((read_res = read(pipefd[0], &element_buff, sizeof(element_buff))) > 0) {
		res ^= element_buff;
	}
	if (read_res < 0) { 
		err(17, "read in parent");
	}
	close(pipefd[0]);
	dprintf(1, "%.4X\n", res);
	// printf("Number of children %d\n", number_of_children);
	for(int i = 0; i < number_of_children; i++) {
		int status;
		if (wait(&status) < 0) { 
			err(18, "wait");
		}
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			warnx("A child didn't exit with 0");
		}
	}
}
