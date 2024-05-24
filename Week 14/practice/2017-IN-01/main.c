#include <stdio.h>

int main(void) {
	// The sevents column of /etc/passwd contains the command interpreters 
	// The bash command that I should execute is the following: 
	// cat /etc/passwd | cut -d ':' -f 7 | sort | uniq -c | sort -n
	
	char* argv1[] = {"cat", "cat", "/etc/passwd", (char*) NULL };
	char* argv2[] = { "cut", "cut", "-d", ":", "-f", "7", (char*) NULL },;
	char* argv3[] = { "sort", "sort", (char*) NULL };
	char* argv5[] = { "sort", "sort", "-n", (char*) NULL ;
}
