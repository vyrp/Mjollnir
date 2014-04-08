#include <stdio.h>
#include <string.h>

const int SIZE = 1024 * 1024 * 1024;

char global[SIZE];

int main(){
    memset(global, 0, SIZE);
    printf("%c%c\n", global[0] + 'H', global[SIZE-1] + 'i');
    //getchar();
    return 0;
}
