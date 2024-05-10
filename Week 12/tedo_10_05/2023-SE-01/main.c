#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

// Return true if byte was read
bool read_byte(int fd, uint8_t* result);
bool read_byte(int fd, uint8_t* result) {
	ssize_t read_size;
	if(( read_size = read(fd, result, sizeof(uint8_t))) < 0) {
		err(1, "err");
	}
	return (read_size != 0);
}

void try_message(int in, int out, off_t current_offset);
void try_message(int in, int out, off_t current_offset) {
	uint8_t checksum = 0x55;
	uint8_t N;
	if(!read_byte(in, &N)) { // end of file 
		return;
	}

	checksum ^= N;
	for(int i = 3; i <= N - 1; i++) {
		uint8_t byte;
		if (!read_byte(in, &byte)) {
			return;
		}
		checksum ^= byte;
	}
	uint8_t expected_checksum;
	if (!read_byte(in, &expected_checksum)) {
		return;
	}
	if(checksum != expected_checksum){
		return;
	}
	uint8_t byte = 0x55;
	if(write(out, &byte, sizeof(byte)) < 0) {
		err(8, "write");
	}
	if (lseek(in, current_offset, SEEK_SET) < 0) {
		err(9, "lseek");
	}

	for(int i = 2; i <= N; i++) {
		if(!read_byte(in, &byte)) {
			errx(10, "unexpected EOF");
		}
		
	}
}

int main(int argc, char* argv[]) {
	int in = open(argv[1], O_RDONLY);
	if( in == -1) {
		err(2, "open");
	}
	int out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if( out == -1) {
		err(2, "open");
	}

	uint8_t byte;
	while(read_byte(in, &byte)) {
		// If the byte is 55, then return one position to the left 
		if (byte == 0x55) {
			// Save the current position
			off_t current_offset = lseek(in, 0, SEEK_CURR);	
			if (current_offset < 0) {
				err(5, "lseek");
			}

			try_message(in, out, current_offset);

			if (lseek(fd, current_offset, SEEK_SET) < 0) {
				err(5, "lseek");
			}

			}
		}
	}
	close(in);
	close(out);

}
