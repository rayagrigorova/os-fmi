#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>

int main(void) {
	int value = 5;

	pid_t pid = fork();
	if (pid < 0) {err(1, "can't fork"); }
	if (pid > 0) {
		// Parent
		int status;
		if (wait(&status) < 0){
			err(2, "wait");
		}
		if (!WIFEXITED(status)){
			dprintf(1, "child was killed");
		}
		if (WEXITSTATUS(status) != 0) {
			dprintf(1, "child returned != 0");
		}
		// All child dprintf() calls will pass before the parent dprintf calls 
		for(int i = 0; i < 100; i++){
			dprintf(1, "In parent\n");
		}
	}
	else {
		// Child
		for(int i = 0; i < 100; i++){
			dprintf(1, "In child\n");
		}
	}
	value += 2;
	dprintf(1, "value is %d\n", value);
	return 0;
}
