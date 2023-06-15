// File: pash.c
//
// Description: Implementation of the pash module. Pair/unpair functions
//              delegate to internal recursive functions, and the underlining
//              Szudzik pairing/unpairing operations are defined as macros.
//
//              All function arguments are expected to be already initialized.
//
// @author      Shaun Thornton
//
// // // // // // // // // // // // // // // // // // // // // // // // //

#include "pash.h"       // Module interface

#include <stdlib.h>     // malloc & free

// (x, y) -> z
#define PAIR(z, x, y) \
    if(mpz_cmp(y, x) > 0) { \
        mpz_mul(z, y, y); \
    } else { \
        mpz_mul(z, x, x); \
        mpz_add(z, z, y); \
    } \
    mpz_add(z, z, x);

// z -> (x, y)
#define UNPAIR(x, y, z) \
    mpz_sqrtrem(x, y, z); \
    if(mpz_cmp(x, y) > 0) { \
        mpz_swap(x, y); \
    } else { \
        mpz_sub(y, y, x); \
    }

// Half of n, rounded up
#define PAIR_COUNT(n) (n + 1) >> 1

// Nearest power of 2 <= n
#ifdef __GNUC__
	#define NPT(n) 0x80000000 >> __builtin_clz(n)
#else
	static inline size_t NPT(size_t n) {
        // Hacker's Delight, First Edition, page 47
        n =     n | (n >> 1);
        n =     n | (n >> 2);
        n =     n | (n >> 4);
        n =     n | (n >> 8);
        n =     n | (n >>16);
        return  n - (n >> 1);
	}
#endif

/// An internal helper function which complements the pair function.
///
/// @param z    the resulting integer's destination
/// @param n    the number of integers to pair
/// @param x    the array of integers to pair
static void pair_internal(mpz_t z, const size_t n, mpz_t x[n]) {
 
    // The number of groups of two we can make
    const size_t p = PAIR_COUNT(n);

    // Pair each group of two
    for(size_t i = 0, j = 0; i < p - 1; ++i, j += 2) {
        PAIR(z, x[j], x[j + 1]);
        mpz_swap(x[i], z);
    }

    // Take care of the last group
    if(!(n & 1)) {
        // If n is even, pair the remaining two integers together
        PAIR(z, x[n - 2], x[n - 1]);
    } else {
        // If n is odd, simply carry down the remaining integer
        mpz_set(z, x[n - 1]);
    }

    // Base case
    if(p == 1) {
        // If there was only one group, we're done
        return;
    }

    // Swap z back into the array
    mpz_swap(x[p - 1], z);

    // Recurse
    pair_internal(z, p, x);
}

// This implementation copies it's parameters before 
// delegating to the actual recursive pairing function.
void pair(mpz_t target, const size_t n, mpz_t integers[n]) {
    
    // The number of groups of two we can make
    const size_t p = PAIR_COUNT(n);

    // Allocate temporary storage
    mpz_t* tmp = malloc(sizeof(mpz_t) * p);

    // Copy the first p integers into temporary storage
    for(size_t i = 0; i < p; ++i) {
        mpz_init_set(tmp[i], integers[i]);
    }

    // Delegate to the actual pair function
    pair_internal(target, n, integers);

    // Copy the first p integers back into the array
    for(size_t i = 0; i < p; ++i) {
        // Swap out garbage value with original value
        mpz_swap(tmp[i], integers[i]);

        // Free the memory associated with the garbage value
        mpz_clear(tmp[i]);
    }

    // Free temporary storage
    free(tmp);
}

// This implementation copies it's parameters before
// delegating to the actual recursive pairing function.
void pair_ui(mpz_t target, const size_t n, size_t integers_ui[n]) {
    
    // Allocate storage for MPZ integers
    mpz_t* integers = malloc(sizeof(mpz_t) * n);

    // Convert the provided integers to MPZ integers
    for(size_t i = 0; i < n; ++i) {
        mpz_init_set_ui(integers[i], integers_ui[i]);
    }

    // Delegate to the actual pair function
    pair_internal(target, n, integers);

    // Clean up
    for(size_t i = 0; i < n; ++i) {
        mpz_clear(integers[i]);
    }

    // Free the MPZ integers
    free(integers);
}

// Delegates to the actual recursive unpairing function.
void unpair(const size_t n, mpz_t res[n], const mpz_t z) {

    // Base case #1
    if(n == 1) {
        // Copy the sole remaining integer in this branch and return
        mpz_set(res[0], z);
        return;
    }

    // Base case #2
    if(n == 2) {
        // Split the sole remaining integer in this branch and return
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

        // Half of n, rounded down
        const size_t mid = n >> 1;

        // Unpair x (recurse)
        unpair(mid, res, x);

        // Unpair y (recurse)
        unpair(mid, res + mid, y);
    } else {

        // Find the nearest power of 2 <= n
        const size_t npt = NPT(n);

        // Unpair x (recurse)
        unpair(npt, res, x);

        // Unpair y (recurse)
        unpair(n - npt, res + npt, y);
    }

    // Clean up
    mpz_clear(x);
    mpz_clear(y);
}
