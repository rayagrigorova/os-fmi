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

typedef struct {
	uint32_t offset;
	uint32_t length;
} pair;

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

	int f3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT);
	if (f3 == -1){
		err(1, "%s",argv[3]);
	}

	int size1 = get_file_size(f1);
	int size2 = get_file_size(f2);
	if (size1 % sizeof(uint32_t) * 2 || size2 % sizeof(uint32_t)){
		errx(1, "The file format of at least one of the files is wrong");
	}

	// 1. Read the pairs from f1
	pair p;
	while (( read_size = read(f1, &p, sizeof(pair))) > 0 )
		// dprintf(1, "(%d, %d)", p.offset, p.length);
		if (p.offset * sizeof(int32_t) >= size2 || (p.offset + p.length) * sizeof(int32_t) >= size2){
			err(1, "At least one of the intervals in f1 is wrong");
		}
		if (lseek(f2, p.offset * sizeof(int32_t), SEEK_SET) < 0){
			err(1, "Error when using lseek");
		}
		uint32_t num;
		for (uint32_t i = 0; i < p.length; i++){
			if (read(f2, &num, sizeof(num)) != sizeof(num)){
				err(1, "Cant read");
			}
			if (write(f3, &num, sizeof(num)) != sizeof(num)){
				err(1, "Cant write");
			}
		}
	}

	// 2 checks: if the sizes of the files are divisible by 32 in bytes
	if (close(f1) < 0){
		err(1, "Error when closing %s", argv[1]);
	}
	if (close(f2) < 0){
		err(1, "Error when closing %s", argv[2]);
	}
	if (close(f3) < 0){
		err(1, "Error when closing %s", argv[3]);
	}
}
