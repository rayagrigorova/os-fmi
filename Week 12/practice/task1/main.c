#include <unistd.h>
#include <err.h>

int main(void) {
	// Write a C program that executes the command 'date'
	if (execlp("date", "date", (char*) NULL) < 0) {
		err(1, "Couldn't execute date");
	}
}
