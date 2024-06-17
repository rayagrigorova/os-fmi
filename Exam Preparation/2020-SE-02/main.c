#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if (argc != 3) { errx(1, "usage"); }

	uint8_t ESCAPE_CHAR = 0x7D;
	uint8_t XOR_VAL = 0x20;
	uint8_t BLANK = 0x55;

	// Algorithm: read the data using cat in a child process 
	// (first create a pipe) 
	// Then the parent reads from the pipe
	// If the read byte is 0x55, skip it
	// If the byte is 0x7D, read the next byte 
	// If the next byte is not one of the ones listed
	// (XORED with 0x20), then errx. Also, if there is 
	// nothing after the escape character, errx
	int pipefd[2];
	if (pipe(pipefd) < 0) { err(3, "pipe"); }

	int pid = fork();	
	if (pid < 0) { err(2, "fork"); }

	if (pid == 0) {
		// in child: close the read end 
		close(pipefd[0]);
		if (dup2(pipefd[1], 1) < 0) { err(5, "dup"); }
		execlp("cat", "cat", argv[1], (char*) NULL);
		err(8, "exec");
	}

	// in parent 
	close(pipefd[1]);
	int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_out < 0) { err(19, "open"); }

	uint8_t buff; 
	uint8_t xored_byte = 0x00;
	int read_res; 

	uint8_t check1 = ((uint8_t)0x00 ^ (uint8_t)XOR_VAL);
	uint8_t check2 = ((uint8_t)0xFF ^ (uint8_t)XOR_VAL);
	uint8_t check3 = ((uint8_t)0x55 ^ (uint8_t)XOR_VAL);
	uint8_t check4 = ((uint8_t)0x7D ^ (uint8_t)XOR_VAL);

	while((read_res = read(pipefd[0], &buff, sizeof(buff))) > 0) {
		// Check what the read byte was 
		// A blank, skip it
		if (buff == BLANK) {
			continue;
		}
		// An escape char, so expect the next byte 
		// to contain information
		if (buff == ESCAPE_CHAR) {
			int read_res_second_byte = read(pipefd[0], &xored_byte, sizeof(xored_byte));
			if (read_res_second_byte < 0) {
				err(11, "read");
			}
			else if (read_res_second_byte == 0) {
				errx(12, "no byte after eascape");
			}
			if ( (xored_byte != check1)
				&& (xored_byte != check2)
				&& (xored_byte != check3)
				&& (xored_byte != check4) ) {
				errx(17, "wrong byte");
			}
			xored_byte ^= XOR_VAL; //Translate the byte back
			// Write the byte to the new file 
			if (write(fd_out, &xored_byte, sizeof(xored_byte)) < 0) {
				err(21, "write");
			}
		}
	}
	if (read_res < 0) { err(11, "read"); } 

	wait(NULL); 

	close(pipefd[0]);
	close(fd_out);
}
