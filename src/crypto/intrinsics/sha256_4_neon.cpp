/* sha256-arm.c - ARMv8 SHA extensions using C intrinsics     */
/*   Written and placed in public domain by Jeffrey Walton    */
/*   Based on code from ARM, and by Johannes Schneiders, Skip */
/*   Hovsmith and Barry O'Rourke for the mbedTLS project.     */

/* For some reason we need to use the C++ compiler. Otherwise       */
/* all the intrinsics functions, like vsha256hq_u32, are missing.   */
/* GCC118 on the compile farm with GCC 4.8.5 suffers the issue.     */
/* g++ -DTEST_MAIN -march=armv8-a+crypto sha256-arm.c -o sha256.exe */

#include <bitcoin/system/crypto/intrinsics/intrinsics.hpp>

#ifdef WITH_NEON

#include <stdint.h>
#include <stdlib.h>
#include <bitcoin/system/serial/serial.hpp>

// Win32: unless made conditional on compiler version, this is the new minium:
// devblogs.microsoft.com/visualstudio/visual-studio-2017-version-15-9-preview-3
// TODO: with neon defined must pull in a library, do via project.props.
#if defined(_MSC_VER)
# define _M_ARM
# define _ARM_USE_NEW_NEON_INTRINSICS
#endif // _MSC_VER

#if defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM)
# if defined(__GNUC__)
#  include <stdint.h>
# endif
# if defined(__ARM_NEON) || defined(_MSC_VER) || defined(__GNUC__)
#  include <arm_neon.h>
# endif
/* GCC and LLVM Clang, but not Apple Clang */
# if defined(__GNUC__) && !defined(__apple_build_version__)
#  if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
#   include <arm_acle.h>
#  endif
# endif
#endif  /* ARM Headers */

namespace libbitcoin {
namespace system {
namespace intrinsics {

static const uint32_t K[] =
{
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

void inline initialize(uint32_t state[8])
{
    state[0] = 0x6a09e667ul;
    state[1] = 0xbb67ae85ul;
    state[2] = 0x3c6ef372ul;
    state[3] = 0xa54ff53aul;
    state[4] = 0x510e527ful;
    state[5] = 0x9b05688cul;
    state[6] = 0x1f83d9abul;
    state[7] = 0x5be0cd19ul;
}

/* Process multiple blocks. The caller is responsible for setting the initial */
/*  state, and the caller is responsible for padding the final block.        */
void sha256_x4_neon(uint32_t* state, const uint8_t* data, uint32_t blocks)
{
    uint32x4_t STATE0, STATE1, ABEF_SAVE, CDGH_SAVE;
    uint32x4_t MSG0, MSG1, MSG2, MSG3;
    uint32x4_t TMP0, TMP1, TMP2;

    /* Load state */
    STATE0 = vld1q_u32(&state[0]);
    STATE1 = vld1q_u32(&state[4]);

    while (blocks--)
    {
        /* Save state */
        ABEF_SAVE = STATE0;
        CDGH_SAVE = STATE1;

        /* Load message */
        MSG0 = vld1q_u32((const uint32_t *)(data +  0));
        MSG1 = vld1q_u32((const uint32_t *)(data + 16));
        MSG2 = vld1q_u32((const uint32_t *)(data + 32));
        MSG3 = vld1q_u32((const uint32_t *)(data + 48));

        /* Reverse for little endian */
        MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
        MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
        MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
        MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));

        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x00]));

        /* Rounds 0-3 */
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x04]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        /* Rounds 4-7 */
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x08]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        /* Rounds 8-11 */
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x0c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        /* Rounds 12-15 */
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x10]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        /* Rounds 16-19 */
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x14]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        /* Rounds 20-23 */
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x18]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        /* Rounds 24-27 */
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x1c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        /* Rounds 28-31 */
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x20]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        /* Rounds 32-35 */
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x24]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        /* Rounds 36-39 */
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x28]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        /* Rounds 40-43 */
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x2c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        /* Rounds 44-47 */
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&K[0x30]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        /* Rounds 48-51 */
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&K[0x34]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        /* Rounds 52-55 */
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&K[0x38]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        /* Rounds 56-59 */
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&K[0x3c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        /* Rounds 60-63 */
        TMP2 = STATE0;
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        /* Combine state */
        STATE0 = vaddq_u32(STATE0, ABEF_SAVE);
        STATE1 = vaddq_u32(STATE1, CDGH_SAVE);

        data += 64;
    }

    /* Save state */
    vst1q_u32(&state[0], STATE0);
    vst1q_u32(&state[4], STATE1);
}

void double_sha256_64x4_neon(uint8_t* out, const uint8_t* in) noexcept
{
    uint32_t state[8];

    static const uint8_t padding1[64] =
    {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0
    };

    uint8_t buffer2[64] =
    {
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0
    };

    initialize(state);
    sha256_x4_neon(state, in, 1);
    sha256_x4_neon(state, padding1, 1);
    to_big_endian(buffer2 + 0, state[0]);
    to_big_endian(buffer2 + 4, state[1]);
    to_big_endian(buffer2 + 8, state[2]);
    to_big_endian(buffer2 + 12, state[3]);
    to_big_endian(buffer2 + 16, state[4]);
    to_big_endian(buffer2 + 20, state[5]);
    to_big_endian(buffer2 + 24, state[6]);
    to_big_endian(buffer2 + 28, state[7]);

    initialize(state);
    sha256_x4_neon(state, buffer2, 1);
    to_big_endian(out + 0, state[0]);
    to_big_endian(out + 4, state[1]);
    to_big_endian(out + 8, state[2]);
    to_big_endian(out + 12, state[3]);
    to_big_endian(out + 16, state[4]);
    to_big_endian(out + 20, state[5]);
    to_big_endian(out + 24, state[6]);
    to_big_endian(out + 28, state[7]);
}

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

#endif // WITH_NEON
