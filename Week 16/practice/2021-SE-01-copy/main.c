#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <err.h>

// This function translates a given byte to manchester code 
// and writes the result to a file
void handle_byte(uint8_t byte, int fd_out) {
	// Difference between the two provided solutions 
	uint16_t res = 0;

	uint16_t one_mask = 2;
	uint16_t zero_mask = 1;

	// Read the most significant bit first 
	uint8_t mask = 1 << 7;	

	for(int i = 0; i < 8; i++) {
		res <<= 2;
		uint8_t current_bit = mask & byte;
		mask >>= 1;
		if (current_bit > 0) { 
			res |= one_mask;
		}
		else {
			res |= zero_mask;
		}
	}
	if (write(fd_out, &res, sizeof(res)) < 0) { err(8, "write"); }
}

int main(int argc, char* argv[]) {
	if (argc != 3) { errx(1, "Usage: %s <file1> <file2>", argv[0]); } 

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) { err(2, "open"); }

	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(2, "open"); }
	
	// 1 -> 10
	// 0 -> 01
	int read_res = 0;
	uint8_t input_byte;

	// Read the byte from the original file and transform it
	while((read_res = read(fd_in, &input_byte, sizeof(input_byte))) > 0) {
		handle_byte(input_byte, fd_out);
	}
	if (read_res < 0) { err(3, "read"); }

	close(fd_in);	
	close(fd_out);	
}
