#include <err.h>
#include <unistd.h>

int main(void) {
	if (execlp("sleep", "sleep", "60", (char*) NULL) < 0) {
		err(1, "Couldn't execute sleep for 60 seconds");
	}
}
