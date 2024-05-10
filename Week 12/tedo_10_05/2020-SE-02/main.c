#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		errx(1, "Usage");
	}
	int scl = open(argv[1], O_RDONLY);
	if (scl < 0) {
		err(2, "open");
	}
	int sdl = open(argv[2], O_RDONLY);
	if (sdl < 0) {
		err(2, "open");
	}
	int out = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (out < 0) {
		err(2, "open");
	}

	// Check input args for validity
	struct stat sdl_stat;
	if (fstat(sdl, &sdl_stat) < 0) { err(3, "stat"); } 
	struct stat scl_stat;
	if (fstat(scl, &scl_stat) < 0) { err(3, "stat"); } 
	if (sdl_stat.st_size % sizeof(uint16_t) != 0) {
		errx(4, "bad size for sdl");
	}

	int numElements = sdl_stat.st_size / sizeof(uint16_t);
	int neededSCL = numElements / 8;

	if (numElements % 8 != 0) {
		neededSCL++;
	}

	if(scl_stat.st_size != neededSCL) {
		errx(5, "Wrong file format");
	}

	// Iterate through SDL element by element. Read the scl file bit by bit

	uint8_t scl_byte; // The least amount that can be read is 1 byte
	uint16_t sdl_data;
	ssize_t read_size;
	while (( read_size = read(scl, &scl_byte, sizeof(scl_byte))) > 0) {
		// Read the byte bit by bit
		// The bits should be read left to right 
		for (uint8_t mask = 1 << 7; mask != 0; mask >>= 1) {
			if ((read_size = read(sdl, &sdl_data, sizeof(sdl_data))) < 0) {
				err(7, "read");
			}
			uint8_t bit = (scl_byte & mask);
			if (bit != 0) {
				// bit is set
				if (read_size == 0) {
					errx(5, "unexpected 1 in scl");
				}
				if (write(out, &sdl_data, szieof(sdl_data)) < 0) {
					err(6, "write");
				}
			}
		}
	}
	if (read_size < 0) {
		err(6, "Read");
	}

	close(scl);
	close(sdl);
	close(out);
}
