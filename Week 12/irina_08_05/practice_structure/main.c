#include <stdint.h>
#include <stdio.h>

struct foo {
	uint8_t a;
	uint16_t b;
};

struct __attribute__((packed)) bar {
	uint32_t a;
	uint8_t b;
	uint16_t c;
};

int main(void){
	struct foo mystruct;
	mystruct.a = 5;
	mystruct.b = 6;
	printf("a is %d b is %d\n", mystruct.a, mystruct.b);
	printf("sizeof a is %d sizeof b is %d\n", sizeof(mystruct.a), sizeof(mystruct.b));
	printf("size is %d\n", sizeof(mystruct));

	struct bar st;
	printf("size is %d\n", sizeof(st));
}
