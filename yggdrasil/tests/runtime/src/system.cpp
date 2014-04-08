#include <stdio.h>
#include <stdlib.h>

int main(){
    int result = 0, status;
    
    result = (status = system("pwd")) || result;
    printf("pwd: %d\n", status);
    
    result = (status = system("touch tmp")) || result;
    printf("touch tmp: %d\n", status);
    
    result = (status = system("ls")) || result;
    printf("ls: %d\n", status);
    
    result = (status = system("rm tmp")) || result;
    printf("rm tmp: %d\n", status);
    
    return result;
}
