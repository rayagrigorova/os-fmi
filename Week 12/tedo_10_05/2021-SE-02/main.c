#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

uint8_t decode(uint8_t byte);
uint8_t decode(uint8_t byte) {
	uint8_t result = 0;
	// Read the byte from right to left
	// Use a mask to save the least significant two bits
	// Then shift left the byte that is encoded  (shift left by 2)
	// Shifting the mask left is not a good idea because it will be hard to interpret the result
	for(int i = 0; i < 4; i++) {
		int code = byte & 0x03;
		if (code != 0x01 && code != 0x02) { errx(5, "bad encoding"); }
		if (code != 0x01) {
			result |= 1 << i; 	
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		errx(1, "Usage");
	}
	int in = open(argv[1], O_RDONLY);
	if (in < 0) {
		err(2, "open");
	}
	int out = open(argv[2], O_RDONLY);
	if (out < 0) {
		err(2, "open");
	}

	// Check input args for validity
	struct stat in_st;
	if (fstat(in, &in_st) < 0) { err(3, "stat"); } 
	if (in_st.st_size % 2 != 0) { err(4, "size"); }

	uint8_t encoded_bytes[2];
	ssize_t read_size;

	while (( read_size = read(int, encoded_bytes, sizeof(encoded_bytes))) > 0) {
		uint8_t result = (decode(encoded_bytes[0]) << 4) | decode(encoded_bytes[1]); 
	}

	if (read_size < 0) {
		err(6, "Read");
	}

	close(scl);
	close(sdl);
	close(out);
}
