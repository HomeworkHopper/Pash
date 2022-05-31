#include <stdio.h>
#include <stdlib.h>

#include "pash.h"

#define N 6    // The number of integers to pair

int main() {

    // Starting array of integers
    mpz_t* in = malloc(sizeof(mpz_t) * N);

    // Initialize and print each integer
    for(int i = 0; i < N; ++i) {
        mpz_init_set_ui(in[i], i + 1);
        gmp_printf("%Zd, ", in[i]);
    }

    // Declare and initialize the paired output
    mpz_t z;
    mpz_init(z);

    // Pair the array of integers into a single integer (z)
    pair(z, N, in);

    // Print the generated integer
    gmp_printf("\n%Zd\n", z);

    mpz_t out[N];
    for(int i = 0; i < N; ++i) {
        mpz_init(out[i]);
 //     mpz_clear(in[i]);
    }

    // Split z back into the original integers
    unpair(N, out, z);

    mpz_clear(z);

    // Print the split integers
    for(int i = 0; i < N; ++i) {
        gmp_printf("%Zd, ", in[i]);
        mpz_clear(out[i]);
        mpz_clear(in[i]);
    }

    printf("\n");

    free(in);
}
