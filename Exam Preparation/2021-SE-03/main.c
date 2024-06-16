#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	// Checks: two arguments, the size of file 1
	// is divisible by 2 
	if (argc != 3) {
		errx(1, "Usage: %s <file_1> <file_2>", argv[0]);
	}

	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0) {
		err(2, "stat");
	}
	if (statbuf.st_size % 2 || statbuf.st_size > (off_t)(524288 * sizeof(uint16_t))) {
		errx(3, "wrong format for file");
	}
	// Additional check if the second arg is a .h file
	// The array in the file should be defined like so:
	// int arr[] = { 12, 45, 29, 4 };
	
	// Algorithm: open the two files and start reading from file1
	// first write the following to file2: 
	// const uint16_t arr[] = {\n
	// After that, write each one of the numbers using dprintf 
	// (add a new line after a few numbers so that the lines are not too long 
	// finally, add }\n int arrN = <size> 
	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) { err(5, "open"); }
	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(5, "open"); }

	const char HEADER_BEGIN[] = "#include <stdint.h>\n\nuint16_t arr[] = {\n";

	if (write(fd_out, HEADER_BEGIN, strlen(HEADER_BEGIN)) < 0) {
		err(6, "open");
	}

	uint16_t buff;
	int read_res;
	const int NEWLINE_COUNT = 5; // Number of elems to write before new line
	int number_of_elements = statbuf.st_size / sizeof(uint16_t);
	int ctr = 0;

	while((read_res = read(fd_in, &buff, sizeof(buff))) > 0) {
		// if(write(fd_out, &buff, sizeof(buff)) < 0) {
		// 	err(8, "read");
		// }
		dprintf(fd_out, "%d", buff);
		ctr++;
		if (ctr < number_of_elements) {
			if (write(fd_out, ", ", 2) < 0) { err(9, "write"); }
		}
		if (ctr % NEWLINE_COUNT == 0) {
			if (write(fd_out, "\n", 1) < 0) { err(9, "write"); }
		}
	}
	if (read_res < 0) { err(7, "read"); }

	if (write(fd_out, "\n};\n\n", 5) < 0) {
		err(9, "write");
	}

	const char ARRAY_SIZE[] = "const uint32_t arrN = ";
	if (write(fd_out, ARRAY_SIZE, strlen(ARRAY_SIZE)) < 0) {
		err(9, "write");
	}
	dprintf(fd_out, "%d;\n", number_of_elements);

	close(fd_in);
	close(fd_out);
}
