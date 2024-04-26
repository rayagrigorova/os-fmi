// копирайте съдържанието на файл1 във файл2
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>

//int main(int argc, char* argv[])
int main(void)
{
	int fd1;
	int fd2;

	fd1 = open("foo.txt", O_RDONLY);
	if (fd1 < 0) {
		err(1, "File failed to open in read mode");
	}

	if ( ( fd2 = open("bar.txt", O_CREAT|O_WRONLY|O_TRUNC, 0666) ) == -1 ){
		err(2, "File failed to open in write mode");
	}

	char buf[4096];
	int n_read;
	int n_written;
	while ( (n_read = read(fd1, buf, sizeof(buf))) > 0 ){
		n_written = write(fd2, buf, n_read);
		if (n_written < 0) {
			err(1, "could not write");
		}
		if (n_written != n_read) {
			errx(1, "could not write all data at once, giving up");
		}
	}
	if (n_read < 0) {
		err(1, "read failed");
	}

	if (close(fd1) < 0) {
		err(1, "could not close reading file");
	}
	if (close(fd2) < 0) {
		err(1, "could not close writing file");
	}
}
