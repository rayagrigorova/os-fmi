#include<stdio.h>

struct {
	int a;
	char c;
	int b;
} test;

struct {
	int a;
	char c;
	int b;
} __attribute__((packed)) test2;


int main(void) {
	printf("%lu\n", sizeof(test));
	printf("%lu\n", sizeof(test2));
}
