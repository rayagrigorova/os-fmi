#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
	if (argc != 4) { errx(1, "usage"); } 
	// Algorithm: put all the zero byte 
	// indices from the index file in an array 
	
	// Write all the indices of zeros to an array 
	struct stat statbuf;
	if (stat(argv[3], &statbuf) < 0) { err(2, "stat"); }
	off_t file_size = statbuf.st_size;

	uint32_t* indices = malloc(file_size * sizeof(uint32_t));

	int fd_idx = open(argv[3], O_RDONLY);
	if (fd_idx < 0) { err(3, "open"); } 

	int read_res; 
	uint32_t buff;
	int ctr = 0;
	while((read_res = read(fd_idx, &buff, sizeof(buff))) < 0) {
		indices[ctr++] = buff;
	}
	if (read_res < 0) { err(5, "read"); } 

	// Now do a binary search: get the middle index 	
	int fd_dict = open(argv[2], O_RDONLY);
	if (fd_dict < 0) { err(3, "open"); } 

	int left = 0;
	int right = file_size;
	while(left < right) {
		int mid = left + (right - left) / 2;
		// Get the word in the middle(also validate it)
		// off_t saved_offset = lseek(fd_dict, 0, SEEK_CUR);
		// if (saved_offset < 0) { err(14, "lseek"); } 

		char word[64];
		char current_char;
		int len = 0;
		while((read_res = read(fd_dict, &current_char, sizeof(current_char))) > 0) {
			if (current_char == '\0' || current_char == '\n') {
				break;
			}
			if (len >= 64) { errx(9, "dict format"); } 	
			word[len++] = current_char;
		}
		if (read_res < 0) { err(5, "read"); } 
		word[len] = '\0';

		int strcmp_res = strcmp(argv[1], word);
		if (strcmp_res < 0) {
			// The word is smaller, so move left
			right = mid - 1;
		}
		else if (strcmp_res > 0) {
			left = mid + 1;
		}
		else {
			// found the word 
			// print the definition
		while((read_res = read(fd_dict, &current_char, sizeof(current_char))) > 0) {
			if (current_char == '\0') { break; } 
			if(write(1, &current_char, sizeof(current_char)) < 0) {
				err(19, "write"); 
			}
		}
		if (read_res < 0) { err(5, "read"); } 
		}
	}
	
	free((void*)indices);
}
