#include <err.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char* argv[]){
	if (argc != 3){
		errx(1, "Usage: <src> <dest>");
	}
	// 1. Open file for reading and file for writing 
	int fd_src, fd_dest;
	if ((fd_src = open(argv[1], O_RDONLY)) < 0){
		err(2, "Could not open %s for reading", argv[1]);
	}
	if ((fd_dest = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0){
		err(3, "Could not open %s for writing", argv[2]);
	}
	// 2. Read from fd1 into a buffer
	char buff[256];
	int result_from_read;
	int result_from_write;

	while((result_from_read = read(fd_src, buff, sizeof(buff))) > 0 ){
		if ((result_from_write = write(fd_dest, buff, result_from_read)) < 0 ){
			err(4, "Could not write to %s", argv[2]);
		}
		if (result_from_write < result_from_read){
			errx(5, "Wrote too few bytes");
		}
	}
	if (result_from_read < 0){
		err(6, "Could not read from %s", argv[1]);
	}
	close(fd_src);
	close(fd_dest);
	return 0;
}
