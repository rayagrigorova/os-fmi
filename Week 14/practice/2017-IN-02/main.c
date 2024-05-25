#include <err.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void execute_with_two_args(const char* command, char command_args[2][5]){
	// execute command in child process and wait 
	
	pid_t pid = fork();
	if (pid < 0) {
		err(5, "fork");
	}
	if (pid == 0) {
		execlp(command, command, command_args[0], command_args[1]);
		err(6, "exec");
	}
	int status;
	pid_t wait_res = wait(&status);
	if (wait_res < 0) {
		err(10, "wait");
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status)) {
		warnx("Child didn't return 0");
	}
}
void execute_with_one_arg(const char* command, char command_args[2][5]){
	// execute command in child process and wait 
	pid_t pid = fork();
	if (pid < 0) {
		err(5, "fork");
	}
	if (pid == 0) {
		execlp(command, command, command_args[0]);
		err(6, "exec");
	}
	int status;
	pid_t wait_res = wait(&status);
	if (wait_res < 0) {
		err(10, "wait");
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status)) {
		warnx("Child didn't return 0");
	}
}

int main(int argc, char* argv[]) {
	const char* command = "echo";
	if (argc > 2) { 
		errx(1, "Usage: %s <command_name>", argv[0]);
	}
	if (argc == 2 && strlen(argv[1]) > 4) {
		errx(2, "Command name too long: must be at most 4 chars");
	}
	if (argc == 2) {
		command = argv[1];
	}
	// char command_args[2][5], fill it with \0
	// Read char by char
	// While read from stdin: 
	// Compare the char to '\n' and ' '
	// If it's a delimeter, terminate the current argument 
	// string. Else, check if there is space for the 
	// new character in the buffer. If not, throw an error 
	// If the two arguments have already been filled, execute the command in the child process 
	// Out of the loop: if there are arguments for 
	// the command, execute the command (but first terminate with '\0');
	char command_args[2][5];
	int arg_number = 0;
	int char_idx = 0;
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 5; j++){
			command_args[i][j] = '\0';
		}
	}

	ssize_t read_res;
	char buff;
	while((read_res = read(0, &buff, sizeof(char))) > 0) {
		if (buff == ' ' || buff == '\n') {
		// terminate the current argument 
			command_args[arg_number][char_idx] = '\0';
			// If the args array is non empty, 'finish' it by
			// going to the next one and moving the character index to the beginning 
			if(strlen(command_args[arg_number]) > 0) {
				arg_number++;
				char_idx = 0;
			}
			// If the number of arguments is 2, execute the command with them
			if (arg_number == 2) {
				execute_with_two_args(command, command_args);
				arg_number = 0; // Start all over 
			}
		}
		else {
			// check if there is space for the new character
			if (char_idx >= 4) {
				errx(4, "Argument too long: must be at most 4 chars");
			}
			// write(1, "Read: ", 6);
			// write(1, &buff, 1);
			// write(1, "\n", 1);
			command_args[arg_number][char_idx++] = buff;
		}
	}
	if (read_res < 0) {
		err(3, "read");
	}

	// If there are arguments for the command, execute the command (but first terminate with '\0' if necessary)

	// If the last character was a delimeter move to the previous command argument
	// since argument_number points to the next argument to be saved 
	if (buff == ' ' || buff == '\n') {
		arg_number--;
	}
	else {
		command_args[arg_number][char_idx] = '\0';
	}
	if (arg_number >= 0 && strlen(command_args[arg_number]) != 0) {
		// There is (at least) one argument for the command 
		if (arg_number == 0) { // one argument 
			execute_with_one_arg(command, command_args);
		}
		else { // two arguments
			execute_with_two_args(command, command_args);
		}
	}
}
