#ifndef RTMATH_H
#define RTMATH_H

/* This contains some extra math/utility definitions for ray tracing. */

/* Delta function - equals 1 if x equals zero, 0 otherwise. Note the very generous delta epsilon. */
#define delta(x) (float)(std::abs(x) <= 1e-3f)

/* Uniform number generation (for C++11 PRNG's only). */
#define RandomVariable(x) (((*x)() - x->min()) / (float)x->max())

#endif
