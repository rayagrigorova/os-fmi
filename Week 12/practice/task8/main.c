#include <err.h> 
#include <sys/wait.h> 
#include <fcntl.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdint.h> 

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като параметър име на файл.
	// Създава процес син, който записва стринга foobar във файла 
	// (ако не съществува, го създава, в противен случай го занулява),
	// след което процеса родител прочита записаното във файла съдържание 
	// и го извежда на стандартния изход, добавяйки по един интервал 
	// между всеки два символа 

	const char string_to_save[] = "foobar";

	if (argc != 2) {
		errx(1, "Usage: %s <file_name>", argv[0]);
	}
	pid_t fork_result = fork();	
	if (fork_result < 0) {
		err(2, "Failed to execute fork()");
	}
	if (fork_result == 0) {
		int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd < 0) {
			err(3, "Error when attempting to open %s", argv[1]);
		}
		if (write(fd, string_to_save, strlen(string_to_save)) < 0) {
			err(4, "Error when using write on %s", argv[1]);
		}
		close(fd);
	}
	else {
		int status;
		pid_t wait_result = wait(&status);		
		if (wait_result < 0) {
			err(5, "Error when using wait()");
		}
		if (!WIFEXITED(status)) {
			errx(6, "The child process didn't terminate normally");
		}
		if (WEXITSTATUS(status) != 0) {
			errx(7, "The child process had a non-null exit status");
		}

		int fd = open(argv[1], O_RDONLY);
		if (fd < 0) {
			err(8, "Error when attempting to open %s for reading", argv[1]);
		}
		// Read the characters two by two, adding a space between them 
		uint16_t buff;
		int read_result;
		while ((read_result = read(fd, &buff, sizeof(buff))) > 0) {
			if (write(1, &buff, read_result) < 0) {
				err(9, "Error when using write()");
			}
			if (write(1, " ", 1) < 0) {
				err(9, "Error when using write()");
			}

		}
		if (read_result < 0) {
			err(99, "read");
		}
		close(fd);
		if (write(1, "\n", 1) < 0) {
			err(10, "Error when using write()");
		}
	}
}
