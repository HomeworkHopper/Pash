#include <stdio.h>
#include <stdlib.h>

#include "pash.h"

#define N 1000000

int main() {

    setbuf(stdout, NULL);

    // Starting array of integers
    mpz_t* in = malloc(sizeof(mpz_t) * N);

    // Initialize and print each integer
    for(int i = 0; i < N; ++i) {
        mpz_init_set_ui(in[i], i + 1);
 //       gmp_printf("%Zd, ", in[i]);
    }

    // Declare and initialize the paired output
    mpz_t z;
    mpz_init(z);

    fprintf(stdout, "Starting pair... ");

    // Pair the array of integers into a single integer (z)
    pair(z, N, in);

    fprintf(stdout, "Done\n");

    // Print the generated integer
   // gmp_printf("\n%Zd\n", z);

    // Declare and initialize the array to split z back into
    mpz_t out[N];
    for(int i = 0; i < N; ++i) {
        mpz_init(out[i]);
        mpz_clear(in[i]);
    }
    
    fprintf(stdout, "Starting unpair... ");

    // Split z back into the original integers
    unpair(N, out, z);

    fprintf(stdout, "Done\n");

    mpz_clear(z);

    // Print the split integers
    for(int i = 0; i < N; ++i) {
     //   gmp_printf("%Zd, ", out[i]);
        mpz_clear(out[i]);
    }
}
