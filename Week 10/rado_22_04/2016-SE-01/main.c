#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
int main(int argc, char* argv[]){
	// Use counting sort 
	// int bytes[256];
	// for byte in file 
		// bytes[byte]++;

	if (argc != 2 ){
		errx(1, "Wrong argument count");
	}
	int fd = open(argv[1], O_RDWR);
	if ( fd == -1 ){
		err(2, "Could not open file");
	}
	int bytes[256];
	// Initialize the array with 0
	for(int i = 0; i < 256; i++){
		bytes[i] = 0;
	}

	uint8_t byte; // 1 byte 

	int result; // Check the exit code of read
	while ((result = read(fd, &byte, sizeof(uint8_t))) == sizeof(uint8_t)) {
		bytes[byte]++;
	}
	if (result == -1){
		err(3, "An error ocurred while reading the file");
	}

	// write() will append to the end of the file with the sorted bytes if lseek is not called
	if(lseek(fd, 0, SEEK_SET) == -1){
		err(3, "Error when calling lseek");
	}

	for(int16_t i = 0; i < 256; i++){ //  The integer will overflow if it is int8_t
		for(int j = 0; j < bytes[i]; j++){
			// if (write(fd, &i, sizeof(i)) == -1){ // This is wrong 
			if (write(fd, &i, sizeof(uint8_t)) == -1){ 
				err(2, "Error when writing to file");
			}
		}
	}
	return 0;
}
