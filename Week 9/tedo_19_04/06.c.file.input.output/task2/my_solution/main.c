#include <err.h>
#include <fcntl.h> // open
#include <unistd.h> // read

int main(int argc, char* argv[]){
	int src=0;
	if (argc == 2){
		src = open(argv[1], O_RDONLY);
		if ( src < 0){
			err(1, "%s: Can't open", argv[1]);
		}
	}


// 	int dest = open("bar.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
// 	if (dest < 0){ 
// 		err(2, "bar.txt: Cant't open");
// 	}
// 
	char c;
	ssize_t readSize;

	size_t ctr = 0;
	while (( readSize = read(src, &c, sizeof(c))) > 0 ){
		write(1, &c, sizeof(c));
		if (c == '\n'){
			ctr++;
		}
		if (ctr >= 10){
			break;
		}
	}

	if (readSize < 0) {
		err(3, "Can't read");
	}

	close(src);
	// close(dest);
}
