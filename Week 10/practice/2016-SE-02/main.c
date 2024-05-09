#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <err.h>

void print_file(int fd);

int main(int argc, char* argv[]){

	// Checks: 3 files, file 1 has a size divisible by 8, file 2 has a size divisible by 4 
	if ( argc != 4){
		errx(1, "Usage: <f1> <f2> <f3>");
	}
	// Using stat, determine the size of each of the two files 
	struct stat statbuf;
	int stat_res = stat(argv[1], &statbuf);
	if (stat_res < 0){
		err(2, "Couldnt stat %s", argv[1]);
	}
	if (statbuf.st_size % 8){
		errx(3, "Incorrect format for %s", argv[1]);
	}

	stat_res = stat(argv[2], &statbuf);
	if (stat_res < 0){
		err(2, "Couldnt stat %s", argv[2]);
	}
	if (statbuf.st_size % 4){
		errx(3, "Incorrect format for %s", argv[2]);
	}

	int fd1, fd2, fd3;
	if ((fd1 = open(argv[1], O_RDONLY)) < 0){
		err(4, "Couldnt open %s for reading", argv[1]);
	}
	if ((fd2 = open(argv[2], O_RDONLY)) < 0){
		err(4, "Couldnt open %s for reading", argv[2]);
	}
	if ((fd3 = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0){
		err(4, "Couldnt open %s for reading", argv[3]);
	}

	// For each xi from fd1
	// lseek to the xi-th number 
	//	for each yi from fd1 
	// read the next 32 bit number from fd2 and write it to fd3 	
	int32_t buff[2];
	ssize_t read_size;
	while ((read_size = read(fd1, buff, sizeof(buff))) > 0){
		off_t lseek_res = lseek(fd2, buff[0] * sizeof(int32_t), SEEK_SET);
		if (lseek_res < 0){
			err(5, "Couldnt lseek in %s", argv[2]);
		}
		for (int i = 0; i < buff[1]; i++){
			int32_t number_buffer; 
			ssize_t number_read_res = read(fd2, &number_buffer, sizeof(int32_t));
			if (number_read_res < 0){
				err(6, "Reading number from %s failed", argv[2]);
			}
			ssize_t number_written = write(fd3, &number_buffer, number_read_res);
			if (number_written < 0){
				err(7, "Error writing to %s", argv[3]);
			}
			else if (number_written != sizeof(int32_t)){
				errx(8, "Coudlnt write all the data to %s", argv[3]);
			}
		}
	}
	print_file(fd3);

	close(fd1);
	close(fd2);
	close(fd3);
}

void print_file(int fd){
	// Use this function for debugging 
	int32_t buff;
	off_t lseek_res = lseek(fd, 0, SEEK_SET);
	if (lseek_res < 0){
		err(1, "lseek");
	}
	ssize_t read_res;
	while ((read_res = read(fd, &buff, sizeof(buff))) > 0){
		printf("%d ", buff);
	}
	if (read_res < 0){
		err(2, "read");
	}
	printf("\n");
}
