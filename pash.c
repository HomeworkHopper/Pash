// File: pash.c
//
// Description: Implementation of the pash module. Pair/unpair functions
//              delegate to internal recursive functions, and the underlining
//              Szudzik pairing/unpairing operations are defined as macros.
//
// @author      Shaun Thornton
//
// // // // // // // // // // // // // // // // // // // // // // // // //

#include <ctype.h>      // size_t
#include <stdlib.h>     // malloc & free
#include <assert.h>     // assertions

#include "pash.h"       // Module interface

// If we're not using GNU C, elide __attribute__
#ifndef __GNUC__
    #define  __attribute__(x)  /*NOTHING*/
#endif

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

/// Inlined helper function which finds the nearest power of 
/// two less than or equal to the provided integer value.
///
/// @param n    an unsigned, non-zero number
/// @returns    the nearest power of two <= n
static inline __attribute__((always_inline)) size_t NPT(size_t n) {

    // Use a (fast) GNU C builtin function if possible
    #ifdef __GNUC__
        // This will ideally use the Bit Scan Reverse (BSR) instruction
        // if the CPU supports it, but will at the very least generate
        // something more efficent than the fallback implementation below.
        return  0x80000000 >> __builtin_clz(n);
    #else
        // Fallback implementation (Hacker's Delight, First Edition, page 47)
        n =     n | (n >> 1);
        n =     n | (n >> 2);
        n =     n | (n >> 4);
        n =     n | (n >> 8);
        n =     n | (n >>16);
        return  n - (n >> 1);
    #endif
}

/// An internal helper function which complements the pair function.
///
/// @param z    the resulting integer's destination
/// @param n    the number of integers to pair
/// @param x    the array of integers to pair
static void pair_internal(mpz_t z, const size_t n, mpz_t x[n]) {
 
    // The number of groups of two we can make
    const size_t p = PAIR_COUNT(n);

    // Pair each group of two
    for(register size_t i = 0, j = 0; i < p - 1; ++i, j += 2) {
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

/// An internal helper function which complements the unpair function.
///
/// @param n    the number of integers to produce
/// @param res  the resulting integers' destination
/// @param z    the integer to unpair
static void unpair_internal(const size_t n, mpz_t res[n], const mpz_t z) {

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
        unpair_internal(mid, res, x);

        // Unpair y (recurse)
        unpair_internal(mid, res + mid, y);
    } else {

        // Find the nearest power of 2 <= n
        const size_t npt = NPT(n);

        // Unpair x (recurse)
        unpair_internal(npt, res, x);

        // Unpair y (recurse)
        unpair_internal(n - npt, res + npt, y);
    }

    // Clean up
    mpz_clear(x);
    mpz_clear(y);
}

// This implementation checks/copies it's parameters before 
// delegating to the actual recursive pairing function.
void pair(mpz_t target, const size_t n, mpz_t integers[n]) {
    
    // Ensure n > 0
    assert(n);

    // The number of groups of two we can make
    const size_t p = PAIR_COUNT(n);

    // Allocate temporary storage
    mpz_t* tmp = malloc(sizeof(mpz_t) * p);

    // Ensure the memory allocation succeeded
    assert(tmp);

    // Copy the first p integers into temporary storage
    for(register size_t i = 0; i < p; ++i) {
        mpz_init_set(tmp[i], integers[i]);
    }

    // Delegate to the actual pair function
    pair_internal(target, n, integers);

    // Copy the first p integers back into the array
    for(register size_t i = 0; i < p; ++i) {
        // Swap out garbage value with original value
        mpz_swap(tmp[i], integers[i]);

        // Free the memory associated with the garbage value
        mpz_clear(tmp[i]);
    }

    // Free temporary storage
    free(tmp);
}

// This implementation checks/copies it's parameters before
// delegating to the actual recursive pairing function.
void pair_ui(mpz_t target, const size_t n, size_t integers_ui[n]) {
    
    // Ensure n > 0
    assert(n);

    // Allocate storage for MPZ integers
    mpz_t* integers = malloc(sizeof(mpz_t) * n);

    // Ensure the memory allocation succeeded
    assert(integers);

    // Convert the provided integers to MPZ integers
    for(register size_t i = 0; i < n; ++i) {
        mpz_init_set_ui(integers[i], integers_ui[i]);
    }

    // Delegate to the actual pair function
    pair_internal(target, n, integers);

    // Clean up
    for(register size_t i = 0; i < n; ++i) {
        mpz_clear(integers[i]);
    }

    // Free the MPZ integers
    free(integers);
}

// This implementation checks it's parameters before
// delegating to the actual recursive unpairing function.
void unpair(const size_t n, mpz_t target[n], const mpz_t integer) {
    
    // Ensure n > 0
    assert(n);

    // Delegate to the actual unpair function
    unpair_internal(n, target, integer);
}
