#include <stdio.h>

int main(){
    FILE *f = fopen("/Mjollnir/yggdrasil/tests/runtime/files/created.txt", "w");
    if(!f){
        printf("Could not open file.\n");
        return 1;
    }
    
    printf("Writing to file...\n");
    fprintf(f, "Written\n");
    
    fclose(f);
    return 0;
}

