#include <stdio.h>
#include <stdlib.h>

const int SIZE = 100;

void free_arrays(char *arrays[SIZE], int n){
	for(int i=0; i<n; i++){
		free(arrays[i]);
	}
}

int main(){
	char* arrays[SIZE];
	printf("Starting...");
	//getchar();
	
	for(int i=0; i<SIZE; i++){
		arrays[i] = (char*)malloc(1024 * 1024 * sizeof(char));
		if(!arrays[i]){
			printf("<< Error in allocation (%d) >>\n", i);
			free_arrays(arrays, i);
			return 1;
		}
	}
	printf(" - allocated");
	//getchar();
	
	free_arrays(arrays, SIZE);
	printf(" - freed");
	
	//getchar();
	printf("Exit\n");
	
	return 0;
}
