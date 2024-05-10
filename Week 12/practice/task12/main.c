#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

// This function forks the current process and waits 
// for the child process to finish.
// The function returns true if the command runs successfully
// The function also has the responsibility do display an 
// appropriate warning if the command that was executed has a 
// non-null exit status
bool execute_command(char* command_name);
bool execute_command(char* command_name) {
	pid_t pid = fork();
	if (pid < 0) {
		err(2, "Error when using fork");
	}
	if (pid == 0) {
		// in the child process 
		if (execlp(command_name, command_name, (char*) NULL) < 0) {
			err(3, "Error when using exec()");
		}
	}
	// The following lines can only be executed in the parent 
	// since execlp was called in the child process 
	int status;
	pid_t wait_res = wait(&status);
	if (wait_res < 0) {
		err(4, "Error when calling wait()");
	}
	if (!WIFEXITED(status)) {
		warnx("The child process for %s didn't terminate normally", command_name);
		return false;
	}
	// if the process terminated normally, check its status
	else if (WEXITSTATUS(status) != 0) {
		warnx("The child process for %s didn't return 0", command_name);
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	// Да се напише програма на C, която получава като 
	// параметри от команден ред две команди (без параметри).
	// и име на файл в текущата директория. Ако файлът не 
	// съществува, го създава. Програмата изпълнява командите 
	// последователно, по реда на подаването им. Ако първата 
	// команда завърши успешно, програмата добавя нейното име към 
	// съдържанието на файла, подаден като команден параметър. 
	// Ако командата завърши неуспешно, програмата уведомява 
	// потребителя чрез подходящо съобщение. 
	if (argc != 4) {
		errx(1, "Usage: %s <cmd_1> <cmd_2> <file_name>", argv[0]);
	}
	if (execute_command(argv[1])) {
		// Open the file, save the command name and then 
		// close the file
		int fd = open(argv[3], O_WRONLY | O_APPEND | O_CREAT, 0666);
		if (fd < 0) {
			err(5, "Error when attempting to open %s", argv[3]);
		}
		if (write(fd, argv[1], strlen(argv[1])) < 0) {
			err(6, "Error when using write on %s", argv[3]);
		}
		close(fd);
	}
	execute_command(argv[2]);
}
