#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <err.h>

struct header {
	uint16_t magic;
	uint16_t filetype;
	uint32_t count;
}__attribute__((packed));

int main(int argc, char* argv[]) {
	// Checks: number orf args == 4
	// For the sizes of the files
	if (argc != 4) { errx(1, "usage"); }

	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0) {
		err(2, "stat");
	}
	off_t size_list = statbuf.st_size;
	if (statbuf.st_size % sizeof(uint16_t)) {
		errx(3, "format size of list");
	}
	if (stat(argv[2], &statbuf) < 0) {
		err(2, "stat");
	}
	off_t size_data = statbuf.st_size;
	if (statbuf.st_size % sizeof(uint32_t)) {
		errx(3, "format size of data");
	}

	int fd_list = open(argv[1], O_RDONLY);
	if (fd_list < 0) { err(5, "open"); }
	int fd_data = open(argv[2], O_RDONLY);
	if (fd_data < 0) { err(5, "open"); }
	int fd_out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(5, "open"); }

	// Check the headers. Also, the number of elements in
	// the list file should be <= the number of elements in the data file 
	// Check if the variable 'count' actally correspons 
	// to the numbers of elements in the two files 
	struct header header_list;	
	struct header header_data;	

	if (read(fd_list, &header_list, sizeof(struct header)) < 0) { err(4, "read"); }
	if (read(fd_data, &header_data, sizeof(struct header)) < 0) { err(4, "read"); }
	// printf("%d\n", header_list.count);
	// printf("%d\n", header_data.count);

	uint16_t magic = 0x5A4D;
	if (header_list.magic != magic || header_data.magic != magic) { errx(8, "format magic"); }
	if ((size_list - 8) / 2 != header_list.count 
			|| (size_data - 8) / 4 != header_data.count) {
			// printf("%d\n", (int)(size_list - 8) / 2);
			// printf("%d\n", (int)(size_data - 8) / 4);
			errx(8, "format count");
	}
	if (header_list.count > header_data.count) {
		errx(8, "format sizes of list data");
	}
	if (header_list.filetype != 1 
			|| header_data.filetype != 2) {
		errx(8, "format filetype");
	}

	// Algorithm: find the biggest offset in output.bin as 
	// defined by the elements of the list file 
	// Create the output file by filling it with 0
	// (stopping at the max value for offset)
	// Then read each uint16_t from list
	// Read the next number in data.bin as well. 
	// lseek in out.bin and write the data.bin entry
	int read_res;
	uint16_t list_buff;
	uint16_t MAX_OFFSET = 0;
	while((read_res = read(fd_list, &list_buff, sizeof(list_buff))) > 0) {
		if (list_buff > MAX_OFFSET) {
			// printf("max offset %d\n", MAX_OFFSET);
			MAX_OFFSET = list_buff;
		}
	}
	if (read_res < 0) { err(11, "read"); }
	if (lseek(fd_list, 8, SEEK_SET) < 0) { err(18, "lseek"); }

	uint64_t zero = 0;
	// printf("max offset %d\n", MAX_OFFSET);
	for(uint16_t i = 0; i < MAX_OFFSET; i++) {
		printf("loop\n");
		if(write(fd_out, &zero, sizeof(zero)) < 0) {
			err(21, "write");
		}
	}
	
	uint32_t data_buff;
	uint64_t out_write;
	while((read_res = read(fd_list, &list_buff, sizeof(list_buff))) > 0) {
		if (read(fd_data, &data_buff, sizeof(data_buff)) < 0) {
			err(19, "read");
		}
		printf("data buff %d\n", data_buff);
		if (lseek(fd_out, list_buff * sizeof(uint64_t), SEEK_SET) < 0) {
			err(23, "lseek");
		}

		out_write = data_buff;
		if (write(fd_out, &out_write, sizeof(out_write)) < 0) {
			err(25, "write");
		}
	}

	if (read_res < 0) { err(11, "read"); }

	close(fd_list);
	close(fd_data);
	close(fd_out);
}
