#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void makeSum(int argc, char *argv[]){
	long int sum=0;
	char *ptr=NULL;
	if(argc !=4){
		printf("ERROR AT INPUT ARGUMENTS");
		exit(EXIT_FAILURE);
	}
	for(int i=1; i<argc; i++){
		sum+=strtol(argv[i], &ptr, 0);
	}
	printf("Suma argumentelor este: %ld\n", sum);

}

int main(int argc, char*argv[]){
	makeSum(argc, argv);
	return 0;
}
