/*
	Реализирайте команда swap, разменяща съдържанието на два файла, подадени като
	входни параметри. Приемаме, че двата файла имат еднакъв брой символи. Може да
	модифицирате решението, да работи и когато нямат еднакъв брой символи.
*/

#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void copy_data(int fd_from, int fd_to) {
	char buf[4096];
	int read_size;
	while ((read_size = read(fd_from, buf, sizeof(buf))) > 0) {
		int write_size = write(fd_to, buf, read_size);
		if (write_size < 0) {
			err(1, "Error while writing");
		}
		if (write_size != read_size) {
			err(1, "could not write data at once");
		}
	}
	if (read_size < 0) {
		err(2, "Error while reading");
	}
}

int open_file(const char* filename, int flags) {
	int fd = open(filename, flags, 0666);
	if (fd < 0) {
		err(1, "could not open %s", filename);
	}
	return fd;
}

void seek_file(int fd, off_t offset, int whence) {
	int pos = lseek(fd, offset, whence);
	if (pos < 0) {
		err(1, "could not seek");
	}
}

void close_file(int fd) {
	if (close(fd) < 0) {
		err(1, "could not close");
	}
}

void clear_file(int fd) {
	seek_file(fd, 0, SEEK_SET);
	if (ftruncate(fd, 0) < 0) {
		err(1, "could not truncate");
	}
}

void delete_file(const char* name) {
	if (unlink(name) < 0) {
		err(1, "could not delete");
	}
}

int make_temp_file() {
	char filename[256] = "swapper_XXXXXX";
	int fd = mkstemp(filename);
	if (fd < 0) {
		err(1, "could not create temp file");
	}
	delete_file(filename);
}

int main (int argc, char* argv[])
{
	if (argc != 3) {
		errx(1, "err");
	}

	int fd1 = open_file(argv[1], O_RDWR);
	int fd2 = open_file(argv[2], O_RDWR);
	int fd3 = make_temp_file();

	copy_data(fd1, fd3);
	clear_file(fd1);
	copy_data(fd2, fd1);
	seek_file(fd3, 0, SEEK_SET);
	clear_file(fd2);
	copy_data(fd3, fd2);

	close_file(fd1);
	close_file(fd2);
	close_file(fd3);

	exit(0);
}

