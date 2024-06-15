#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int comp(const void* a, const void* b) {
	uint32_t a_int = *(const uint32_t*)a;
	uint32_t b_int = *(const uint32_t*)b;

	if (a_int < b_int) { return -1; }
	if (a_int > b_int) { return 1; }
	return 0;
}

void merge(int fd_1, int fd_2, char* new_file_name) {
	int fd_out = open(new_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(3, "open"); }

	uint32_t buff_file1;
	uint32_t buff_file2;
	int read_res_1 = 0;
	int read_res_2 = 0;
	while((read_res_1 = read(fd_1, &buff_file1, sizeof(buff_file1))) > 0 
			&& 	(read_res_2 = read(fd_2, &buff_file2, sizeof(buff_file2))) > 0 ) {
		if (buff_file1 < buff_file2) {
			if(write(fd_out, &buff_file1, sizeof(buff_file1)) < 0) { err(11, "write"); } 
		}
		else {
			if(write(fd_out, &buff_file2, sizeof(buff_file2)) < 0) { err(11, "write"); } 
		}
	}
	if (read_res_1 < 0 || read_res_2 < 0) { err(10, "read"); } 

	if (read_res_1 > 0) { 
			if(write(fd_out, &buff_file1, sizeof(buff_file1)) < 0) { err(11, "write"); } 
	}
	if (read_res_2 > 0) { 
			if(write(fd_out, &buff_file2, sizeof(buff_file2)) < 0) { err(11, "write"); } 
	}

	while((read_res_1 = read(fd_1, &buff_file1, sizeof(buff_file1))) > 0) {
			if(write(fd_out, &buff_file1, sizeof(buff_file1)) < 0) { err(11, "write"); } 
	}
	if (read_res_1 < 0) { err(10, "read"); } 
	while((read_res_2 = read(fd_2, &buff_file2, sizeof(buff_file2))) > 0) {
			if(write(fd_out, &buff_file2, sizeof(buff_file2)) < 0) { err(11, "write"); } 
	}
	if (read_res_2 < 0) { err(10, "read"); } 

	close(fd_out);
}


int main(int  argc, char* argv[]) {
	// Idea: read half of the file, put it in an array 
	// and sort the said array using qsort() 
	// Write the uint32_t's to a new file // Do the same for the other half of the file using the same array 
	// Now, merge the two new files into a third 
	
	// Checks: 2 arguments 
	if (argc != 3) {
		errx(1, "Usage: %s <file1> <sorted_name>", argv[0]);
	}

	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0) {
		err(2, "stat");
	}

	off_t size = statbuf.st_size;
	if (size % 4) { errx(6, "wrong format"); }

	off_t first_half = size / 2;
	first_half /= sizeof(uint32_t);
	off_t second_half = size/sizeof(uint32_t) - first_half;
	printf("size: %d\n", (int)size);
	printf("first half: %d second half %d\n", (int)first_half, (int)second_half);
	printf("Size of array: %d\n", (int)(size / 2 + sizeof(uint32_t)));

	uint32_t* numbers_array = (uint32_t*)malloc(size / 2 + sizeof(uint32_t));
	if (numbers_array == NULL) { err(15, "malloc"); }

	int fd_in = open(argv[1], O_RDONLY);
	if (fd_in < 0) { err(3, "open"); }

	int read_res;
	uint32_t buff;
	int ctr = 0;

	// Read the first half of the file into an array
	while((read_res = read(fd_in, &buff, sizeof(buff))) > 0 && ctr < first_half) {
		numbers_array[ctr++] = buff;	
	}
	if (read_res < 0) { err(5, "read"); } 
	qsort(numbers_array, first_half, sizeof(uint32_t), comp);

	int fd_1 = open("file1", O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd_1 < 0) { err(9, "open");}
	// Write the first half of the sorted numbers to a file
	for(int i = 0; i < first_half; i++) {
		if(write(fd_1, &numbers_array[i], sizeof(uint32_t)) < 0) {
			err(10, "write");
		}
	}

	// Other half
	ctr = 0;
	while(ctr < second_half && (read_res = read(fd_in, &buff, sizeof(buff))) > 0) {
		numbers_array[ctr++] = buff;	
	}
	if (read_res < 0) { err(5, "read"); } 
	qsort(numbers_array, second_half, sizeof(uint32_t), comp);

	int fd_2 = open("file2", O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd_2 < 0) { err(9, "open");}

	for(int i = 0; i < second_half; i++) {
		if(write(fd_2, &numbers_array[i], sizeof(uint32_t)) < 0) {
			err(10, "write");
		}
	}

	if (lseek(fd_1, 0, SEEK_SET) < 0) { err(18, "lseek"); } 
	if (lseek(fd_2, 0, SEEK_SET) < 0) { err(18, "lseek"); } 
	merge(fd_1, fd_2, argv[2]);

	close(fd_in);	
	close(fd_1);
	close(fd_2);

	free(numbers_array);
}
