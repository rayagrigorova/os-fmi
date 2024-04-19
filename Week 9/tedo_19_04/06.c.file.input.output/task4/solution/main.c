#include <err.h>
#include <fcntl.h> // open
#include <unistd.h> // read

int main(int argc, char* argv){
	if (argc != 3){
		errx("No enough args");
	}

	int fd1 = open(argv[2], O_RDONLY);
	if ( fd1 < 0){
		err(1, "foo.txt: can't  open");
	}

	int fd2 = open(argv[2], O_RDONLY);
	if ( fd2 < 0){
		err(1, "foo.txt: can't  open");
	}
	int fdtemp = open("tempfile", O_RDONLY);
	if ( fd2 < 0){
		err(1, "foo.txt: can't  open");
	}

	int dest = open("bar.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
	if (dest < 0){ 
		err(2, "bar.txt: Cant't open");
	}

	char c;
	ssize_t readSize;

	while (( readSize = read(src, &c, sizeof(c))) > 0 ){
		write(dest, &c, sizeof(c));
	}
	if (readSize != 0) {
		err(3, "Can't read");
	}

	// dprintf(1, "read character: %hhu\n", c);

	close(src);
	close(dest);
}
