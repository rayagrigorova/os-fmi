#include <stdint.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
	if (argc != 2){
		errx(1, "Usage: <FILE_NAME>");
	}
	// Use counting sort 
	uint32_t counting_array[256];
	for (int i = 0; i < 256; i++){
		counting_array[i] = 0;
	}
	// 1. Count the bytes from the file
	int fd = open(argv[1], O_RDWR | O_CREAT, 0666);
	if (fd < 0){
		err(2, "Couldnt open file %s", argv[1]);
	}

	ssize_t read_bytes;
	uint8_t buff;
	// Read one character at a time
	while (( read_bytes = read(fd, &buff, sizeof(buff))) > 0){
		counting_array[buff]++;
	}
	if (read_bytes < 0){
		err(3, "Err when reading %s", argv[1]);
	}

	// Seek to the beginning of the file and overwrite the bytes
	off_t seek_res = lseek(fd, 0, SEEK_SET);
	if (seek_res < 0){
		err(4, "Err when seeking");
	}

	for(uint16_t i = 0; i < 256; i++){
		for(uint32_t j = 0; j < counting_array[i]; j++){
			ssize_t written = write(fd, &i, sizeof(uint8_t));			
			if (written < 0){
				err(5, "Error when writing to file %s", argv[1]);
			}
		}
	}

	close(fd);
	return 0;
}
