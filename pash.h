#ifndef PASH_H
#define PASH_H

#include <gmp.h>

void pair(mpz_t z, const mpz_t x, const mpz_t y);

void unpair(mpz_t x, mpz_t y, const mpz_t z);

void multi_pair(mpz_t z, size_t n, mpz_t x[n]);

void multi_unpair(size_t n, mpz_t res[n], const mpz_t z);

#endif
