#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	char prompt[] = "Please, enter a command :)\n";
	while(1) {
		int write_res = write(1, &prompt, strlen(prompt));
		if (write_res < 0) {
			err(1, "write syscall failed");
		}
		if (write_res != (int)strlen(prompt)) {
			err(2, "couldn't write everything :(");
		}

		char buff[256];
		int read_res = 0; // a surprise tool that will help us later 
		if ((read_res = read(0, buff, sizeof(buff))) < 0) {
			err(3, "write");
		}
		if (read_res >= 256) {
			errx(4, "command too long");
		}

		// C functions expect a null terminated string 
		if (read_res > 0) {
			buff[read_res - 1] = '\0';
		}

		if (strcmp("exit", buff) == 0) { exit(0); }
		
		pid_t pid = fork();
		if (pid < 0) { err(5, "fork"); } 
		// The child process executes the command 
		// and the parent waits because else the number 
		// of children could become huge
		if (pid == 0) {
			// char command_path[235];
			// strcpy(command_path, "usr/bin/");
			// strcat(command_path, buff);
			execlp(buff, buff, (char*) NULL);
			err(6, "exec");
		}
		int status;
		if (wait(&status) < 0) {
			err(7, "couldnt wait for child");
		}
		if (!WIFEXITED(status) || WEXITSTATUS(status)) {
			warnx("Something happened to the child oof");
		}
	}
}
