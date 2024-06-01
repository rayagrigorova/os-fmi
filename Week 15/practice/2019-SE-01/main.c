#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
 
struct log_file_record {
	int start;
	int end;
	int exit_status;
};

bool stop_cond(struct log_file_record record, int threshold) {
	// This function returns true if the stop conditions are fulfilled (the error code != 0 and end - start < threshold
	return record.exit_status != 0 && (record.end - record.start) < threshold;
}

int main(int argc, char* argv[]) {
	//argv[1] -> threshold
	// argv[2] -> program Q
	// argv[3]-argv[n] -> parameters for Q (not necessary)
	if (argc < 3) {
		errx(1, "Usage: %s <threshold> <Q> [<arg for Q>]...", argv[0]);
	}

	int threshold = strtol(argv[1], NULL, 10); 
	if (errno < 0) { err(19, "strtol"); }

	int log_fd = open("run.log", O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (log_fd < 0) { err(8, "log file failed to open"); } 

	// This represents the previous execution of Q 
	// (its  values are set in a way that represent a valid execution of Q)
	struct log_file_record prev;
	prev.start = 0;
	prev.end = threshold;
	prev.exit_status = 0;

	while (1) {
		// start Q
		int curr_start = time(NULL);	
		int pid = fork();
		if (pid < 0) { err(3, "fork"); }
		if (pid == 0) {
			execvp(argv[2], (argv + 2));
			err(7, "execvp");
		}

		int status; 
		if (wait(&status) < 0) { err(8, "wait"); }
		int curr_end = time(NULL);	

		int exit_status = 0;
		if (WIFSIGNALED(exit_status)) { exit_status = 129; }
		else if (WIFEXITED(status)) {
			exit_status = WEXITSTATUS(status);
		}

		struct log_file_record buff;
		buff.start = curr_start;
		buff.end = curr_end;
		buff.exit_status = exit_status;
// 		if (write(log_fd, &buff, sizeof(buff)) < 0) {
// 			err(18, "write");
// 		}
		dprintf(log_fd, "%d %d %d", curr_start, curr_end, exit_status);
		if (stop_cond(buff, threshold) && stop_cond(prev, threshold)) {
			exit(0);
		}
		prev = buff;
	}
}
