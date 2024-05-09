#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

void read_from_fd(bool line_numbering, uint32_t* line_number, int fd);

int main(int argc, char* argv[]) {
	// For each argument (after the first one if it's '-n')
	// if the argument is -, then read from stdin and echo the result 
	// else, open the file and read from it
	// There will be 2 functions that read from a file - one 
	// that reads line by line and one that reads in a buffer 
	uint32_t arg_index = 1;	
	uint32_t line_number = 1;	
	bool line_numbering = false;
	if (strcmp("-n", argv[1]) == 0) {
		arg_index++;
		line_numbering = true;
	}
	for (; arg_index < argc; arg_index++){
		if(strcmp("-", argv[arg_index]) == 0){
			// Read from stdin until EOF
			read_from_fd(line_numbering, &line_number, 0);
		}
		else{
			int fd = open(argv[arg_index], O_RDONLY | O_CREAT);
			if (fd < 0){
				err(11, "Error when trying to open %s", argv[arg_index]);
			}
			read_from_fd(line_numbering, &line_number, fd);
		}
	}
}

const char SPACE = ' ';
const char CR = '\r';
void read_from_fd(bool line_numbering, uint32_t *line_number, int fd) {
	// char buff[256];
	char buff;
	int read_result;
	char number_buff[32];
	if (line_numbering){
		snprintf(number_buff, sizeof(number_buff), "%d", *line_number);
		(*line_number)++;
		if(write(1, number_buff, strlen(number_buff)) < 0){
			err(3, "Error when writing line number");
		}
		if(write(1, &SPACE, sizeof(SPACE)) < 0){
			err(3, "Error when writing line number");
		}
	}
	while((read_result = read(fd, &buff, sizeof(buff))) > 0){
		if (write(1, &buff, read_result) < 0){
			err(1, "Error writing to stdout");
		}
		if (line_numbering && buff == '\n'){
			snprintf(number_buff, sizeof(number_buff), "%d", *line_number);
			(*line_number)++;
			if(write(1, number_buff, strlen(number_buff)) < 0){
				err(3, "Error when writing line number");
			}
			if(write(1, &SPACE, sizeof(SPACE)) < 0){
				err(3, "Error when writing line number");
			}
		}
	}
	if (read_result < 0){
		err(2, "Error when reading");
	}
	if(write(1, &CR, sizeof(CR)) < 0){
		err(3, "Error when writing line number");
	}
	(*line_number)--;
}

