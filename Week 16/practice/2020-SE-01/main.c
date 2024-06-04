#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// for each file, keep its element size in bytes 
int file_info[5];

struct interval {
	uint16_t start_elem_number;
	uint16_t number_of_elements;
}__attribute__((packed));

struct set {
	struct interval intervals[4];
}__attribute__((packed));

void validate_file(int fd, int element_size_in_bytes) {
		// The file size for each file should be right 
		// and the number of elements declared on the 5th and 6th byte should be the actual number of elements 
		struct stat buff;
		if (fstat(fd, &buff) < 0) {
			err(2, "Couldn't stat");
		}

		if (buff.st_size < 16 || (buff.st_size - 16) % element_size_in_bytes != 0) {
			errx(3, "The file format for is wrong");
		}
		if (lseek(fd, 4, SEEK_SET) < 0) {
			err(5, "lseek");
		}
		uint16_t expected_number_of_elements;
		if (read(fd, &expected_number_of_elements, sizeof(expected_number_of_elements)) < 0) {
			err(7, "read");
		}
		uint16_t actual_number_of_elements = (buff.st_size - 16) / element_size_in_bytes;
		if ( actual_number_of_elements != expected_number_of_elements) {
			errx(8, "The number of elements doesnt correspond for fd %d\nExpected: %d Got: %d", fd, expected_number_of_elements, actual_number_of_elements);
		}
}

int open_file_for_reading(char* file_name) {
	int fd = open(file_name, O_RDONLY);
	if (fd < 0) { err(4, "Couldn't open %s", file_name); }
	return fd;
}

void handle_interval_64(struct interval interval, int fd, int fd_crucifixus) {
	int curr_element_size = 8;
	if(lseek(fd, 16 + interval.start_elem_number * curr_element_size, SEEK_SET) < 0) {
		err(11, "lseek");
	}
	uint64_t buff;
	for(int i = 0; i < interval.number_of_elements; i++) {
		int read_res = 0;
		if ((read_res = read(fd, &buff, sizeof(buff))) < 0) {
			err(12, "Error when reading");
		}
		if (write(fd_crucifixus, &buff, read_res) < 0) {
			err(13, "write to crucifixus");
		}
	}
}

void handle_interval_32(struct interval interval, int fd, int fd_crucifixus) {
	int curr_element_size = 4;
	if(lseek(fd, 16 + interval.start_elem_number * curr_element_size, SEEK_SET) < 0) {
		err(11, "lseek");
	}
	uint32_t buff;
	for(int i = 0; i < interval.number_of_elements; i++) {
		int read_res = 0;
		if ((read_res = read(fd, &buff, sizeof(buff))) < 0) {
			err(12, "Error when reading");
		}
		if (write(fd_crucifixus, &buff, read_res) < 0) {
			err(13, "write to crucifixus");
		}
	}
}
void handle_interval_16(struct interval interval, int fd, int fd_crucifixus) {
	int curr_element_size = 2;
	if(lseek(fd, 16 + interval.start_elem_number * curr_element_size, SEEK_SET) < 0) {
		err(11, "lseek");
	}
	uint16_t buff;
	for(int i = 0; i < interval.number_of_elements; i++) {
		int read_res = 0;
		if ((read_res = read(fd, &buff, sizeof(buff))) < 0) {
			err(12, "Error when reading");
		}
		if (write(fd_crucifixus, &buff, read_res) < 0) {
			err(13, "write to crucifixus");
		}
	}
}
void handle_interval_8(struct interval interval, int fd, int fd_crucifixus) {
	int curr_element_size = 1;
	if(lseek(fd, 16 + interval.start_elem_number * curr_element_size, SEEK_SET) < 0) {
		err(11, "lseek");
	}
	uint8_t buff;
	for(int i = 0; i < interval.number_of_elements; i++) {
		int read_res = 0;
		if ((read_res = read(fd, &buff, sizeof(buff))) < 0) {
			err(12, "Error when reading");
		}
		if (write(fd_crucifixus, &buff, read_res) < 0) {
			err(13, "write to crucifixus");
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 7) {
		errx(1, "Usage: %s <postfix> <prefix> <infix> <suffix> <crucifixus>", argv[0]);
		// Each file has a header of 16 bytes (5th and 6th represent a uint16_t for 
		// the number of elements in the file)
	}

	file_info[0] = 2;
	file_info[1] = 4;
	file_info[2] = 1;
	file_info[3] = 2;
	file_info[4] = 8;

	int fd_affix = open_file_for_reading(argv[1]);

	validate_file(fd_affix, file_info[fd_affix - 3]);
	int fd_postfix = open_file_for_reading(argv[2]);

	validate_file(fd_postfix, file_info[fd_postfix - 3]);
	int fd_prefix = open_file_for_reading(argv[3]);

	validate_file(fd_prefix, file_info[fd_prefix - 3]);
	int fd_infix = open_file_for_reading(argv[4]);

	validate_file(fd_infix, file_info[fd_infix - 3]);
	int fd_suffix = open_file_for_reading(argv[5]);

	validate_file(fd_suffix, file_info[fd_suffix - 3]);
	int fd_crucifixus = open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_crucifixus < 0) { err(9, "Coudln't open result file"); }

	// Algorithm: Read a set from affix at once. For each member of the set, lseek to the 
	// position in the file and read as many elements as the interval says 
	if (lseek(fd_affix, 16, SEEK_SET) < 0) { err(8, "lseek in affix"); } // skip the header
	struct set set_buff; 
	ssize_t read_result;
	while((read_result = read(fd_affix, &set_buff, sizeof(set_buff))) > 0) {
		handle_interval_32(set_buff.intervals[0], fd_postfix, fd_crucifixus);
		handle_interval_8(set_buff.intervals[1], fd_prefix, fd_crucifixus);
		handle_interval_16(set_buff.intervals[2], fd_infix, fd_crucifixus);
		handle_interval_64(set_buff.intervals[3], fd_suffix, fd_crucifixus);

		// A more general solution that uses a bigger buffer 
		// didn't work properly :( 
		// (probably because of endianness)
		// handle_interval(set_buff.intervals[0], fd_postfix, fd_crucifixus);
		// handle_interval(set_buff.intervals[1], fd_prefix, fd_crucifixus);
		// handle_interval(set_buff.intervals[2], fd_infix, fd_crucifixus);
		// handle_interval(set_buff.intervals[3], fd_suffix, fd_crucifixus);
	}
	if (read_result < 0) { 
		err(10, "read");
	}

	close(fd_affix);
	close(fd_postfix);
	close(fd_prefix);
	close(fd_infix);
	close(fd_suffix);
	close(fd_crucifixus);
}
