#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

int main(void) {
	// write to log file 

	//run passwd to lock password 
	pid_t passwd_pid = fork();
	if (passwd_pid < 0) { err(1, "fork");}

	if (passwd_pid == 0) {
		execlp("echo", "echo", "passwd", "-l", "username", (char*) NULL);
		// execlp("passwd", "passwd", "-l", "username", (char*) NULL);
		err(1, "can't exec");
	}

	int status;
	if (wait(&status) < 0) {
		err(4, "Error when executuing wait()");
	}
	if (!WIFEXITED(status)) {
		err(2, "child was killed");
	}
	if (WEXITSTATUS(status) != 0) {
		err(3, "child returned non 0");
	}
	// exec killall
	// execlp("killall", "killall", "-u", "username", (char*) NULL);
	execlp("echo", "echo", "killall", "-u", "username", (char*) NULL);
}
