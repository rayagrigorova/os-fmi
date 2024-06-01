#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <err.h>

struct header {
	uint32_t magic; 
	uint8_t header_version; // 0x01
	uint8_t data_version;
	uint16_t count; 
	uint32_t reserved_1;
	uint32_t reserved_2;
}__attribute__((packed));

// when data version is 0x00
struct structure_1_record {
	uint16_t offset;
	uint8_t original_byte;
	uint8_t new_byte;
}__attribute__((packed));

// when data version is 0x01
struct structure_2_record {
	uint32_t offset;
	uint16_t original_word;
	uint16_t new_word;
}__attribute__((packed));

void copy_file(int src, int dest);
void copy_file(int src, int dest) {
	char buff[256];
	int read_res;

	while((read_res = read(src, buff, sizeof(buff))) > 0) {
		if (write(dest, buff, read_res) < 0) {
			err(7, "write");
		}
	}

	if (read_res < 0) {
		err(6, "read");
	}

	// Return to the beginnings of both files
	if (lseek(src, 0, SEEK_SET) < 0) {
		err(5, "lseek");
	}

	if (lseek(dest, 0, SEEK_SET) < 0) {
		err(5, "lseek");
	}
}

// lseek is positioned where it should be in the patch file 
void change_bytes(int fd_patch, int fd1, int fd2);
void change_words(int fd_patch, int fd1, int fd2);

void change_bytes(int fd_patch, int fd1, int fd2) {
	// For each entry in the patch file:
	// Get the offset - if it's invalid 
	// in relation to the file size, return an error 
	// Else, lseek in f1. If the atual original byte 
	// doesn't match the expected one, errx()
	// Else, lseek in f2 to the same 
	// position and write over it with new_byte
	uint8_t f1_buff;
	struct structure_1_record record;
	int read_res;

	struct stat statbuf;
	if (fstat(fd1, &statbuf) < 0) { err(15, "fstat"); }
	off_t f1_size = statbuf.st_size;

	while((read_res = read(fd_patch, &record, sizeof(struct structure_1_record))) > 0) {
		if(record.offset >= f1_size) {
			errx(17, "Invalid offset");
		}
		if (lseek(fd1, record.offset, SEEK_SET) < 0) {
			err(20, "lseek");
		}
		if (read(fd1, &f1_buff, sizeof(f1_buff)) < 0) {
			err(21, "read in buff");
		}
		if (f1_buff != record.original_byte) {
			errx(22, "The original byte doesn't exist in the file");
		}
		if (lseek(fd2, record.offset, SEEK_SET) < 0) {
			err(23, "lseek 2"); 
		}
		if (write(fd2, &record.new_byte, sizeof(record.new_byte)) < 0) {
			err(25, "write of new byte");
		}
	}
	if (read_res < 0) { err(14, "read"); }
}

void change_words(int fd_patch, int fd1, int fd2) {
	// The algorithm is the same as for change_bytes
	uint16_t f1_buff;
	struct structure_2_record record;
	int read_res;

	struct stat statbuf;
	if (fstat(fd1, &statbuf) < 0) { err(15, "fstat"); }
	off_t f1_size = statbuf.st_size;

	while((read_res = read(fd_patch, &record, sizeof(struct structure_2_record))) > 0) {
		if(record.offset * 2 >= f1_size) { 
			errx(17, "Invalid offset");
		}
		if (lseek(fd1, record.offset * 2, SEEK_SET) < 0) {
			err(20, "lseek");
		}
		if (read(fd1, &f1_buff, sizeof(f1_buff)) < 0) {
			err(21, "read in buff");
		}
		if (f1_buff != record.original_word) {
			errx(22, "The original word doesn't exist in the file");
		}
		if (lseek(fd2, record.offset * 2, SEEK_SET) < 0) {
			err(23, "lseek 2"); 
		}
		if (write(fd2, &record.new_word, sizeof(record.new_word)) < 0) {
			err(25, "write of new byte");
		}
	}
	if (read_res < 0) { err(14, "read"); }
}

int main(int argc, char* argv[]) {
	if (argc != 4) { 
		errx(1, "Usage: %s <f1> <f2> <f3>", argv[0]);
	}

	// Validate the patch.bin file: 
	// Check if its size is at least 16
	// (since the header has a size of 16)
	// If yes, read the header 
	// If data version == 0, then the file size 
	// should be 16 + count * 4 (because each entry in the data section is 4 bytes)
	// If data version == 1, then the file size 
	// should be 16 + count * 8
	// (because each entry in the data section is 4 bytes)

	struct stat statbuf;
	if (stat(argv[1], &statbuf) < 0) { err(2, "stat"); }
	if (statbuf.st_size < 16) { errx(3, "Invalid patch file format: file too small\n"); } 

	int fd_patch = open(argv[1], O_RDONLY);
	if(fd_patch < 0) { err(4, "open"); } 

	struct header patch_file_header;
	// Read the header
	if(read(fd_patch, &patch_file_header, sizeof(struct header)) < 0) {
		err(5, "read"); 
	}

	// Validate the values in the header 
	// The header version should be == 0x01
	// Data version is either 0x00 or 0x01 
	if (patch_file_header.header_version != 0x01 
			|| (patch_file_header.data_version != 0x00 && patch_file_header.data_version != 0x01)) {
		errx(3, "Invalid patch file format: values in header are wrong\n");
	}

	// Bytes 
	if (patch_file_header.data_version == 0x00) {
		if (statbuf.st_size != 16 + patch_file_header.count * 4) {
			errx(3, "Invalid patch file format: the data version is 0x00, total patch file size doesn't correspond\n"); 
		}
	}

	// Words
	else { 
		if (statbuf.st_size != 16 + patch_file_header.count * 8) {
			errx(3, "Invalid patch file format: the data version is 0x01, total patch file size doesn't correspond\n"); 
		}
		// Also check the size of f1 according to the version:
		// If the version is 1, then the file size should 
		// be divisible by 2
		if (stat(argv[2], &statbuf) < 0) { 
			err(2, "stat");
		}
		if (statbuf.st_size % 2) {
			errx(3, "Invalid patch file format: data version is 0x01 and the size of f1 is odd\n"); 
		}
	}

	// Actual logic of the program
	int f1 = open(argv[2], O_RDONLY);
	if(f1 < 0) { err(4, "open"); }
	int f2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if(f2 < 0) { err(4, "open"); }

	// Alogirthm: copy f1 into f2 
	// For each entry from the patch file: 
	// Based on the data version, change words or bytes (but first check the offset of course) :D
	// Also, check if the original byte/word is actually present in f1
	copy_file(f1, f2);
	if (patch_file_header.data_version == 0x00) {
		change_bytes(fd_patch, f1, f2);
	}
	else {
		change_words(fd_patch, f1, f2);
	}

	close(fd_patch);
	close(f1);
	close(f2);
}
