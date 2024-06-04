int main(int argc, char* argv[]) {
// 	fds[] = pipe()
// 	for row in file:
// 		pid = fork()
// 		if pid == 0:
// 			close(fds[0])
// 			r = subroutine(row)
// 			write(fds[1], r)
// 			close(fds[1])
// 			exit(0)
// 
// 	close(fds[1]);
// 	result = 0;
// 	e = 0;
// 
// 	while(read(fds[0], e)) {
// 		r ^= e;
// 	}
// 
// 	close(fds[0]);
// 	for(int i = 0; i < ... ; i++) {
// 		wait();
// 	}
// 
// 	subroutine(row):
// 		fd = open(row.name);
// 		result = 0 
// 		lseek(fd, row.offset);
// 		for(int i = 0 ; i < row.len; i++) {
// 			e = read(fd)
// 			result ^= e
// 		}
// 		return result

	return 0;
}
