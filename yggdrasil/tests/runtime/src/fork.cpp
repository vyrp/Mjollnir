#include <stdio.h>
#include <unistd.h>

int main(){
	printf("Press to start\n");
	int pid = fork();
	printf("pid = %d\n", pid);
	pid = fork();
	printf("pid = %d\n", pid);
	//getchar();
	return 0;
}
