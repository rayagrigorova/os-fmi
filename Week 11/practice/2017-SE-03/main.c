#include <err.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>

struct patch_entry{
	uint16_t offset;
	uint8_t original;
	uint8_t new;
};

int main(int argc, char* argv[]){
	// 0. Validation: 3 (actual) arguments, the size of the patch file should be divisible by 4. Validate the offsets and the bytes on the given offsets
	if (argc != 4){
		errx(1, "Usage: %s <patch file> <original> <new>", argv[0]);
	}
	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0){
		err(2, "Couldn't stat %s", argv[1]);
	}
	if (statbuf.st_size % 4) {
		errx(3, "The format of %s is incorrect", argv[1]);
	}

	off_t file_size;
	if (stat(argv[2], &statbuf) < 0){
		err(7, "Couldn't stat %s", argv[2]);
	}
	file_size = statbuf.st_size;

	int read_result;
	struct patch_entry buff;
	int fd_patch = open(argv[1], O_RDONLY);
	if (fd_patch < 0){
		err(4, "Couldn't open %s", argv[1]);
	}

	int fd_original = open(argv[2], O_RDONLY);
	if (fd_original < 0){
		err(4, "Couldn't open %s", argv[2]);
	}

	while ((read_result = read(fd_patch, &buff, sizeof(struct patch_entry))) > 0) {
		if (buff.offset >= file_size){
			errx(8, "Invalid offset found in %s", argv[1]);
		}
		if (lseek(fd_original, buff.offset, SEEK_SET) < 0){
			err(9, "Error when using lseek in %s", argv[2]);
		}
		uint8_t original_char;
		if (read(fd_original, &original_char, sizeof(original_char)) < 0){
			err(10, "Error when reading from %s", argv[2]);
		}
		if (original_char != buff.original){
			printf("Expected: %s, got %s", &original_char, &buff.original);
			errx(11, "At least one invalid original character in %s", argv[1]);
		}
	}
	if (read_result < 0) {
		err(6, "Error when reading from %s", argv[1]);
	}

	if (lseek(fd_patch, 0, SEEK_SET) < 0) {
		err(5, "Error when using lseek in %s", argv[1]);
	}
	if (lseek(fd_original, 0, SEEK_SET) < 0) {
		err(12, "Error when using lseek in %s", argv[2]);
	}

	// Algorithm: open file 2 and copy all the contents of file 1 into it 
	int fd_new = open(argv[3], O_WRONLY | O_CREAT, 0666);
	if (fd_new < 0){
		err(13, "Error when opening %s", argv[3]);
	}

	char read_buff[512];
	while ((read_result = read(fd_original, read_buff, sizeof(read_buff))) > 0) {
		if (write(fd_new, read_buff, read_result) < 0) {
			err(14, "Couldn't write to %s", argv[3]);
		}
	}
	if (read_result < 0) {
		err(15, "Couldn't read from %s", argv[2]);
	}

	close(fd_original);
	// Now, go through the patch file and change the bytes that need changing in the new file using lseek
	if (lseek(fd_new, 0, SEEK_SET) < 0) {
		err(16, "Couldn't lseek in %s", argv[3]);
	}

	while ((read_result = read(fd_patch, &buff, sizeof(struct patch_entry))) > 0) {
		if (lseek(fd_new, buff.offset, SEEK_SET) < 0) {
			err(17, "Couldn't lseek in %s", argv[3]);
		}
		if (write(fd_new, &buff.new, sizeof(buff.new)) < 0) {
			err(18, "Couldn't write in %s", argv[3]);
		}
	}

	close(fd_patch);
	close(fd_new);
}
