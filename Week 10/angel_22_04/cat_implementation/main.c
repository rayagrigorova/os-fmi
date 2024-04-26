#include <err.h>
#include <fcntl.h>
#include <unistd.h>

void read_and_write(int fd_read, int fd_write){
	char buff[256];
	int read_size;
	while ((read_size = read(fd_read, buff, sizeof(buff))) > 0 ){
		if ( write(fd_write, buff, read_size ) < 0){
			err(2, "Failed to write");
		}
	}
	if (read_size < 0){
		err(4, "Failed to read");
	}
}

int main(int argc, char* argv[]) {
	if (argc == 1){
		// The program should read from stdin and write the information that is read back to stdout
		while (1){
			read_and_write(0, 1);
		}
	}

	else{
		for(int i = 1; i < argc; i++){
			// For each file, do the following: open it for reading 
			// and read it into a buff, writing the result to stdout
			int fd = open(argv[i], O_RDONLY);
			if (fd < 0) {
				err(3, "Failed to open the file for reading");
			}

			read_and_write(fd, 1);

		    if (close(fd) < 0){
				err(4, "Failed to close the file");
		    }
		}
	}

	return 0;
}
