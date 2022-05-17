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

void multi_unpair(const size_t n, mpz_t res[n], mpz_t z) {

    if(n == 1) {
        mpz_set(res[0], z);
        return;
    }

    mpz_t x, y;
    mpz_init(x);
    mpz_init(y);

    unpair(x, y, z);

    if(n == 2) {
        mpz_set(res[0], x);
        mpz_set(res[1], y);
        return;
    }

    if(__builtin_popcount(n) == 1) {
        const size_t mid = n >> 1;
        multi_unpair(mid, res, x);
        multi_unpair(mid, res + mid, y);
    } else {
        const size_t npt = 2 << ((sizeof(int) * 8 - 2) - __builtin_clz(n));
        multi_unpair(npt, res, x);
        multi_unpair(n - npt, res + npt, y);
    }
}

int main() {

    const int size = 6;

    mpz_t z;

    mpz_t* integers = malloc(sizeof(mpz_t) * size);

    for(int i = 0; i < size; ++i) {
        mpz_init_set_ui(integers[i], i + 1);
    }

    mpz_init(z);

    multi_pair(z, size, integers);
    
    mpz_t output[size];

    for(int i = 0; i < size; ++i) {
        mpz_init(output[i]);
    }

    multi_unpair(size, output, z);

    for(int i = 0; i < size; ++i) {
        gmp_printf("%Zd, ", output[i]);
    }

    printf("\n");
}
