#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>
#include <unistd.h>

struct index_file_entry{
	uint16_t offset;
	uint8_t length;
	uint8_t saved; // not used
};

int main(int argc, char* argv[]) {
	// 0. Validation - number of args is 4,
	// the sizes of the two files are correct
	// Also, check if each offset from file 2 exists in file 1.
	//If not, end the program
	if (argc != 5){
		errx(1, "Usage: <f1.dat> <f1.idx> <f2.dat> <f2.idx>");
	}
	struct stat statbuf;
	if (stat(argv[2], &statbuf) < 0){
		err(4, "Coudln't stat %s", argv[2]);
	}
	if (statbuf.st_size % 4){
		errx(5, "Wrong file format for %s", argv[2]);
	}

	// Check if each offset from file 2 is valid in file 1 
	int dat_1, idx_1;
	if ((dat_1 = open(argv[1], O_RDONLY)) < 0){
		err(6, "Couldn't open %s", argv[1]);
	}

	if ((idx_1 = open(argv[2], O_RDONLY)) < 0){
		err(7, "Couldn't open %s", argv[2]);
	}

	if (stat(argv[1], &statbuf) < 0){
		err(13, "Coudln't stat %s", argv[1]);
	}

	uint32_t file1_size = statbuf.st_size;
	struct index_file_entry index_entry_buff;
	int read_result;
	while ((read_result = read(idx_1, &index_entry_buff, sizeof(struct index_file_entry))) > 0){
		if (index_entry_buff.length == 0 || index_entry_buff.offset + index_entry_buff.length > file1_size){
			errx(11, "The file %s contains at least one invalid offset + length combination", argv[2]);
		}
	}
	if (read_result < 0){
		err(8, "Couldn't read from %s", argv[2]);
	}

	if (lseek(idx_1, 0, SEEK_SET) < 0){
		err(12, "Couldn't lseek in %s", argv[2]);
	}

	// 1. For each string in the index file, do the following: 
	// Check the starting character: if it is between A and Z, write the string to f2.dat. Save the following information to .idx: the length read from the input index file and an index relative to the output .dat file (I will use a variable for this)
	int dat_2, idx_2;
	if ((dat_2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666 )) < 0){
		err(14, "Couldn't open %s", argv[3]);
	}

	if ((idx_2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){
		err(15, "Couldn't open %s", argv[4]);
	}
	uint16_t current_ind = 0; // This index will be saved to the output .idx file
	uint8_t char_buff;
	while ((read_result = read(idx_1, &index_entry_buff, sizeof(struct index_file_entry))) > 0){
		// lseek to the right position	
		if (lseek(dat_1, index_entry_buff.offset, SEEK_SET) < 0){
			err(16, "Couldnt lseek");
		}
		if (read(dat_1, &char_buff, sizeof(char_buff)) < 0){
			err(17, "Error when reading");
		}
		if (char_buff >= 'A' && char_buff <= 'Z'){
			// Copy the whole string to f2.dat
			int i = 0;
			do {
				if (write(dat_2, &char_buff, sizeof(char_buff)) < 0){
					err(18, "write");
				}
				if (read(dat_1, &char_buff, sizeof(char_buff)) < 0){
					err(19, "Error when reading");
			}
			i++;
			}
			while(i < index_entry_buff.length);

			// Write to the new index file
			if (write(idx_2, &current_ind, sizeof(current_ind)) < 0){
				err(20, "write");
			}
			if (write(idx_2, &index_entry_buff.length, sizeof(index_entry_buff.length)) < 0){
				err(21, "write");
			}
			if (write(idx_2, &index_entry_buff.saved, sizeof(uint8_t)) < 0){
				err(22, "write");
			}

			current_ind += index_entry_buff.length; 
		}
				
	}
	if (read_result < 0){
		err(13, "Couldn't read from %s", argv[2]);
	}
}
