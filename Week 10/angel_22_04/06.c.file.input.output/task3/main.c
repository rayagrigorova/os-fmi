// Реализирайте команда wc, с един аргумент подаден като входен параметър
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

void print_text(char* text) {
	int n_written = write(1, text, strlen(text));
	if (n_written < 0) {
		err(1, "could not write text");
	}
	if (n_written != (int)strlen(text)) {
		errx(1, "could not write text at once");
	}
}

int main (int argc, char* argv[]){
	int fd1;
	char c;
	int lines = 0;
	int words = 0;
	int chars = 0;

	if (argc != 2) {
		errx(1, "please supply a file as argument");
	}

	if ( (fd1 = open(argv[1], O_RDONLY)) == -1 ) {
		err(1, "could not open %s", argv[1]);
	}

	int n_read;
	while ( (n_read = read(fd1, &c, sizeof(c))) == sizeof(c) ) {
		if (c=='\n') {
			lines++;
			words++;
		}

		if (c==' ') {
			words++;
		}

		chars++;
	}
	if (n_read < 0) {
		err(1, "could not read data from %s", argv[1]);
	}
	
	if (close(fd1) < 0) { err(1, "could not close %s", argv[1]); }

	char text[256];
	snprintf(
		text, sizeof(text),
		"File %s has:\n%d number of lines.\n%d number of words.\n%d number of bytes.\n",
		argv[1], lines, words, chars
	);
	print_text(text);
}
