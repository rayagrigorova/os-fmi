#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

// using fstat, check the file size 
int get_file_size(int fd){
	struct stat statbuf;
	if (stat(fd, &statbuf) < 0){
		err(1, "stat");
	}
	return statbuf.st_size;
}
typedef struct diff{
	uint16_t original;
	uint16_t new;
} diff;

int main(int argc, char* argv[]){
	if (argc != 4){
		errx(1, "Not enough args");
	}	
	// Open the thee files  
	int f1 = open(argv[1], O_RDONLY);
	if (f1 == -1){
		err(1, "%s",argv[1]);
	}

	int f2 = open(argv[2], O_RDONLY);
	if (f2 == -1){
		err(1, "%s",argv[2]);
	}

	int patch = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT);
	if (patch == -1){
		err(1, "%s",argv[3]);
	}

	int size1 = get_file_size(f1);
	int size2 = get_file_size(f2);
	if (size1 != size2){
		errx(1, "Couldnt patch");
	}

	// If the files are too big, a patch uwing int16_t is impossible 
	if (size1 > UINT16_MAX){
		errx(1, "File 1 is too big to patch");
	}

	// Read the files byte by byte
	uint8_t original_byte;
	uint8_t new_byte;
	ssize_t read_size;
	uint16_t current_offset;
	while ((read_size = read(f1, &original_byte, sizeof(uint8_t))) < 0){
		if (read(f2	, &new_byte, sizeof(uint8_t)) < 0){
			err(4, "Cant read f2");
		}
		if (original_byte != new_byte){
			diff d;
			d.offset = current_offset;
			d.original = original_byte;
			d.new = new_byte;

			if (write(patch, &d, sizeof(diff)) == -1){
				err(5, "Err writing to patch file");
			}
		}

		current_offset++;
	}
	if (read_size < 0){
		err(1, "Read err");
	}

	if (close(f1) < 0){
		err(1, "Error when closing %s", argv[1]);
	}
	if (close(f2) < 0){
		err(1, "Error when closing %s", argv[2]);
	}
	if (close(patch) < 0){
		err(1, "Error when closing %s", argv[3]);
	}
}
