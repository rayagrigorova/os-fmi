/*
	Реализирайте команда head без опции (т.е. винаги да извежда
	на стандартния изход само първите 10 реда от съдържанието на
	файл подаден като първи параматър)
*/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[])
{
	int fd1;
	int i = 0;

	if (argc != 2) {
		errx(2, "please supply a file as argument");
	}

	if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
		err(1, "could not open file %s", argv[1]);
	}

	char c;
	int n_read;
	while ((n_read = read(fd1, &c, sizeof(c))) == sizeof(c)) {
		if (c == '\n') {
			i=i+1;
		}

		if (write(1, &c, 1) < 0) {
			err(1, "could not write to %s", argv[1]);
		}

		if (i == 10) {
			break;
		}
	}
	if (n_read < 0) {
		err(1, "could not read from %s", argv[1]);
	}

	if (close(fd1) < 0) { err(1, "could not close %s", argv[1]); }
	exit(0);
}
