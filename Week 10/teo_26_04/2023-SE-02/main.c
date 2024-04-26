#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdio.h>

int get_file_size(int fd){
	struct stat statbuff;
	if (fstat(fd, &statbuff) < 0){
		err(1, "Couldnt stat file");
	}
	return statbuff.st_size;
}

int main(int argc, char* argv[]) {
	if (argc != 4){
		errx(1, "Arg count");
	}
	int dict = open(argv[2], O_RDONLY);
	if (dict < 0){
		err(2, "Couldnt open dict");
	}
	int idx = open(argv[3], O_RDONLY);
	if (idx < 0){
		err(3, "Couldnt open index file");
	}
	int arr[] = {1,2,3,40,5};
	int size = sizeof(arr) / sizeof(int); 
	int search = 40;
	int left = 0;
	int right = size - 1;
	bool found = true;
	while( right >= left){
		int  mid = left + (right - left) / 2;
		if (search < arr[mid]){
			right = mid - 1;
		} else if (search > arr[mid]){
			left = mid + 1;
		}
		else{
			found = true;
			break;
		} 
	}
	return 0;
}
