#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

const char DING[] = "DING\n";
const char DONG[] = "DONG\n";

bool handle_arg(char* arg) {
	if ((strlen(arg) != 1) || (!(arg[0] >= '0' && arg[0] <= '9'))){
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	// Check if there are 2 args and if they are numbers between 0 and 9
	if (argc != 3) { errx(1, "usage"); } 
	if ((!handle_arg(argv[1])) || (!handle_arg(argv[2]))) {
		errx(2, "wrong args");
	}
	// Idea: fork the process and create 2 pipes
	// through one of the pipes, the parent will inform
	// the child that it wrote 'ding', the other
	// pipe will be used by the child to inform the parent that it wrote 'dong'
	// I will use the result of fork() somewhere so I know in which process I am at all times 
	int from_parent_to_child[2];
	int from_child_to_parent[2];
	if (pipe(from_parent_to_child) < 0) { err(3, "pipe"); } 
	if (pipe(from_child_to_parent) < 0) { err(3, "pipe"); } 
	int pid = fork();
	if (pid < 0) { err(4, "fork"); } 
	if (pid == 0) {
		// close read and write ends that are not used 
		close(from_parent_to_child[1]);
		close(from_child_to_parent[0]);
	}
	else {
		close(from_parent_to_child[0]);
		close(from_child_to_parent[1]);
	}

	int number_of_reps = argv[1][0] - '0';
	int sleep_time = argv[2][0] - '0';

	// printf("Reps: %d\n", number_of_reps);
	for(int i = 0; i < number_of_reps; i++) {
		// printf("lol\n");
		// If we are in the child, wait for the parent to write a byte to the pipe and then print DONG
		// Else if we are in the parent and this is not 
		// the first rep, wait for the child to write 'dong'
		uint8_t dummy_byte = 0;
		if (pid == 0) {
			// read a dummy byte from the parent pipe 	
			if (read(from_parent_to_child[0], &dummy_byte, sizeof(dummy_byte)) < 0) {
				err(6, "read"); 
			}
			// write dong since the parent signaled that we should 
			if (write(1, DONG, strlen(DONG)) < 0) {
				err(9, "write");
			}
			// ! This may be the last 'dong' to be printed			   
			if (i + 1 >= number_of_reps) {
				close(from_child_to_parent[1]);
				exit(0);
			}
			// else, signal to the parent that
			// ding should be printed 
			if (write(from_child_to_parent[1], &dummy_byte, sizeof(dummy_byte)) < 0) { err(11, "write"); } 
		}
		else {
			// in the parent 
			// If it's the first execution of the loop, skip the waiting for input since this would lead to deadlock
			if (i != 0) {
				if (read(from_child_to_parent[0], &dummy_byte, sizeof(dummy_byte)) < 0) {
					err(6, "read"); 
				}
				sleep(sleep_time);
			}
			if (write(1, DING, strlen(DING)) < 0) {
				err(9, "write");
			}
			if (write(from_parent_to_child[1], &dummy_byte, sizeof(dummy_byte)) < 0) { err(11, "write"); } 
		}
	}
	wait(NULL);
}

