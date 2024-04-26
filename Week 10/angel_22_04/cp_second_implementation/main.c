#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> // stat
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){
	// This is an implementation of cp with an arbitrary number of arguments
	// 0. Check if the arguments are valid: they should be at least 2 
	if (argc < 3){
		errx(1, "Usage: cp <src>...<directory>");
	}
	// Use stat to check if the last argument of the script is a directory 
	struct stat statbuf;
	if (stat(argv[argc - 1], &statbuf) < 0){
		err(2, "Could not stat %s", argv[argc - 1]);
	}
	if (!(statbuf.st_mode & S_IFDIR)){
		errx(3, "The last argument should be a directory");
	}

	for (int i = 1; i < argc - 1; i++){
		// Generate the correct address of the file 
		char* buff = malloc(strlen(argv[argc - 1]) + strlen(argv[i]) + 2);
		strcpy(buff, argv[argc - 1]);
		buff[strlen(argv[argc - 1])] = '/';
		buff[strlen(argv[argc - 1]) + 1] = '\0';
		strcat(buff, argv[i]);

		// For each of the files do the following: use open to create a file in the target directory if it doesnt exist 
		int fd_dest = open(buff, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd_dest < 0){
			err(4, "Failed to copy file %s", argv[i]);
		}
		int fd_src = open(argv[i], O_RDONLY);
		if (fd_src < 0){
			err(5, "Failed to find file %s", argv[i]);
		}

		ssize_t read_bytes;
		char cp_buff[256];
		while (( read_bytes = read(fd_src, cp_buff, sizeof(cp_buff))) > 0){ 
			ssize_t write_res = write(fd_dest, cp_buff, read_bytes);
			if (write_res < 0){
				err(7, "Couldnt write to copied file");
			}
			else if (write_res < read_bytes){
				errx(8, "Couldnt write all the data at once");
			}
		}

		if (read_bytes < 0){
			err(6, "Read err");
		}

		free(buff);
		if (close(fd_src) < 0){
			err(9, "Couldnt close scr file %s", argv[i]);
		}
		if (close(fd_dest) < 0){
			err(10, "Couldnt close dest file for %s", argv[i]);
		}
	}

	return 0;
}
