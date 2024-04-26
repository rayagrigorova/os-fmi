#include <stdint.h>
#include <stdbool.h>

struct __attribute__((packed)) test{
	uint8_t int1;
	uint32_t int3;
	uint8_t int2;
};

// struct test{
// 	uint8_t int1;
// 	uint32_t int3;
// 	uint8_t int2;
// };

int main(void){
 	// size_t index;
 	// ssize_t sindex;
	// int16_t var; 
	// int32_t var2; 
	// uint8_t var3;
	// bool flag = true; 
	// test my_struct; // This wont work 
	struct test my_struct; 
	return 0;
}
