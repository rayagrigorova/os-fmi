#include <unistd.h>
#include <err.h>

int main(void){
// 	if (execl("/bin/date", "date", NULL) < 0){
// 		err(1, "Error execl-ing");
// 	}
	int n = 1000;
	pid_t a = fork();
	if (a < 0){
		err(1, "Coudln't fork");
	}

	if (a > 0){
		for(int i = 0; i < n; i++){
			write(1, "F", 1);
		}
	}
	
	else {
		for(int i = 0; i < n; i++){
			write(1, "C", 1);
		}
	}
}
