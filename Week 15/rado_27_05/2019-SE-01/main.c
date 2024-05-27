int main(int argc, char* argv[]) {
	// The current program is used this way:
	// ./P 8 Q arg1 arg2 ... argN
	// execvp(Q, args);
	// The args that we want to pass are argv
	// Increment the argv and pass it to execv
	// Regarding the NULL pointer in the  end, we don't need to add it seperately (argv[] 
	// of P contians a NULL at the end) !!
	// So this usage is valid: execvp(Q, argv+2);
}
