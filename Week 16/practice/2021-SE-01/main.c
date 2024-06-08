#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <err.h>

// This function parses the byte and translates it into uint16_t 
// by the given rules. Then the uint16_t is written to fd_out
void handle_byte(uint8_t byte, int fd_out) {
	// Create two uint8_t-s for the result 
	uint8_t res[2];
	res[0] = 0;
	res[1] = 0;

	uint8_t one_mask = 2;
	uint8_t zero_mask = 1;

	// Read the most significant bit first and left shift the result 
	// after translating it to manchester encoding 
	uint8_t mask = 1 << 7;	

	for(int i = 0; i < 8; i++) {
		res[i / 4] <<= 2;
		uint8_t current_bit = mask & byte;
		mask >>= 1;
		if (current_bit > 0) { 
			printf("read a 1\n");
			res[i / 4] |= one_mask;
		}
		else {
			printf("read a 0\n");
			res[i / 4] |= zero_mask;
		}
		printf("res: %x\n", res[i/4]);
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
	// Algorthm: read the input file byte by byte 
	// For each byte:
	// Use a buffer (of size uint16_t) for the translated bytes
	// For the input buffer, use a mask that contains a '1' to 
	// read the bit. For the uint16_t buffer, use a 01 and 10 
	// mask. Both masks should be left shifted by 1 and 2 respectively
	int read_res = 0;
	uint8_t input_byte;

	// Read the byte from the  roiginal file and transform it
	// the handle_byte function also takes care of writing the result 
	// uint16_t to the out file
	while((read_res = read(fd_in, &input_byte, sizeof(input_byte))) > 0) {
		handle_byte(input_byte, fd_out);
	}
	if (read_res < 0) { err(3, "read"); }

	close(fd_in);	
	close(fd_out);	
}
