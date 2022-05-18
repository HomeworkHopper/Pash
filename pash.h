#ifndef PASH_H
#define PASH_H

#include <gmp.h>

void pair(mpz_t z, size_t n, mpz_t x[n]);

void unpair(size_t n, mpz_t res[n], const mpz_t z);

#endif
