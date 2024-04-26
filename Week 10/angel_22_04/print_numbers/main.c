#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>
#include <stdio.h>

void print_min_max(int fd, bool flag);
void print_all(int fd);

// The flag will be set to 0 when the min should be found and to 1 when searching for max
int main(int argc, char* argv[]){
	if (argc != 3 ){
		errx(1, "The  number of arguments is wrong");
	}
	const char MIN[] = "--min";
	const char MAX[] = "--max";
	const char PRINT[] = "--print";
	// If the first argument doesn't match any of the const trings, then it is in the wrong format
	if (strcmp(MIN, argv[1]) != 0 && strcmp(MAX, argv[1]) != 0
			&& strcmp(PRINT, argv[1]) != 0 ) {
		errx(2, "Incorrect format for first argument: must be --min, --max or --print");
	}

	int fd = open(argv[2], O_RDONLY);
	if (fd < 0){
		err(3, "There was an error trying to open %s", argv[2]);
	}

	// Stat the file to check if its size in bytes is divisible by 2. If not, then it doesnt consist fo uint16_t numbers
	struct stat buffer;
	int stat_result = fstat(fd, &buffer);
	if (stat_result < 0){
		err(4, "Could not stat file");
	}
	if (buffer.st_size % 2){
		errx(5, "The file should consist of uint16_t numbers");
	}

	if (!strcmp(MIN, argv[1])){
		print_min_max(fd, 0);
	}

	else if (!strcmp(MAX, argv[1])){
		print_min_max(fd, 1);
	}

	else {
		print_all(fd);
	}
	if (close(fd) < 0){
		err(10, "There was an error when trying to close %s", argv[2]);
	}
	return 0;
}

void print_min_max(int fd, bool flag){
	uint16_t result_number = 0; // The min/max number will be saved in this variable
	bool result_num_initialized = false; // A flag that shows if the result number was set yet
	uint16_t buff; // A buffer for the currently processed number from the file
	ssize_t result; // A variable for the number of bytes read
	char string_repr[256];

	while ((result = read(fd, &buff, sizeof(buff))) > 0){
		if (result < sizeof(buff)){
			err(6, "Could not read all the data at once");
		}
		bool compare_res = ((flag) ? (buff > result_number) : (buff < result_number));
		if (!result_num_initialized || compare_res){
			result_number = buff;
			result_num_initialized = true;
		}
	}

	if (result < 0){
		err(7, "Could not read from numbers file");
	}
	int len = snprintf(string_repr, sizeof(string_repr), "%d\n", result_number);
	if (write(1, &string_repr, len) < 0){
		err(8, "Could not write number to stdout");
	}
}

void print_all(int fd){
	uint16_t buff;
	char string_repr[256];
	ssize_t result;
	while  ( (result = read(fd, &buff, sizeof(buff))) > 0 ){
		int len = snprintf(string_repr, sizeof(string_repr), "%d\n", buff);
		if(write(1, &string_repr, len) < 0){
			err(8, "Could not write number to stdout");
		}
	}
	if (result < 0){
		err(7, "Could not read from numbers file");
	}
}

