#include <string.h>
#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdbool.h>

void print_char_range(char* buff, ssize_t range_start, ssize_t range_end) {
	for (ssize_t i = range_start - 1; i < range_end; i++) {
		char curr = buff[i];
		if(write(1, &curr, 1) < 0) {
			err(2, "write");
		}
	}
	if(write(1, "\n", 1) < 0) {
		err(2, "write");
	}
}

ssize_t count_delims(char* buff, size_t line_length, char delim) {
	ssize_t ctr = 0; 
	for(size_t i = 0; i < line_length; i++) {
		if(buff[i] == delim) {
			ctr++;
		}
	}
	return ctr;
}

void print_field_range(char* buff, ssize_t line_length, char delim, ssize_t range_start, ssize_t range_end){
	// Skip to the range_start - 1 th delimeter 
	ssize_t ctr = 1; // This counter is for the number of fields passed 
	ssize_t i = 0;
	for(; i < line_length && ctr < range_start; i++) {
		if (buff[i] == delim){ ctr++; }
	}
	if (i >= line_length) { errx(11, "Invalid range"); }
	for(; i < line_length; i++) {
		if (buff[i] == delim){
			ctr++;
		}
		if (ctr > range_end) {
			break; 
		}
		if(write(1, &buff[i], 1) < 0) { err(5, "write"); }
	}
	if (write(1, "\n", 1) < 0) {
		err(13, "write");
	}
}

bool read_next_line(char* buff, size_t* line_length) {
	// Read in the buff while there is still something to read 
	// (read char by char) 
	char current = '\0';
	*line_length = 0; // init with 0
	ssize_t read_res = read(0, &current, 1); 
	if (read_res < 0) { err(1, "read"); }
	if (read_res == 0) {return false;} // If no chars are read during the first read
	buff[*line_length] = current;
	(*line_length)++;
	while((read_res = read(0, &current, 1)) > 0){
		if (current == '\n') { break; }	
		buff[*line_length] = current;
		(*line_length)++;
	}
	if (read_res < 0) { err(1, "read"); }
	return true;
}

bool validate_number_range(char* number_range, ssize_t* range_start, ssize_t* range_end) {
	size_t len = strlen(number_range);
	if (len != 1 && len != 3){
		return false;
	}
	if (len == 1 && number_range[0] >= '1' && number_range[0] <= '9') {
		*range_start = number_range[0] - '0';
		*range_end = number_range[0] - '0';
		return true;
	}
	if (len == 3 && number_range[0] >= '1' && number_range[0] <= '9'
			&& number_range[2] >= '1' && number_range[2] <= '9'
			&& number_range[0] <= number_range[2] 
			&& number_range[1] == '-') {
		*range_start = number_range[0] - '0';
		*range_end = number_range[2] - '0';
		return true;
	}
	return false;
}

int main(int argc, char* argv[]) {
	if(argc != 3 && argc != 5) {
		errx(1, "Usage: %s -c <number>/<number>-<number> or %s -d <string> -f <number>/<number>-<number>\n", argv[0], argv[0]);
	}

	ssize_t range_start = 0;
	ssize_t range_end = 0;

	// 0. Input validation: 
	// -c <number>/<number>-<number>
	// -d <string> -f <number>/<number>-<number>
	if (argc == 3 && strcmp("-c", argv[1]) == 0 && validate_number_range(argv[2], &range_start, &range_end)){
		// print_char_range();
	}
	else if (argc == 5 && strcmp("-d", argv[1]) == 0 && strcmp("-f", argv[3]) == 0 && validate_number_range(argv[4], &range_start, &range_end)) {
		// print_fields();
	}
	else {
		errx(1, "Usage: %s -c <number>/<number>-<number> or %s -d <string> -f <number>/<number>-<number>\n", argv[0], argv[0]);

	}


	// Implement reading line by line using read()
	char buff[1024];
	size_t line_length = 0;
	while (read_next_line(buff, &line_length)){
		// If -c is used, determine start and end range and print from it 
		// If the option is -d, find the delimeter and again, use a range 
		// for the columns (count the columns before starting the parsing)

		if (argv[1][1] == 'c') {
			ssize_t used_range_end = MIN(line_length, range_end);
			if (used_range_end < range_start) { errx(9, "Invalid range"); }
			print_char_range(buff, range_start, used_range_end);
		}

		else {
			char delim = argv[2][0];
			ssize_t used_range_end = MIN(range_end, count_delims(buff, line_length, delim) + 1);
			if (used_range_end < range_start) { errx(9, "Invalid range"); }
			print_field_range(buff, line_length, delim, range_start, used_range_end);
		}
	}
}
