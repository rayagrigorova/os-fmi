#include <stdint.h>
#include <sys/stat.h>
#include <err.h>
#include <unistd.h>

struct Swap{
	uint16_t offset;
	uint8_t orig;
	uint8_t new;
};

int main(int argc, char* argv[]){
	// 0. Argument validation: number of arguments, the number of path.bin should be divisible by 32
	if (argc != 4){
		errx(1, "The number of arguments is wrong");
	}
	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0){
		err(2, "Couldnt stat %s", argv[1]);
	}
	if (statbuf.st_size % 32){
		errx(3, "The size of the patch size is wrong");
	}

	int fd_f1, fd_patch;

	if (fd_f1 = open(argv[2], O_RDONLY) < 0){
		err(4, "Coudln't open %s", argv[2]);
	}
	if (fd_patch = open(argv[1], O_RDONLY) < 0){
		err(6, "Coudln't open %s", argv[1]);
	}
	if (stat(argv[2], &statbuf) < 0){
		err(7, "Couldnt stat %s", argv[2]);
	}
	int f1_size = statbuf.st_size;
	// 1. Validate patch.bin: check for the offset in the file and if orig is actually the byte on this position
	struct Swap buff;
	uint8_t byte;
	int return_res;
	while (( return_res = read(fd_patch, &buff, sizeof(buff))) > 0){
		if(buff.offset >= f1_size){
			errx(9, "The patch file is  in invalid format");
		}
		if(lseek(fd_f1, buff.offset, SEEK_SET) < 0){
			err(10, "Error when using lseek");
		}
		if (read(fd_f1, &byte, sizeof(byte)) < 0){
			err(11, "Error when reading from %s", argv[2]);
		}
		if(byte != buff.orig){
			errx(12, "The patch file contains an invalid original byte");
		}
	}
	if (return_res < 0){
		err(8, "Error when reading from %s", argv[1]);
	}

	if(lseek(fd_f1, 0, SEEK_SET) < 0){
		err(14, "Error when using lseek on %s", argv[2]);
	}
	if(lseek(fd_patch, 0, SEEK_SET) < 0){
		err(15, "Error when using lseek on %s", argv[1]);
	}
	
	int fd_f2;
	if (fd_f2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666) < 0){
		err(16, "Coudln't open %s", argv[3]);
	}
	// 2. Copy the file f1 in f2 and 
	while ((return_res = read(fd_f1, &byte, sizeof(byte))) > 0){
		if (write(fd_f2, &byte, sizeof(byte)) < 0 )	{
			err(17, "Coudln't write to %s", argv[3]);
		}
	}
	// 3. Make the changes where necessary - for each patch lseek to offset, change the byte to <new>k
	return 0;
}
