#include <stdio.h>

int main(){
    FILE *f = fopen("/Mjollnir/yggdrasil/tests/runtime/files/target.txt", "r");
    if(!f){
        printf("Could not open file.\n");
        return 1;
    }
    
    printf("Reading file...\n");
    
    char str[100];
    fscanf(f, "%s", str);
    printf("%s\n", str);
    
    int i;
    fscanf(f, "%d", &i);
    printf("%d\n", i);
    
    fclose(f);
    return 0;
}

