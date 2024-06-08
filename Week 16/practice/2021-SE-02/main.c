#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

void handle_encoded_data(uint8_t buff[], int fd_out) {
	// For each one of the two bytes in the buffer: 
	// Parse the last 2 bits 
	// If they are 10, write '1' to the uint8_t result 
	// Else if they are 01, write '0'
	// (I will not actually write 0 since the result 
	// will be initialized to 0 anyway)
	// In any other case, the format of the file is invalid and an 
	// error should be printed 
	// Left shift the uint8_t result before each iteration 
	// (to avoid the last exessive left shift from the  last iteration)

	uint8_t result = 0; // the decoded 16 bits 
	uint8_t one_mask = 2; // 10 for '1' 
	uint8_t zero_mask = 1; // 01 for '0'

	for(int i = 0; i < 8; i++) {
		// I will read the buffer from least to most significant bit so the result should be right shifted 
		result >>= 1;
		uint8_t last_two_bits = 3; // a '11' mask to read the last 2 bits 
		// The index is '1 - i /4', because I want to start by parsing the last 8 bits first 
		// (meaning buff[1]) 
		// The reason for this is endianness
		last_two_bits &= buff[1 - i / 4];

		if (last_two_bits == one_mask) {
			// write '1' to the result 
			result |= 1 << 7; 
		}
		// Skipping the case when the last 2 bits are 01
		// due to the fact that the result is initialized to 0
		// and that I am shifting the bits
		else if (last_two_bits != zero_mask) {
			errx(7, "Invalid format for input file: read the bits %x", last_two_bits);
		}
		buff[1 - i / 4] >>= 2; // go to the next 2 bits
	}
	if (write(fd_out, &result, sizeof(result)) < 0) {
		err(8, "Write of result to file failed");
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		errx(1, "Usage: %s <file_in> <file_out>", argv[0]);
	}
	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) { err(2, "open"); }

	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(2, "open"); }

	// Read 2 bytes from the input file and translate them to a single byte 
	uint8_t buff[2];
	int read_res;

	while((read_res = read(fd_in, &buff, sizeof(buff))) > 0) {
		handle_encoded_data(buff, fd_out);
	}
	if (read_res < 0) { 
		err(3, "read");
	}
}
