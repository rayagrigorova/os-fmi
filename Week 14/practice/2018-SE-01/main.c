#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <err.h>

const int BUFF_SIZE = 256; 

void delete_chars(char* buff, ssize_t buff_size, char* set1);
void delete_chars(char* buff, ssize_t buff_size, char* set1) {
	// Create a boolean array that represents if each character should be deleted 
	bool should_be_deleted[256];
	// Initialize the array
	for(int i = 0; i < 256; i++) {
		should_be_deleted[i] = false;
	}
	// Each character from set1 should be deleted 
	for(size_t i = 0; i < strlen(set1); i++) {
		should_be_deleted[(int)set1[i]] = true;
	}
	ssize_t pos = 0;
	char result[BUFF_SIZE];
	for(int i = 0; i < buff_size; i++) {
		if(should_be_deleted[(int)buff[i]]){
			continue; // Don't add characters that should be deleted 
		}
		result[pos++] = buff[i];
	}

	if(pos == 0){ return; }; // nothing to print
	if (write(1, result, pos) < 0) { // Directly print the result since the removal is the only action to be executed 
		err(5, "write");
	}
}

ssize_t squeeze_chars(char* buff, ssize_t buff_size, char* set1);
ssize_t squeeze_chars(char* buff, ssize_t buff_size, char* set1) {
	
	bool should_be_squeezed[256];
	// Same as with the above function
	for(int i = 0; i < 256; i++) {
		should_be_squeezed[i] = false;
	}
	for(size_t i = 0; i < strlen(set1); i++) {
		should_be_squeezed[(int)set1[i]] = true;
	}

	ssize_t ptr_write = 1;
	ssize_t ptr_read = 1;
	// For regular chars: just copy them
	// For characters that have to be squeezed: Write a character only when it first consecutively occurrs
	// If it's not its first occurance, skip it
	char curr = buff[0];
	for(int i = 1; i < buff_size; i++) {
		// Simply write the next character if it shouldn't be squeezed 
		
		
		
		if (!should_be_squeezed[(int)buff[ptr_read]]) {
			
			if (ptr_write != ptr_read) {
				buff[ptr_write] = buff[ptr_read];
			}
			ptr_write++;
			ptr_read++;
			continue;
		}	
		// Skip over repeated chars 
		if (buff[ptr_read] == curr) {
			
			ptr_read++;
			continue;
		}
		// The current character changed, so write it and move the pointers
		else {
			
			curr = buff[ptr_read];
			if (ptr_write != ptr_read) {
				buff[ptr_write] = buff[ptr_read];
			}
			ptr_write++;
			ptr_read++;
		}
	}
	
	return ptr_write;
}

void replace_from_set2(char* set1, char* set2, char* buff, ssize_t buff_len);
void replace_from_set2(char* set1, char* set2, char* buff, ssize_t buff_len) {
	// determine replacements 
	char replacements_table[256];
	for(int i = 0; i < 256; i++){
		replacements_table[i] = (char)i;
	}
	for(size_t i = 0; i < strlen(set2); i++) {
		replacements_table[(int)set1[i]] = set2[i];
	}
	// perform replacements 
	for(int i = 0; i < buff_len; i++) {
		buff[i] = replacements_table[(int)buff[i]];
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3 || argc > 4) {
		errx(1, "Usage: %s [OPTION] SET1 [SET2]", argv[0]);
	}
	// Use a pointer to each of the sets for simplicity
	char* set1 = NULL;
	char* set2 = NULL;
	// Possible usages of the program: 
	// <set1> <set2> 
	// <option> <set1> 
	// <option> <set1> <set2> 
	// In the third use case, if the option is -d, 
	// set2 can be ignored (the original 'tr' throws an error)
	// However, if the option is -s, apply both the transformation from 
	// use case 1 and -s (first -s, then the replacement from symbols // from set1 with ones from set2)
	char option = '0';
	if (argc == 3) {
		// <set1> <set2> or <option> <set1> 
		// Case <option> <set1>:
		if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-s") == 0) {
			option = argv[1][1];

			set1 = argv[2];
			set2 = argv[3];
		}
		// Case <set1> <set2>:
		else {
			if (strlen(argv[1]) != strlen(argv[2])) {
				errx(2, "The two sets should be of equal length");
			}
			set1 = argv[1];
			set2 = argv[2];
		}
	}

	// <option> <set1> <set2> 
	else {
		if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-s") == 0) {
			option = argv[1][1];
		}
		else {
			errx(3, "Invalid option");
		}
		if (strlen(argv[2]) != strlen(argv[3])) {
			errx(2, "The two sets should be of equal length");
		}
	}
	// If the program is used like so: ./main -d <set1> <set2>, set2 will be ignored (instead of throwing an error 
	// like in the original implementation (because of the task condition)

	// If the option is -d, then simply delete all ocurrances of characters from set1 and exit
	// If the option is -s, overwrite the old string by using two pointers 
	// Finally, replace all ocurrances of characters from <set1> with ones from <set2> (if set2 is given). To do this 
	// without passing through the buffered string many times, I will
	// use a boolean array of size 256
	char buff[BUFF_SIZE];
	ssize_t read_res = 0;
	if ((read_res = read(0, buff, sizeof(buff))) < 0) {
		err(4, "Eror when using read()");
	}

	if (option == 'd') {
		delete_chars(buff, read_res, set1);
		return 0;
	}
	// The old buffer will be overritten with the new chars
	ssize_t new_size = read_res;
	if (option == 's') {
		new_size = squeeze_chars(buff, read_res, set1); 
		
		if (argc == 3) { // the program is used as -s <set1> 
			if (write(1, buff, new_size) < 0) {err(7, "write");}
			return 0;
		} 
	}
	// replace from set1 with set2 	
	replace_from_set2(set1, set2, buff, new_size);
	if (write(1, buff, new_size) < 0) {err(7, "write");}
}
