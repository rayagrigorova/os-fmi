#include <err.h>
#include <fcntl.h> // open
#include <unistd.h> // read

int main(){
	int src = open("foo.txt", O_RDONLY);
	// int src = open("/etc/shadow", O_RDONLY);

	if ( src < 0){
		// write(2, "Can't open\n", 11);	
		// exit(1)
		// errno = ...
		// dprintf("Can't open the file\n")
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
