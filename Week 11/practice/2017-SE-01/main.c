#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>

struct patch_entry {
	uint16_t offset;
	uint8_t original;
	uint8_t new;
};

int main(int argc, char* argv[]){
	// 'if an offset exists in only one of the files, end the program' => the files should be of the same length 
	// 0. Checks: for 3 arguments and file sizes 
	if (argc != 4){
		errx(1, "Usage: %s <original> <new> <patch>", argv[0]);
	}
	struct stat statbuf;
	off_t size1;
	if (stat(argv[1], &statbuf) < 0){
		err(2, "Couldn't stat %s", argv[1]);
	}
	size1 = statbuf.st_size;
	if (stat(argv[2], &statbuf) < 0){
		err(3, "Couldn't stat %s", argv[2]);
	}
	if (size1 != statbuf.st_size){
		errx(4, "The sizes of the two files should be the same");
	}

	// Go through the files byte by byte (keeping the current offset in a variable). If the current read characters are different for the two files, save them to a buffer patch file entry and write them at once
	int fd1, fd2, fd_patch;
	if ((fd1 = open(argv[1], O_RDONLY)) < 0){
		err(5, "Error opening %s", argv[1]);
	}
	if ((fd2 = open(argv[2], O_RDONLY)) < 0){
		err(6, "Error opening %s", argv[2]);
	}
	if ((fd_patch = open(argv[3], O_WRONLY|O_CREAT|O_TRUNC,0666)) < 0){
		err(7, "Error opening %s", argv[3]);
	}

	uint8_t buff1, buff2;
	uint16_t current_index = 0;
	struct patch_entry entry_buff;
	int read_res_1, read_res_2;
	while ((read_res_1 = read(fd1, &buff1, sizeof(buff1))) > 0 && 
			(read_res_2 = read(fd2, &buff2, sizeof(buff2))) > 0) {
		if(buff1 != buff2) {
			// Save all the data to the patch_entry variable
			entry_buff.offset = current_index;
			entry_buff.original = buff1;
			entry_buff.new = buff2;
			int write_result = write(fd_patch, &entry_buff, sizeof(struct patch_entry));
			if (write_result < 0){
				err(8, "Error when writing to patch file %s", argv[3]);
			}
			if (write_result != sizeof(struct patch_entry)){
				errx(9, "Couldn't write all the data at once to %s", argv[3]);
			}
		}
		current_index++;
	}

	if(read_res_1 < 0){
		err(8, "Error when reading from %s", argv[1]);
	}

	if(read_res_2 < 0){
		err(9, "Error when reading from %s", argv[2]);
	}

	close(fd1);
	close(fd2);
	close(fd_patch);
}
