#include <err.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <fcntl.h> 

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която създава файл в текущата 
	// директория и генерира два процеса, които записват низовете foo и bar 
	// в създадения файл 
	// - Програмата не гарантира последователно записване на низовете 
	// - Променете програмата така, че да записва низовете последователно, като първият е foo 

	// The file name will be given as an argument of the program
	if (argc != 2) {
		errx(1, "Usage: %s <file_name>", argv[0]);
	}

	int fd = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (fd < 0) {
		err(2, "Couldn't open %s", argv[1]);
	}

	for(int i = 0; i < 2; i++) {
		pid_t fork_res = fork();
		if (fork_res < 0) {
			err(3, "Error when using fork");
		}
		if (fork_res == 0) {
			if (i == 0) {
				if (write(fd, "foo", 3) < 0) {
					err(4, "Error when writing to file");
				}
			}
			else {
				if (write(fd, "bar", 3) < 0) {
					err(4, "Error when writing to file");
				}
			}
			close(fd);
			return 0;
		}
		else {
			// Wait for the child process to finish so that 
			// 'foo' always comes before 'bar'
			int status;
			pid_t wait_res = wait(&status);
			if (wait_res < 0) {
				err(5, "Error when using wait()");
			}
			if (!WIFEXITED(status)) {
				errx(6, "The child process didn't terminate normally");
			}
			if (WEXITSTATUS(status) != 0) {
				errx(7, "The child process had a non-zero exit status");
			}
		}
	}
	close(fd);
}
