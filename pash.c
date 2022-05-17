#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <ctype.h>

#include "pash.h"

void pair(mpz_t z, const mpz_t x, const mpz_t y) {
    if(mpz_cmp(y, x) > 0) {     // If y > x
        mpz_mul(z, y, y);       //    z = y * y
    } else {                    // Else
        mpz_mul(z, x, x);       //    z = x * x
        mpz_add(z, z, y);       //    z = z + y
    }                           //
    mpz_add(z, z, x);           // z = z + x
}

void unpair(mpz_t x, mpz_t y, const mpz_t z) {
    mpz_sqrtrem(x, y, z);       // x = sqrt(z), y = z - x * x
    if(mpz_cmp(x, y) > 0) {     // If x > y
        mpz_swap(x, y);         //    SWAP(x, y)
    } else {                    // Else
        mpz_sub(y, y, x);       //     y = y - x
    }
}


void multi_pair(mpz_t z, const size_t n, mpz_t x[n]) {
    
    const size_t p = (n + 1) >> 1;

    mpz_t *r = malloc(sizeof(mpz_t) * p);
    mpz_init(r[p - 1]);

    if((n & 1) == 0) {
        pair(r[p - 1], x[n - 2], x[n - 1]);
    } else {
        mpz_set(r[p - 1], x[n - 1]);
    }
    
    if(p == 1) {
        mpz_set(z, r[0]);
        return;
    }

    for(size_t i = 0, j = 0; i < p - 1; ++i, j += 2) {
        mpz_init(r[i]);
        pair(r[i], x[j], x[j + 1]);
    }
    
    multi_pair(z, p, r);
}

int main() {

    const int size = 1000000;

    mpz_t z;

    mpz_t* integers = malloc(sizeof(mpz_t) * size);

    for(int i = 0; i < size; ++i) {
        mpz_init_set_ui(integers[i], i + 1);
    }

    mpz_init(z);

    multi_pair(z, size, integers);

    size_t sz = mpz_sizeinbase(z, 10);
    printf("%zu\n", sz);
}
