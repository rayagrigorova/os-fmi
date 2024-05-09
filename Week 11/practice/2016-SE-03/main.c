void split_file(int f, int f1, int f2){
	uint32_t num;
	int bytes_read;
	while ((bytes_read = read(f, &num, sizeof(num))) > 0){
		if (bytes_read != sizeof(num)){
			errx(4, "Incorrect file format");
		}
		int target_fd = f1;
		if (i % 2){
			target_fd = f2;
		}
		int bytes_written = write(target_fd, &num, sizeof(num));
		if (bytes_written < 0){
			err(5, "Error on write in split file");
		}
		if (bytes_written < sizeof(num)) {
			err(6, "Error on write in split file: couldnt write number at once");
		}
		i++;
	}
	if (bytes_read < 0){
		err(7, "Couldn't read from f");
	}
}

void sort_file(int fd) {
	if (lseek(fd, 0, SEEK_SET) < 0){
		err(8, "lseek");
	}
	uint32_t buff[65000000];
	// read fd into buffer 
	// use qsort() to sort the buffer 
}

void merge_files(int fd1, int fd2, int fd){
	while (1){
		// read from fd1 and fd2 and write the lower value into fd 
		// write the smaller element into the file
	}
}

int main(int argc, char* argv[]){
	// Read the file and split it into 2 so it can fit into the available memory
	// Sort each one of the pieces seperately and then merge the two pieces
	if (argc != 2){
		errx(1, "Usage: %s <file of uint32_t elements>", argv[0]);
	}

	int f = open("f", O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (f < 0){
		err(1, "Could't open f");
	}
	
	int f1 = open("f1", O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (f1 < 0){
		err(2, "Could't open f1");
	}

	int f2 = open("f2", O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (f2 < 0){
		err(3, "Could't open f2");
	}

	split_file(f, f1, f2);
	sort_file(f1);
	sort_file(f2);
	merge_files(f1, f2, f);
	close(f);
	close(f1);
	close(f2);
}
