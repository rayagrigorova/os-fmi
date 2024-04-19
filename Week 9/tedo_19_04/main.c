#include <stdio.h>
#include <unistd.h>

int main(){
	uid_t myID = getuid();
	// printf("Hello, world!\n");
	dprintf(1, "Hello world! My ID is %d.\n", myID);
	return 0;
}
