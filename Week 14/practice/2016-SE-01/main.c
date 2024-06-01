#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Usage: %s <binary_file_name>", argv[0]);
	}
	// Algorithm: go through the file once and count bytes using counting sort 
	// Then, lseek to its beginning and start overwriting all of its bytes with the counted symbols 
	int fd = open(argv[1], O_RDWR);
	if (fd < 0) { err(2, "open"); }

	uint32_t arr[256];
	for(int i = 0; i < 256; i++) {
		arr[i] = 0;
	}

	int read_res;
	uint8_t buff;
	while((read_res = read(fd, &buff, sizeof(buff))) > 0) {
		arr[(int)buff]++;
	}
	if (read_res < 0) {
		err(3, "read");
	}
	if (lseek(fd, 0, SEEK_SET) < 0) {
		err(4, "lseek");
	}
// 	for(int i = 0; i < 256; i++) {
// 		if (arr[i] > 0) {
// 			printf("Char: %c count: %d\n", i, arr[i]);
// 		}
// 	}
	for(int i = 0; i < 256; i++) {
		uint8_t to_write = (uint8_t)i;
		for(uint32_t j = 0; j < arr[i]; j++) {
			if(write(fd, &to_write, sizeof(to_write)) < 0) {
				err(5, "write");
			}
		}
	}
}
