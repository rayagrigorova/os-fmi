// This version of the program works fine
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <err.h>

// This function parses a byte and converts it to 16 bits 
// that are in Manchester code (using 2 uint8_t-s) 
// This function also writes the result to a file
void handle_byte(uint8_t byte, int fd_out) {
	// Create two uint8_t-s for the result 
	uint8_t res[2];
	res[0] = 0;
	res[1] = 0;

	uint8_t one_mask = 2; // 10
	uint8_t zero_mask = 1; // 01

	// Read the most significant bit first 
	uint8_t mask = 1 << 7;	

	for(int i = 0; i < 8; i++) {
		// Start by left shifting to avoid an exess left shift 
		// after the last iteration of the loop. The first left shift, of course,
		// will do nothing.
		res[i / 4] <<= 2;
		// Read the current bit
		uint8_t current_bit = mask & byte;
		// Proceed to the bit of the next highest significance 
		mask >>= 1;
		if (current_bit > 0) { 
			// Set the last 2 bits to 10
			res[i / 4] |= one_mask;
		}
		else {
			// Set the last 2 bits to 01
			res[i / 4] |= zero_mask;
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
