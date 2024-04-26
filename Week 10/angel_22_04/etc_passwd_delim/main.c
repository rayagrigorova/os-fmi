#include <fcntl.h>
#include <err.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	const char REPLACEMENT = '?';

	// The file name is accepted as an argument  of the function 
	if (argc != 2){
		errx(1, "The program expects a file as a single argument");
	}
	// Open the file for reading and writing 
	int fd = open(argv[1], O_RDWR);
	if (fd < 0){
		err(2, "Could not open %s for reading", argv[1]);
	}
	// Go through each symbol of the file. If the current symbol is ':', lseek one position back and write '?' there 

	char buff; 
	ssize_t read_size;
	while (( read_size = read(fd, &buff, sizeof(buff))) > 0){
		if (buff == ':'){
			off_t lseek_result = lseek(fd, -1, SEEK_CUR); // Move one position back from the current one
			if (lseek_result < 0){
				err(3, "Could not lseek properly");
			}
			ssize_t write_result = write(fd, &REPLACEMENT, sizeof(REPLACEMENT)); 
			if (write_result < 0){
				err(4, "There was an error while trying to replace : with ? in the file");
			}
		}
	}

	if (read_size < 0){
		err(5, "There was an error while reading the file %s", argv[1]);
	}

	if (close(fd) < 0){
		err(6, "There was an error while closing the file %s", argv[1]);
	}
}
