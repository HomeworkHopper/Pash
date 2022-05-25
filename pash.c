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


static void pair_internal(mpz_t z, const size_t n, mpz_t x[n]) {
    
    // Number of groups of two
    const size_t p = (n + 1) >> 1;

    // Pair each group of two
    for(size_t i = 0, j = 0; i < p - 1; ++i, j += 2) {
        PAIR(z, x[j], x[j + 1]);
        mpz_swap(x[i], z);
    }

    // Take care of the last group
    if(!(n & 1)) {
        PAIR(z, x[n - 2], x[n - 1]);
    } else {
        mpz_swap(z, x[n - 1]);
    }

    // If there was only one group, we're done
    if(p == 1) {
        return;
    }

    // Swap z back into the array
    mpz_swap(x[p - 1], z);

    // Recurse
    pair_internal(z, p, x);
}

static void unpair_internal(const size_t n, mpz_t res[n], const mpz_t z) {

    // Base case #1
    if(n == 1) {
        mpz_set(res[0], z);
        return;
    }

    // Base case #2
    if(n == 2) {
        UNPAIR(res[0], res[1], z);
        return;
    }

    // Result variables
    mpz_t x, y;
    mpz_init(x);
    mpz_init(y);

    // Unpair z into x and y
    UNPAIR(x, y, z);

    // If n is a perfect power of two...
    if(!(n & (n - 1))) {

        // 0.5 * n
        const size_t mid = n >> 1;

        // Unpair x
        unpair_internal(mid, res, x);

        // Unpair y
        unpair_internal(mid, res + mid, y);
    } else {

        // Nearest power of two <= n
        const size_t npt = 0x80000000 >> __builtin_clz(n);

        // Unpair x
        unpair_internal(npt, res, x);

        // Unpair y
        unpair_internal(n - npt, res + npt, y);
    }

    // Clean up
    mpz_clear(x);
    mpz_clear(y);
}

void pair(mpz_t target, const size_t n, mpz_t integers[n]) {
    
    // Ensure n > 0
    assert(n);

    // Delegate to actual pair function
    pair_internal(target, n, integers);
}

void unpair(const size_t n, mpz_t target[n], const mpz_t integer) {
    
    // Ensure n > 0
    assert(n);

    // Delegate to actual unpair function
    unpair_internal(n, target, integer);
}
