#include <unistd.h>
#include <fcntl.h>
int main(void) {
	int fd = open("text.txt", O_RDONLY);
	dup2(fd, 0);
	close(fd);

	execlp("cowsay", "cowsay", (char*) NULL);
}
