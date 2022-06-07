#include <stdio.h>
#include <stdlib.h>

#include "pash.h"

int main() {

    mpz_t a;
    mpz_t b;
    mpz_t c;

    mpz_init_set_ui(a, 2);
    mpz_init_set_ui(b, 3);
    mpz_init_set_ui(c, 1);

    mpz_t arr[3];

    (*arr[0]) = *a;
    (*arr[1]) = *b;
    (*arr[2]) = *c;

    mpz_t z;
    mpz_init(z);

    pair(z, 3, arr);

    gmp_printf("%Zd\n", z);

    unpair(3, arr, z);

    gmp_printf("%Zd, %Zd, %Zd\n", arr[0], arr[1], arr[2]);
}
