#include <stdio.h>
#include <stdlib.h>

#include "pash.h"

int main() {

    // Source integers
    size_t arr[] = {69, 420, 1234};

    // Target integers
    mpz_t z;
    mpz_init(z);

    // Pair the integers into z
    pair_ui(z, 3, arr);

    // Print z
    gmp_printf("%Zd\n", z);

    // Result integers
    mpz_t res[3];

    // Initialize result integers
    for(int i = 0; i < 3; ++i) {
        mpz_init(res[i]);
    }

    // Unpair z into result integers
    unpair(3, res, z);

    // Print result integers, should equal initial integers
    gmp_printf("%Zd, %Zd, %Zd\n", res[0], res[1], res[2]);
}
