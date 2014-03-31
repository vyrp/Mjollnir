#include <stdio.h>
#include <time.h>

int main(){
    timespec dt = { 2, 300000000 };

    printf("tic...\n");
    printf("toc (%d)\n", nanosleep(&dt, NULL));
	return 0;
}
