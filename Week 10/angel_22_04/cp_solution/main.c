#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]){
	if (argc != 3){
		errx(1, "2 arguments required");
	}

	int src_fd = open(argv[1], O_RDONLY);
	if (src_fd < 0){
		err(1, "Could not open %s for reading", argv[1]);
	}

	int dest_fd  = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dest_fd < 0){
		err(2, "Could not open %s for writing", argv[2]);
	}
	// Open the source file and the dest file
	// Read from the scr file into the dest file
	int result; 
	char buff[256];
	// char buff;
	while ((result = read(src_fd, buff, sizeof(buff))) > 0 ){
		if(write(dest_fd, buff, result) < 0){
			err(2, "Failed to write to %s", argv[2]);
		}
	}
	if (result < 0 ){
		err(3, "Failed to read from %s", argv[1]);
	}

	return 0;
}
