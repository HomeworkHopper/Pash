// File: pash.h
//
// Description: Interface for the pash module
//
// Author:      Shaun Thornton

#ifndef PASH_H
#define PASH_H

// Use the GNU Multiple Precision Arithmetic Library
#include <gmp.h>

/// A function which pairs the provided n integers into a single integer.
///
/// @param target   the resulting integer's destination
/// @param n        the number of integers to pair
/// @param integers the array of integers to pair
void pair(mpz_t target, const size_t n, mpz_t integers[n]);

/// A function which unpairs an integer into n seperate integers.
///
/// @param n        the number of integers to produce
/// @param target   the resulting integers' destination
/// @param integer  the integer to unpair
void unpair(const size_t n, mpz_t target[n], const mpz_t integer);

#endif
