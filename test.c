#include <stdio.h>
#include <stdlib.h>

int main(){
    char *binaryString = "1011";
    long decimal = strtol(binaryString, NULL, 10);

    printf("Результат: %ld/n", decimal);
}