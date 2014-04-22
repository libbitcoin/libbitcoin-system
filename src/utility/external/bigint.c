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
//#include <bitcoin/utility/external/bigint.h>
//
//#include <stdlib.h>
//#include <string.h>
//
///*
// * General note:
// * The variable name `si` always means source iterator and `di` always means
// * destination iterator.
// */
//
///* ==== Internal helpers ================================================== */
//
//static uint8_t x00[] = {0x00};
//static uint8_t xff[] = {0xff};
//static bigint_t const bigint_0  = {x00, x00 + 1};
//static bigint_t const bigint_n1 = {xff, xff + 1};
//
//static bigint_t bigint_new(size_t size)
//{
//    bigint_t out;
//    out.lsb = (uint8_t*)malloc(size);
//    out.end = out.lsb + size;
//    return out;
//}
//
///**
// * Trims unnecessary MSB's off the end of a bigint.
// * Bigints are not allowed to have unnecessary bytes, so trimming should be
// * the final step in any function that creates or manipulates a bigint.
// */
//static bigint_t bigint_trim(bigint_t in)
//{
//    while (in.lsb + 1 < in.end &&
//        in.end[-1] == (uint8_t)-(in.end[-2] >> 7))
//        --in.end;
//    return in;
//}
//
///**
// * Swaps the inputs if in0 is longer than in1, making in1 the longer one.
// * Knowing which input is longer generally simplifies sign-extension.
// * @return the size of the longest input.
// */
//static size_t bigint_swap(bigint_t *in0, bigint_t *in1)
//{
//    if (bigint_size(*in1) < bigint_size(*in0)) {
//        bigint_t temp = *in0;
//        *in0 = *in1;
//        *in1 = temp;
//    }
//    return bigint_size(*in1);
//}
//
///* ==== Lifetime ========================================================== */
//
//void bigint_free(bigint_t in)
//{
//    free(in.lsb);
//}
//
//bigint_t bigint_copy(bigint_t in)
//{
//    size_t size = bigint_size(in);
//    bigint_t out = bigint_new(size);
//    memcpy(out.lsb, in.lsb, size);
//    return out;
//}
//
//bigint_t bigint_nan()
//{
//    bigint_t out = {0, 0};
//    return out;
//}
//
///* ==== Binary conversion ================================================= */
//
//bigint_t bigint_from_int(int in)
//{
//    bigint_t out = bigint_new(sizeof(int));
//    uint8_t *di = out.lsb;
//
//    while (di < out.end) {
//        *di++ = 0xff & in;
//        in >>= 8; /* Undefined for negative numbers! */
//    }
//    return bigint_trim(out);
//}
//
//bigint_t bigint_from_le(uint8_t const *in, size_t size)
//{
//    bigint_t out = bigint_new(size);
//    memcpy(out.lsb, in, size);
//    return bigint_trim(out);
//}
//
//bigint_t bigint_from_be(uint8_t const *in, size_t size)
//{
//    bigint_t out = bigint_new(size);
//    uint8_t const *si = in + size;
//    uint8_t *di = out.lsb;
//
//    while (di < out.end)
//        *di++ = *--si;
//    return bigint_trim(out);
//}
//
//void bigint_to_be(uint8_t *out, size_t size, bigint_t in)
//{
//    uint8_t *di = out + size;
//    uint8_t const *si = in.lsb;
//
//    while (out < di)
//        *--di = si < in.end ? *si++ : bigint_sign(in);
//}
//
//void bigint_to_le(uint8_t *out, size_t size, bigint_t in)
//{
//    uint8_t *di = out;
//    uint8_t const *si = in.lsb;
//
//    while (di < out + size)
//        *di++ = si < in.end ? *si++ : bigint_sign(in);
//}
//
//int bigint_to_int(bigint_t in)
//{
//    int out;
//    if (*(unsigned short*)"\x00\xff" == 0x00ff)
//        bigint_to_be((unsigned char*)&out, sizeof(int), in);
//    else
//        bigint_to_le((unsigned char*)&out, sizeof(int), in);
//    return out;
//}
//
///* ==== String conversion ================================================= */
//
//bigint_t bigint_from_dec(char const *in)
//{
//    /* The number of bytes per digit is ln(10)/ln(256), which is 0.41524. The
//     * ratio 49/118 is 0.41525, which is a good approximation to this value.
//     * Since the MSB needs to be zero, the result should include one extra bit
//     * of padding, or about 15/118 of a byte. Finally, the result needs to be
//     * rounded up to whole bytes: */
//    bigint_t out = bigint_new((49*strlen(in) + 15 + 117) / 118);
//    char const *si = in;
//
//    /* Clear the result: */
//    memset(out.lsb, 0, bigint_size(out));
//
//    /* For each input digit, out = 10*out + digit: */
//    while (*si) {
//        uint8_t *di = out.lsb;
//        unsigned carry = *si++ - '0';
//
//        while (di < out.end) {
//            carry += 10*(*di);
//            *di++ = carry;
//            carry >>= 8;
//        }
//    }
//
//    return bigint_trim(out);
//}
//
//static unsigned from_hex(char c)
//{
//    return
//        'A' <= c && c <= 'F' ? 10 + c - 'A' :
//        'a' <= c && c <= 'f' ? 10 + c - 'a' :
//        c - '0';
//}
//
//bigint_t bigint_from_hex(char const *in)
//{
//    bigint_t out = bigint_new(1 + strlen(in)/2);
//    uint8_t *di = out.lsb;
//    char const *si = in + strlen(in);
//
//    /* Copy and convert pairs of digits, starting from the LSB: */
//    while (in + 1 < si) {
//        *di =  from_hex(*--si);
//        *di |= from_hex(*--si) << 4;
//        ++di;
//    }
//    if (in + 1 == si)
//        *di = from_hex(*--si);
//    else
//        *di = 0;
//
//    return bigint_trim(out);
//}
//
//static char to_hex(unsigned n)
//{
//    return n < 10 ? '0' + n : 'a' + n - 10;
//}
//
//char *bigint_to_hex(bigint_t in)
//{
//    char *out = (char*)malloc(2*bigint_size(in) + 1);
//    char *di = out;
//    uint8_t const *si = in.end;
//
//    while (in.lsb < si) {
//        --si;
//        *di++ = to_hex(*si >> 4);
//        *di++ = to_hex(*si & 0x0f);
//    }
//    *di++ = 0;
//    return out;
//}
//
///* ==== Comparison ======================================================== */
//
//uint8_t bigint_sign(bigint_t in)
//{
//    return -(in.end[-1] >> 7);
//}
//
//int bigint_is_zero(bigint_t in)
//{
//    return 1 == bigint_size(in) && 0 == in.lsb[0];
//}
//
//int bigint_compare(bigint_t in0, bigint_t in1)
//{
//    uint8_t const *si0, *si1;
//
//    /* Handle sign or length differences: */
//    if (bigint_sign(in0) != bigint_sign(in1))
//        return 1 | -(1 & bigint_sign(in0));
//    if (bigint_size(in0) != bigint_size(in1))
//        return 1 | -((1 & bigint_sign(in0)) ^ 
//            (bigint_size(in0) < bigint_size(in1)));
//
//    /* Both have the same sign and same length, so byte-wise compare: */
//    si0 = in0.end;
//    si1 = in1.end;
//    while (in0.lsb < si0) {
//        --si0; --si1;
//        if (*si0 != *si1)
//            return 1 | -(*si0 < *si1);
//    }
//
//    /* They are exactly the same: */
//    return 0;
//}
//
///* ==== Shifter =========================================================== */
//
//bigint_t bigint_shift(bigint_t in, signed shift)
//{
//    signed bytes = shift >> 3; /* Rounds towards negative infinity */
//    unsigned bits = shift & 0x7;
//    bigint_t out;
//    uint8_t *di;
//    uint8_t const *si;
//
//    /* Stop if the entire integer would be shifted away: */
//    if (bigint_size(in) + bytes + 1 <= 0)
//        return bigint_sign(in) ? bigint_n1 : bigint_0;
//
//    /* Grab space for the new integer: */
//    out = bigint_new(bigint_size(in) + bytes + 1);
//
//    /* Fill in the lower bytes: */
//    if (0 <= bytes) {
//        di = out.lsb + bytes;
//        si = in.lsb;
//        memset(out.lsb, 0, bytes);
//        *di++ = *si++ << bits;
//    } else {
//        di = out.lsb;
//        si = in.lsb - bytes;
//    }
//
//    /* Fill in the middle bytes: */
//    while (si < in.end) {
//        *di++ = si[0] << bits | si[-1] >> (8 - bits);
//        ++si;
//    }
//
//    /* Fill in the MSB: */
//    *di = bigint_sign(in) << bits | si[-1] >> (8 - bits);
//
//    return bigint_trim(out);
//}
//
///* ==== Logic ============================================================= */
//
//bigint_t bigint_not(bigint_t in)
//{
//    bigint_t out = bigint_new(bigint_size(in));
//    uint8_t *di = out.lsb;
//    uint8_t const *si = in.lsb;
//
//    while (si < in.end)
//        *di++ = ~*si++;
//    return bigint_trim(out);
//}
//
//bigint_t bigint_and(bigint_t in0, bigint_t in1)
//{
//    bigint_t out = bigint_new(bigint_swap(&in0, &in1));
//    uint8_t *di = out.lsb;
//    uint8_t const *si0 = in0.lsb;
//    uint8_t const *si1 = in1.lsb;
//
//    while (si0 < in0.end)
//        *di++ = *si0++ & *si1++;
//    while (si1 < in1.end)
//        *di++ = bigint_sign(in0) & *si1++;
//    return bigint_trim(out);
//}
//
//bigint_t bigint_or(bigint_t in0, bigint_t in1)
//{
//    bigint_t out = bigint_new(bigint_swap(&in0, &in1));
//    uint8_t *di = out.lsb;
//    uint8_t const *si0 = in0.lsb;
//    uint8_t const *si1 = in1.lsb;
//
//    while (si0 < in0.end)
//        *di++ = *si0++ | *si1++;
//    while (si1 < in1.end)
//        *di++ = bigint_sign(in0) | *si1++;
//    return bigint_trim(out);
//}
//
//bigint_t bigint_xor(bigint_t in0, bigint_t in1)
//{
//    bigint_t out = bigint_new(bigint_swap(&in0, &in1));
//    uint8_t *di = out.lsb;
//    uint8_t const *si0 = in0.lsb;
//    uint8_t const *si1 = in1.lsb;
//
//    while (si0 < in0.end)
//        *di++ = *si0++ ^ *si1++;
//    while (si1 < in1.end)
//        *di++ = bigint_sign(in0) ^ *si1++;
//    return bigint_trim(out);
//}
//
///* ==== ALU =============================================================== */
//
//bigint_t bigint_neg(bigint_t in)
//{
//    bigint_t out = bigint_new(1 + bigint_size(in));
//    uint8_t *di = out.lsb;
//    uint8_t const *si = in.lsb;
//    unsigned carry = 1;
//
//    while (si < in.end) {
//        carry += (uint8_t)~*si++;
//        *di++ = carry;
//        carry >>= 8;
//    }
//    *di++ = carry + (uint8_t)~bigint_sign(in);
//    return bigint_trim(out);
//}
//
//bigint_t bigint_add(bigint_t in0, bigint_t in1)
//{
//    bigint_t out = bigint_new(1 + bigint_swap(&in0, &in1));
//    uint8_t *di = out.lsb;
//    uint8_t const *si0 = in0.lsb;
//    uint8_t const *si1 = in1.lsb;
//    unsigned carry = 0;
//
//    while (si0 < in0.end) {
//        carry += *si0++ + *si1++;
//        *di++ = carry;
//        carry >>= 8;
//    }
//    while (si1 < in1.end) {
//        carry += bigint_sign(in0) + *si1++;
//        *di++ = carry;
//        carry >>= 8;
//    }
//    *di++ = carry + bigint_sign(in0) + bigint_sign(in1);
//    return bigint_trim(out);
//}
//
///*
// * Algorithm notes:
// * Subtraction is the same as addition, but with the carry pre-set to 1 and
// * one of the inputs inverted. Which input to invert depends on whether or
// * not the inputs have been swapped for length reasons. The flip variables
// * control which input is inverted.
// */
//bigint_t bigint_sub(bigint_t in0, bigint_t in1)
//{
//    unsigned flip1 = bigint_size(in1) < bigint_size(in0) ? 0 : 0xff;
//    unsigned flip0 = (uint8_t)~flip1;
//    bigint_t out = bigint_new(1 + bigint_swap(&in0, &in1));
//    uint8_t *di = out.lsb;
//    uint8_t const *si0 = in0.lsb;
//    uint8_t const *si1 = in1.lsb;
//    unsigned carry = 1;
//
//    while (si0 < in0.end) {
//        carry += (flip0 ^ *si0++) + (flip1 ^ *si1++);
//        *di++ = carry;
//        carry >>= 8;
//    }
//    while (si1 < in1.end) {
//        carry += (flip0 ^ bigint_sign(in0)) + (flip1 ^ *si1++);
//        *di++ = carry;
//        carry >>= 8;
//    }
//    *di++ = carry + (flip0 ^ bigint_sign(in0)) + (flip1 ^ bigint_sign(in1));
//    return bigint_trim(out);
//}
//
///* ==== Multiplier & Divider Support ====================================== */
//
///**
// * Computes the expression out = -out.
// */
//static void bigint_inplace_neg(bigint_t out)
//{
//    uint8_t *di = out.lsb;
//    unsigned carry = 1;
//
//    while (di < out.end) {
//        carry += (uint8_t)~*di;
//        *di++ = carry;
//        carry >>= 8;
//    }
//}
//
///**
// * Computes the expression out += in * 256^(shift).
// * Setting flip to 0xff performs subtraction instead.
// */
//static void bigint_inplace_add(bigint_t out, bigint_t in,
//    unsigned flip, unsigned shift)
//{
//    uint8_t *di = out.lsb + shift;
//    unsigned char const *si = in.lsb;
//    unsigned carry = 1 & flip;
//
//    while (si < in.end) {
//        carry += (flip ^ *si++) + *di;
//        *di++ = carry;
//        carry >>= 8;
//    }
//    while (di < out.end) {
//        carry += (flip ^ bigint_sign(in)) + *di;
//        *di++ = carry;
//        carry >>= 8;
//    }
//}
//
///**
// * Computes the expression out += k * in * 256^shift.
// * Setting flip to 0xff performs subtraction instead.
// */
//static void bigint_inplace_mad(bigint_t out, bigint_t in,
//    unsigned flip, unsigned shift, uint8_t k)
//{
//    uint8_t *di = out.lsb + shift;
//    uint8_t const *si = in.lsb;
//    unsigned carry = k & flip; /* Since -k*in = k*(~in + 1) = k*~in + k */
//
//    while (si < in.end) {
//        carry += k*(flip ^ *si++) + *di; /* Total is always <= 0xffff */
//        *di++ = carry;
//        carry >>= 8;
//    }
//    while (di < out.end) {
//        carry += k*(flip ^ bigint_sign(in)) + *di;
//        *di++ = carry;
//        carry >>= 8;
//    }
//}
//
///* ==== Multiplier & Divider ============================================== */
//
///*
// * Algorithm notes:
// * If in0 is an unsigned positive number, each of its bytes can act as an
// * independent factor. The idea is to multiply each unsigned byte of in0
// * with the complete signed in1, shift the result, and add the product to an
// * accumulator. Once the multiplications are done, the accumulator holds the
// * complete product.
// *
// * This only works if in0 is an unsigned positive number. If in0 is negative,
// * the algorithm multiplies both in0 and in1 by -1, which makes in0 positive
// * and doesn't change the final result.
// *
// * Rather than make temporary copies of the two inputs, the algorithm just
// * changes the sign on the fly as it reads each byte.
// *
// * Note that in0 acts as an unsigned integer after flipping, so a byte like
// * 0x80 is actually positive.
// *
// * There are faster algorithms like Karatsuba multiplication, but this
// * approach should work fine for pretty much any real-world use case.
// */
//bigint_t bigint_mul(bigint_t in0, bigint_t in1)
//{
//    int i; /* ev: Fixes signed mismatch by avoiding size_t here. */
//    bigint_swap(&in0, &in1);
//    bigint_t out = bigint_new(bigint_size(in0) + bigint_size(in1));
//    unsigned flip = bigint_sign(in0);
//    unsigned carry = 1 & flip;
//
//    /* Clear the accumulator: */
//    memset(out.lsb, 0, bigint_size(out));
//
//    /* For each byte in in0: */
//    for (i = 0; i < bigint_size(in0); ++i) {
//        /* Sign-adjust in0 and multiply: */
//        carry += (flip ^ in0.lsb[i]);
//        bigint_inplace_mad(out, in1, flip, i, carry);
//        carry >>= 8;
//    }
//    return bigint_trim(out);
//}
//
///**
// * The core division algorithm for one-byte denominators. This is a part of
// * the larger bigint_div algorithm.
// *
// * The denominator is passed in d, and the numerator is passed in out.r.
// * The algorithm treats the numerator as an unsigned number, and divides it
// * by the absolute value of the denominator. The numerator must be at least
// * one byte longer than the denominator.
// *
// * The implementation uses byte-wise grade-school long division.
// */
//static void bigint_div_core_1(bigint_t q, bigint_t r, bigint_t d)
//{
//    unsigned flip = bigint_sign(d);
//    unsigned abs_d = (flip ^ *d.lsb) + (1 & flip); /* abs_d = |d| */
//    unsigned i = (unsigned)(bigint_size(r) - 1);
//
//    while (i--) {
//        unsigned n = r.lsb[i + 1] << 8 | r.lsb[i];
//        unsigned guess = n / abs_d;
//        bigint_inplace_mad(r, d, (uint8_t)~flip, i, guess);
//        q.lsb[i] = guess;
//    }
//}
//
///**
// * Returns 1 if there are no set bits below the denominator's two most-
// * significant bytes. This is needed by the bigint_div_core_n algorithm.
// */
//static unsigned bigint_div_core_n_carry(bigint_t in)
//{
//    uint8_t const *si = in.lsb;
//    while (si < in.end - 2)
//        if (*si++)
//            return 0;
//    return 1;
//}
//
///**
// * The core division algorithm for multi-byte denominators. This is a part of
// * the larger bigint_div algorithm.
// *
// * The parameters are the same as for bigint_div_core_1.
// *
// * The implementation uses byte-wise grade-school long division, except that
// * there is no way to get a perfectly-accurate guess for the quotient byte
// * on the first try. Instead, the algorithm just looks at the denominator's
// * first two bytes and assumes the rest are zero. This might cause it to guess
// * high, which would produce a negative remainder. If this happens, it fixes
// * the guess and adjusts the remainder accordingly.
// *
// * The guess can only be off by 1 in the worst case. The largest possible
// * 2-byte numerator is 0x7fff, and the smallest possible 2-byte denominator
// * is 0x80. If we guess 0x7fff / 0x80 = 0xff, the real answer could be as low
// * as 0x7fff / 0x81 = 0xfe.
// */
//static void bigint_div_core_n(bigint_t q, bigint_t r, bigint_t d)
//{
//    unsigned flip = bigint_sign(d);
//    unsigned abs_d = /* abs_d = |d| (first two bytes) */
//        ((flip ^ d.end[-1]) << 8 | (flip ^ d.end[-2])) +
//        (bigint_div_core_n_carry(d) & flip);
//    unsigned i = (unsigned)(bigint_size(r) - bigint_size(d));
//    uint8_t *pn = r.lsb + bigint_size(d) - 2;
//
//    while (i--) {
//        unsigned n = pn[i + 2] << 16 | pn[i + 1] << 8 | pn[i];
//        unsigned guess = n / abs_d;
//        bigint_inplace_mad(r, d, (uint8_t)~flip, i, guess);
//
//        /* Put one back if we subtracted too many: */
//        if (bigint_sign(r)) {
//            guess -= 1;
//            bigint_inplace_add(r, d, flip, i);
//        }
//
//        q.lsb[i] = guess;
//    }
//}
//
///*
// * Algorithm notes:
// * The core division algorithms operate byte-by-byte, which means that they
// * can't handle negative numbers. Therefore, any cases with negative numbers
// * need to be reduced to cases with only positive numbers.
// *
// * The first step is to make the denominator positive by changing its sign
// * along with the numerator and remainder's signs. This makes the remainder
// * positive too, since it has the same sign as the denominator. This doesn't
// * affect rounding behavior, since the quotient's sign stays the same.
// *
// * The numerator and remainder live in temporary storage, so they can be
// * changed in-place. The denominator must be changed on-the-fly as it is read,
// * which the core algorithms handle.
// *
// * At this point only the numerator can be negative, which implies a negative
// * quotient as well. This case can be solved by setting the quotient's most-
// * significant padding byte to 0xff, and then performing one round of
// * multiply-subtraction. This round of multiply-subtraction is actually an
// * addition, since the multiplication factor is -1. This addition takes place
// * one byte to the left of the numerator's MSB, since it involves the
// * quotient's padding area. This is guaranteed to make the numerator positive,
// * since the shifted denominator is larger than the numerator. Once this is
// * done, the division can proceed as if everything were positive (even though
// * the quotient's MSB is negative).
// *
// * This works because a 2's complement negative number is actually a positive
// * number plus -1*(2^bits). This negative component lives to the left of the
// * physical bits, so it normally never manifests itself except during sign-
// * extension. This fixup makes the negative part explicit, inserting it
// * directly into the quotient and compensating for its effects on the
// * numerator. Once this is done, the division operates on just the remaining
// * positive bits to the right. The division still rounds downwards, but
// * downwards is towards negative infinity now.
// */
//bigint_div_t bigint_div(bigint_t n, bigint_t d)
//{
//    bigint_t q, r;
//    bigint_div_t out;
//    unsigned flip = bigint_sign(d);
//    unsigned places = (unsigned)(1 + bigint_size(n) - bigint_size(d));
//
//    /* Stop if the denominator is longer than the numerator: */
//    if (bigint_size(n) < bigint_size(d)) {
//        if (bigint_sign(n) != bigint_sign(d)) { /* Negative quotient */
//            out.q = bigint_copy(bigint_n1);
//            out.r = bigint_add(n, d); /* N - q*D = N + D */
//        } else {
//            out.q = bigint_copy(bigint_0);
//            out.r = bigint_copy(n);
//        }
//        return out;
//    }
//
//    /* Copy and sign-convert the numerator into the working area: */
//    r = bigint_new(1 + bigint_size(n));
//    memset(r.lsb, 0, bigint_size(r));
//    bigint_inplace_add(r, n, flip, 0);
//
//    /* Allocate space for the quotient: */
//    q = bigint_new(1 + places);
//    q.lsb[places] = bigint_sign(r);
//
//    /* Fix negative numerators: */
//    if (bigint_sign(r))
//        bigint_inplace_add(r, d, flip, places);
//
//    /* Perform the division: */
//    if (1 == bigint_size(d))
//        bigint_div_core_1(q, r, d);
//    else
//        bigint_div_core_n(q, r, d);
//
//    /* Fix the output: */
//    if (flip)
//        bigint_inplace_neg(r);
//    out.q = bigint_trim(q);
//    out.r = bigint_trim(r);
//    return out;
//}
