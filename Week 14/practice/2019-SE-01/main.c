#include <stdint.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <err.h>

// none of this works 

const char TEMP_FILE_NAME[] = "temp_file.txt";

struct UID_duration{
	uint32_t UID;
	uint32_t duration;
}__attribute__((packed));

// Struct size: 16
struct login_info{
	uint32_t UID;
	uint16_t saved1;
	uint16_t saved2;
	uint32_t time1;
	uint32_t time2;
}__attribute__((packed));

void swap_records(int fd_copy, struct UID_duration val1, int ind1, struct UID_duration val2, int ind2);

double get_average(int fd) {
	int ctr = 0;
	int read_res;
	struct login_info buff;
	double result = 0;

	while((read_res = read(fd, &buff, sizeof(buff))) > 0) {
		uint32_t duration = buff.time2 - buff.time1;
		result += duration;
		ctr++;
	}

	if (read_res < 0) {
		err(3, "Error from read (average calculation)");
	}
	if (lseek(fd, 0, SEEK_SET) < 0) {
		err(4, "lseek");
	}
	if (ctr == 0) { return 0; }
	return result / ctr;
}

double get_dispersion(int fd) {
	int ctr = 0;
	int read_res;
	struct login_info buff;
	double result = 0;
	double average = get_average(fd);

	while((read_res = read(fd, &buff, sizeof(buff))) > 0) {
		uint32_t duration = buff.time2 - buff.time1;
		result += (duration - average) * (duration - average);
		ctr++;
	}

	if (read_res < 0) {
		err(3, "read error: dispersion calculation");
	}
	if (lseek(fd, 0, SEEK_SET) < 0) {
		err(4, "lseek");
	}
	if (ctr == 0) { return 0; }
	return result / ctr;
}

// Only copy UID and the calculated duration
// This function returns the copy_file fd and saves the size of 
// the file into the number_of_records parameter 
int copy_file(int fd, int* number_of_records) {
	int fd_copy = open(TEMP_FILE_NAME, O_WRONLY | O_CREAT, 0666);
	if (fd_copy < 0) { err(5, "Failed to create temp file"); }

	int read_res;
	struct login_info buff;
	int ctr = 0;

	while((read_res = read(fd, &buff, sizeof(buff))) > 0) {
		uint32_t duration = buff.time2 - buff.time1;
		if (write(fd_copy, &buff.UID, sizeof(buff.UID)) < 0) {
			err(5, "write");
		}
		if (write(fd_copy, &duration, sizeof(duration)) < 0) {
			err(5, "write");
		}
		ctr++;
	}

	if (read_res < 0) {
		err(3, "read error when copying to file");
	}

	if (lseek(fd_copy, 0, SEEK_SET) < 0) {
		err(4, "lseek");
	}

	*number_of_records = ctr;
	return fd_copy;
}

void sort_by_UID(int fd_copy, int number_of_records) {
	// Perform insertion sort to minimize the number of swaps
	struct UID_duration min_elem;
	int read_res;

	for(int i = 0; i < number_of_records - 1; i++) {
		if (lseek(fd_copy, i * sizeof(struct UID_duration), SEEK_SET) < 0) {
			err(4, "lseek");
		}

		// position where the min element is 
		int min_pos = i * sizeof(struct UID_duration);
		struct UID_duration first_unsorted; // Save the record on i-th position
		// Read the first record from the unsorted part of  the array
		if ((read_res = read(fd_copy, &first_unsorted, sizeof(struct UID_duration))) < 0) {
			err(8, "read error when sorting by UID (first record), i = %d", i);
		}
		else if (read_res != sizeof(struct UID_duration)) { errx(40, "read didn't read as many bytes as expected"); }
		min_elem = first_unsorted;

		int curr_pos = min_pos + sizeof(struct UID_duration);
		struct UID_duration curr;

		while((read_res = read(fd_copy, &curr, sizeof(struct UID_duration))) > 0) {
			if (read_res != sizeof(struct UID_duration)) { errx(40, "read didn't read as many bytes as expected"); }
			if (curr.UID < min_elem.UID) {
				min_pos = curr_pos;	
				min_elem = curr;
			}
			curr_pos += sizeof(struct UID_duration);
		}
		if (read_res < 0) {
			err(8, "read error when sorting by UID");
		}
		if (min_pos != i) {
			swap_records(fd_copy, first_unsorted, i * sizeof(struct UID_duration), min_elem, min_pos);
		}
	}

	if (lseek(fd_copy, 0, SEEK_SET) < 0) { err(13, "lseek"); }
}

void swap_records(int fd_copy, struct UID_duration val1, int ind1, struct UID_duration val2, int ind2) {
	// Write val1 to ind2 and then val2 to ind1
	if (lseek(fd_copy, ind2, SEEK_SET) < 0) {
		err(8, "lseek");
	}
	if (write(fd_copy, &val1, sizeof(val1)) < 0) {
		err(9, "write");
	}
	if (lseek(fd_copy, ind1, SEEK_SET) < 0) {
		err(8, "lseek");
	}
	if (write(fd_copy, &val2, sizeof(val2)) < 0) {
		err(9, "write");
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Usage: %s <bin_file_name>", argv[0]);
	}
	// Check if the file size is divisible by 16
	struct stat buff_stat;
	if (stat(argv[1], &buff_stat) < 0) {
		err(7, "stat");
	}
	if (buff_stat.st_size % sizeof(struct login_info)) {
		errx(8, "Incorrect file format for %s", argv[1]);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		err(2, "open");
	}
	// duration =: time2 - time1 
	// For each user:
	// if user had a session with length > D, print UID and length 
	// of longest session

	// Steps: 
	// 1. and 2. Calculate average (go through the whole file) and calculate dispersion 

	double dispersion = get_dispersion(fd);

	int number_of_records = 0;
	int fd_copy = copy_file(fd, &number_of_records);
	printf("fd copy is %d num records: %d\n", fd_copy, number_of_records);
 	sort_by_UID(fd_copy, number_of_records);

 	struct UID_duration buff;
 	// Handle corner cases
 	if (number_of_records == 0) { return 0; }
 	if (number_of_records == 1) {
 		if (read(fd_copy, &buff, sizeof(buff)) < 0) {
 			err(14, "read error when handling case with 1 record");
 		}
 		if (buff.duration * buff.duration > dispersion) {
 			// Legal to use? 
 			dprintf(1, "UID %d longest session %d\n", buff.UID, buff.duration);
 		}
 		return 0;
 	}
 
 	// go through the copy file 
	// for each record check the UID: if it's different 
 	// from the previously initialized one, check if the 
 	// longest session duration for the respective user ^ 2 
 	// is greater than the dispersion
 	// If yes, print the said longest duration and the UID
 	// Initialize the UID with the first record in the 
 	// copy file and go from there (I handle the corner case with 1 record above)
 	if (read(fd_copy, &buff, sizeof(buff)) < 0) {
 		err(14, "read error when initializing during copy file read");
 	}
 	uint32_t curr_UID = buff.UID;
 	uint32_t longest_duration_for_curr_UID = buff.duration;
 	int read_res;
 
 	while((read_res = read(fd_copy, &buff, sizeof(buff))) > 0) {
 		if(buff.UID != curr_UID) {
 			if (longest_duration_for_curr_UID * longest_duration_for_curr_UID > dispersion) {
 				dprintf(1, "UID %d longest session %d\n", buff.UID, buff.duration);
 			}
 			curr_UID = buff.UID;
 			longest_duration_for_curr_UID = buff.duration;
 		}
 		else {
 			longest_duration_for_curr_UID = MAX(longest_duration_for_curr_UID, buff.duration);
 		}
 	}
 	if (read_res < 0) {
 		err(3, "read error after readingcopy file");
 	}
 
 	close(fd);
 	close(fd_copy);

	execlp("rm", "rm", TEMP_FILE_NAME, (char*) NULL);
	err(20, "exec");
}
