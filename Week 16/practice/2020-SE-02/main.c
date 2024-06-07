#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

int main(int argc, char* argv[]) {
	// SCL -> bits 
	// SDL -> uint16_t, each element corresponds to a bit in SCL
	// Create a new file that only contains the SDL elements 
	// that had a high level in SCL
	if (argc != 4) {
		errx(1, "Usage: %s <SCL> <SDL> <new_file>", argv[0]);
	}

	// Checks: if SDL.size % 2 == 0 and if 
	// the size of SCL * 8 == size of SDL / 2
	// ( The number of elements is equal to the number of bits)
	struct stat statbuf_for_scl;
	struct stat statbuf_for_sdl;

	if (stat(argv[1], &statbuf_for_scl) < 0) { 
		err(2, "stat");
	}
	if (stat(argv[2], &statbuf_for_sdl) < 0) { 
		err(2, "stat");
	}

	if (statbuf_for_sdl.st_size % 2) { errx(3, "Invalid format for %s", argv[2]); } 
	if (statbuf_for_scl.st_size * 8 != statbuf_for_sdl.st_size / 2 ) {
		errx(4, "The size of %s doesn't correspond to the size of %s", argv[1], argv[2]);
	}

	// Idea: parse the bits one by one (by using a uint8_t 
	// and a mask + a counter so that we know when we should 
	// read the next byte)
	// For each byte, read from SDL
	// If the bit is 0, don't write to the new file 
	// If the bit is 1, write to the new file 
	int fd_scl = open(argv[1], O_RDONLY);
	if (fd_scl < 0) { err(5, "open"); }

	int fd_sdl = open(argv[2], O_RDONLY);
	if (fd_sdl < 0) { err(5, "open"); }

	int fd_newfile = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_newfile < 0) { err(5, "open"); }

	uint8_t mask = 1 << 7;
	uint8_t scl_buff;
	uint16_t sdl_buff;
	if (read(fd_scl, &scl_buff, sizeof(scl_buff)) < 0) { err(6, "read"); }
	int ctr = 0; // The bit number 
	int read_res = 0;
	while((read_res = read(fd_sdl, &sdl_buff, sizeof(sdl_buff))) > 0) {
		// Read the next byte from scl if needed
		if (ctr > 7) {
			ctr = 0;
			mask = 1 << 7;
			if (read(fd_scl, &scl_buff, sizeof(scl_buff)) < 0) { err(6, "read"); }
		}
		uint8_t result = (mask & scl_buff);
		mask >>= 1;
		ctr++;
		if (result == 0) { continue; }
		printf("SDL BUFFER: %d\n", sdl_buff);
		if (write(fd_newfile, &sdl_buff, sizeof(sdl_buff)) < 0) { 
			err(7, "write to new file");
		}
	}
	if (read_res < 0) { err(8, "read from sld"); }

	close(fd_scl);	
	close(fd_sdl);	
	close(fd_newfile);	
}
