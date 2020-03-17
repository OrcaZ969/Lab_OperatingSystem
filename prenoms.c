#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int sexe; // 1=Garcon, 2=Fille
    char prenom[25];
    int annee; // 1900..2018
    int nombre; // d'enfants nés cette année avec ce prénom
} tuple;

int main(int argc, char **argv)
{
    printf("Hello, prenoms\n");

    return 0;
}
