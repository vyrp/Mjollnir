#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int NUM = 100;
const int SIZE = 1024 * 1024;

void free_arrays(char *arrays[NUM], int n){
	for(int i=0; i<n; i++){
		free(arrays[i]);
	}
}

int main(){
	char* arrays[NUM];
	printf("Starting...");
	//getchar();
	
	for(int i=0; i<NUM; i++){
		arrays[i] = (char*)malloc(SIZE * sizeof(char));
		if(!arrays[i]){
			printf("<< Error in allocation (%d) >>\n", i);
			free_arrays(arrays, i);
			return 1;
		}
		memset(arrays[i], 0, SIZE);
	}
	printf(" - allocated");
	//getchar();
	
	free_arrays(arrays, NUM);
	printf(" - freed");
	
	//getchar();
	printf("Exit\n");
	
	return 0;
}
