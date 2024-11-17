#include <stdio.h>



int main(){
    FILE *a;
    printf("Tipo File sem declaracao: %x\n", a);
    a = fopen("input.txt","r");
    printf("Tipo File com declaracao: %x\n", a);

    return 0;

}