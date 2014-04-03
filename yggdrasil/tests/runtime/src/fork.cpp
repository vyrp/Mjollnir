#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
#ifdef DEBUG
	printf("Press to start\n");
	getchar();
#endif

    fork();
	int result = fork();
	pid_t pid = getpid();
	pid_t ppid = getppid();

	printf("pid = %d | ppid = %d | result = %d\n", pid, ppid, result);

#ifdef DEBUG
	getchar();
#endif
	return 0;
}
