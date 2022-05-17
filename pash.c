#include <stdio.h>
#include <assert.h>

#include "pash.h"

void pair(mpz_t z, const mpz_t x, const mpz_t y) {

    if(mpz_cmp(y, x) > 0) {
        mpz_mul(z, y, y);
    } else {
        mpz_mul(z, x, x);
        mpz_add(z, z, y);
    }

    mpz_add(z, z, x);
}

void unpair(mpz_t x, mpz_t y, const mpz_t z) {
    
    mpz_sqrtrem(x, y, z);

    if(mpz_cmp(x, y) > 0) {
        mpz_swap(x, y);
    } else { 
        mpz_sub(y, y, x);
    }
}

int main() {

    mpz_t x, y, z;

    mpz_init(x);
    mpz_init(y);
    mpz_init(z);

    mpz_set_ui(x, 1);
    mpz_set_ui(y, 2);

    pair(z, x, y);

    gmp_printf("%Zd\n", z);

    unpair(x, y, z);

    gmp_printf("%Zd, %Zd\n", x, y);
}
