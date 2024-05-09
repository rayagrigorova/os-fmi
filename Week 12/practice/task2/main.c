#include <err.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "The number of arguments required is one");
	}
	if (execlp("ls", "ls", argv[1], (char*) NULL) < 0) {
		err(2, "Couldn't execute ls with one argument");
	}
}
