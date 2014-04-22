///*
// * Copyright 2013 William Swanson
// *
// * This program is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License, version 2, as
// * published by the Free Software Foundation.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// */
//#ifndef BIGINT_H_INCLUDED
//#define BIGINT_H_INCLUDED
//
//#include <stddef.h>
//#include <stdint.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
///**
// * An arbitrary-width integer, which can optionally represent a NaN (not a
// * number) value. NaN's are generated when an error occurs, such as dividing
// * by zero or running out of memory. They propagate silently, so it is only
// * necessary to have only a single error check at the end of a calculation.
// *
// * The bigint_t struct acts like a const pointer type. It may be passed around
// * cheaply, but must only be freed once. Once a bigint is created, there are
// * no functions for modifying its internal data. Instead, all math operations
// * create new bigints to hold their results.
// *
// * ## Implementation details
// *
// * The bytes are stored LSB-first within the array. A zero-length array
// * indicates a NaN value.
// *
// * The number is signed and stored in 2's compliment. Unsigned numbers can
// * use an all-zero MSB to avoid being negative in cases where this would
// * normally be a problem.
// *
// * The data array must be trimmed to the shortest length possible, so extra
// * sign-extension bytes are not allowed. This means that there is only one
// * correct way to encode any particular integer.
// */
//typedef struct {
//    uint8_t *lsb;
//    uint8_t *end;
//} bigint_t;
//
///**
// * Releases the memory used by a bigint.
// */
//void bigint_free(bigint_t in);
//
///**
// * Copies the memory held by a bigint.
// */
//bigint_t bigint_copy(bigint_t in);
//
///**
// * Constructs a bigint NaN value.
// */
//bigint_t bigint_nan();
//
///**
// * Converts the host's native integer type to a bigint.
// */
//bigint_t bigint_from_int(int in);
//
///**
// * Creates a bigint from signed little-endian data.
// */
//bigint_t bigint_from_le(uint8_t const *in, size_t size);
//
///**
// * Creates a bigint from signed big-endian data.
// */
//bigint_t bigint_from_be(uint8_t const *in, size_t size);
//
///**
// * Converts a bigint to the host's native integer type.
// */
//int bigint_to_int(bigint_t in);
//
///**
// * Converts a bigint to a fixed-width little-endian integer, either by
// * truncation or by sign extension.
// */
//void bigint_to_le(uint8_t *out, size_t size, bigint_t in);
//
///**
// * Converts a bigint to a fixed-width big-endian integer, either by
// * truncation or by sign extension.
// */
//void bigint_to_be(uint8_t *out, size_t size, bigint_t in);
//
///**
// * Converts a string of decimal characters to a bigint. Does not validate
// * the input for correctness.
// */
//bigint_t bigint_from_dec(char const *in);
//
///**
// * Converts a string of hexadecimal characters to a bigint.
// */
//bigint_t bigint_from_hex(char const *in);
//
///**
// * Converts a bigint to a string of hexadecimal digits.
// * The returned string should be released with free().
// */
//char *bigint_to_hex(bigint_t in);
//
///**
// * Returns true if the bigint is a NaN value.
// */
//#define bigint_is_nan(a) ((a).end == (a).lsb)
//
///**
// * Returns the number of bytes needed to represent the bigint.
// */
//#define bigint_size(a) ((a).end - (a).lsb)
//
///**
// * Returns a sign-extension byte for the bigint, either 0xff or 0x00.
// */
//uint8_t bigint_sign(bigint_t in);
//
///**
// * Returns true if a bigint is zero
// */
//int bigint_is_zero(bigint_t in);
//
///**
// * Compares two numbers and returns 0 if they are the same, 1 if in1 is
// * smaller, or -1 if in0 is smaller.
// */
//int bigint_compare(bigint_t in0, bigint_t in1);
//
///**
// * Performs a left shift. If the shift amount is negative, this is the same
// * as a right shift.
// */
//bigint_t bigint_shift(bigint_t in, signed shift);
//
///* Standard logic operations: */
//bigint_t bigint_not(bigint_t in);
//bigint_t bigint_and(bigint_t in0, bigint_t in1);
//bigint_t bigint_or (bigint_t in0, bigint_t in1);
//bigint_t bigint_xor(bigint_t in0, bigint_t in1);
//
///* Standard ALU operations: */
//bigint_t bigint_neg(bigint_t in);
//bigint_t bigint_add(bigint_t in0, bigint_t in1);
//bigint_t bigint_sub(bigint_t in0, bigint_t in1);
//bigint_t bigint_mul(bigint_t in0, bigint_t in1);
//
///**
// * Return value for division.
// */
//typedef struct {
//    bigint_t q; /* Quotient */
//    bigint_t r; /* Remainder */
//} bigint_div_t;
//
///**
// * Divides two bigints, rounding towards negative infinity. The results
// * satisfy the relationship N/D = q + r/D. Since the division rounds towards
// * negative infinity, the r/D part is always positive, which means that the
// * remainder r always has the same sign as the denominator D. Here are some
// * examples:
// *
// *   N |  D |  q |  r
// * ----|----|----|----
// *   7 |  3 |  2 |  1
// *  -7 |  3 | -3 |  2
// *   7 | -3 | -3 | -2
// *  -7 | -3 |  2 | -1
// *
// * The denominator must never be 0.
// */
//bigint_div_t bigint_div(bigint_t inN, bigint_t inD);
//
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif
