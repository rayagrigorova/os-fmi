#include <err.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>

void read_safe(int fd, uint8_t* byte) {
	if (read(fd, byte, sizeof(*byte)) < 0) {
		err(16, "read"); 
	}
}
void write_safe(int fd, uint8_t* byte) {
	if (write(fd, byte, sizeof(*byte)) < 0) {
		err(17, "write"); 
	}
}

// Check if the message is valid and return the number N 
int try_message (int fd) {
	uint8_t buff;
	uint8_t xor_res = 0;

	read_safe(fd, &buff);
	if (buff != 0x55) {
		printf("byte is wrong got: %c\n", buff);
		return -1;
	}
	xor_res ^= buff;

	uint8_t N;
	read_safe(fd, &N);
	if (N < 4) { 
		printf("N too small\n");
		return -1; 
	} 
	xor_res ^= N;

	for(int i = 3; i <= N - 1; i++) {
		read_safe(fd, &buff);
		xor_res ^= buff;
	}
	 
	uint8_t checksum;
	read_safe(fd, &checksum);
	if (checksum != xor_res) { 
		printf("checksum\n");
		return -1; 
	} 
	return N;
}

int main(int argc, char* argv[]) {
	if (argc != 3) { errx(1, "usage"); } 

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) { err(2, "open"); }
	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(2, "open"); }

	// Algorithm: from each byte in the input file 
	// validate the message starting from it using 
	// the criteria described 
	// if the message is correct, write it to the output file 
	// The last position that could be the start of a message is 
	// file size - 4 
	struct stat statbuf;	
	if (fstat(fd_in, &statbuf) < 0) { err(5, "stat"); } 

	// Don't search for new messages after this point 
	int end_pos = statbuf.st_size - 4;
	for(int i = 0; i <= end_pos; i++) {
		// lseek since the offset was moved
		if (lseek(fd_in, i, SEEK_SET) < 0) { err(11, "lseek"); }	
		// the try_message function will return -1 
		// if the message was invalid and a positive 
		// number if the message is valid (the positive number is N);
		int res = try_message(fd_in);
		// printf("res: %d\n", res);
		// If the message was valid, write it to the file 
		if (res > 0) {
			printf("valid\n");
			uint8_t buff;
			// Skip to the start of the message 
			if (lseek(fd_in, i + 2, SEEK_SET) < 0) {
				err(11, "lseek");
			}
			// Read N - 3 bytes
			for(int j = 0; j < res - 3; j++) {
				read_safe(fd_in, &buff);
				write_safe(fd_out, &buff);
			}
		}
	}

	close(fd_in);
	close(fd_out);
}
