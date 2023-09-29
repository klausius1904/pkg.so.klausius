#include <stdio.h>
#include <stdlib.h>

void printArgs(int argc, char **argv){
	if(argc != 4){
		printf("ERROR!  INCORRECT ARGS\n");
		exit(EXIT_FAILURE);
	}
	for (int i=0; i<argc; i++){
		printf("ARGUMENTUL %d este %s\n", i, argv[i]);
	}
}
int main(int argc, char *argv[]){
	printArgs(argc, argv);
	return 0;
}
