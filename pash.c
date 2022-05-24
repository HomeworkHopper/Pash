#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "pash.h"

#define PAIR(z, x, y) \
    if(mpz_cmp(y, x) > 0) { \
        mpz_mul(z, y, y); \
    } else { \
        mpz_mul(z, x, x); \
        mpz_add(z, z, y); \
    } \
    mpz_add(z, z, x);

#define UNPAIR(x, y, z) \
    mpz_sqrtrem(x, y, z); \
    if(mpz_cmp(x, y) > 0) { \
        mpz_swap(x, y); \
    } else { \
        mpz_sub(y, y, x); \
    }

// WARNING: This function destroys the provided array
static void pair_internal(mpz_t z, const size_t n, mpz_t x[n]) {
    
    const size_t p = (n + 1) >> 1;  // Number of pairs

    for(size_t i = 0, j = 0; i < p - 1; ++i, j += 2) {
        PAIR(z, x[j], x[j + 1]);
        mpz_swap(x[i], z);
    }

    if(!(n & 1)) {
        PAIR(z, x[n - 2], x[n - 1]);
        if(p == 1) 
            return;
        mpz_swap(x[p - 1], z);
    } else
        mpz_swap(x[p - 1], x[n - 1]);

    pair_internal(z, p, x);  // Recurse
}

static void unpair_internal(const size_t n, mpz_t res[n], const mpz_t z) {

    if(n == 1) {
        mpz_set(res[0], z);
        return;
    }

    if(n == 2) {
        UNPAIR(res[0], res[1], z);
        return;
    }

    // Local vars
    mpz_t x, y;
    mpz_init(x);
    mpz_init(y);

    // Split
    UNPAIR(x, y, z);

    // Is perfect power of two?
    if(!(n & (n - 1))) {

        // n / 2
        const size_t mid = n >> 1;

        // Unpair left
        unpair_internal(mid, res, x);

        // Unpair right
        unpair_internal(mid, res + mid, y);
    } else {

        // 10000000000000000000000000000000 >> number of leading zeros in n
        const size_t npt = 0x80000000 >> __builtin_clz(n);

        // Unpair left
        unpair_internal(npt, res, x);

        // Unpair right
        unpair_internal(n - npt, res + npt, y);
    }

    // Clean up
    mpz_clear(x);
    mpz_clear(y);
}

void pair(mpz_t target, const size_t n, mpz_t integers[n]) {
    assert(n != 0);

    if(n == 1) {
        mpz_swap(target, integers[0]);
    }

    pair_internal(target, n, integers);
}

void unpair(const size_t n, mpz_t target[n], const mpz_t integer) {
    assert(n != 0);

    if(n == 1) {
        mpz_set(target[0], integer);
    }

    unpair_internal(n, target, integer);
}
