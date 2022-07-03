/* sha256-x86.c - Intel SHA extensions using C intrinsics  */
/*   Written and place in public domain by Jeffrey Walton  */
/*   Based on code from Intel, and by Sean Gulley for      */
/*   the miTLS project.                                    */

// Modifications by:
// Copyright (c) 2017-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/system/crypto/intrinsics/intrinsics.hpp>

#ifdef WITH_SHANI

#include <stdint.h>
#include <immintrin.h>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {
namespace intrinsics {

alignas(__m128i) const uint8_t flip_mask[16] =
{
    0x03, 0x02, 0x01, 0x00, // 0x00010203ul
    0x07, 0x06, 0x05, 0x04, // 0x04050607ul
    0x0b, 0x0a, 0x09, 0x08, // 0x08090a0bul
    0x0f, 0x0e, 0x0d, 0x0c  // 0x0c0d0e0ful
};

alignas(__m128i) const uint8_t init0[16] =
{
    0x8c, 0x68, 0x05, 0x9b, // 0x9b05688cul
    0x7f, 0x52, 0x0e, 0x51, // 0x510e527ful
    0x85, 0xae, 0x67, 0xbb, // 0xbb67ae85ul
    0x67, 0xe6, 0x09, 0x6a  // 0x6a09e667ul
};

alignas(__m128i) const uint8_t init1[16] =
{
    0x19, 0xcd, 0xe0, 0x5b, // 0x5be0cd19ul
    0xab, 0xd9, 0x83, 0x1f, // 0x1f83d9abul
    0x3a, 0xf5, 0x4f, 0xa5, // 0xa54ff53aul
    0x72, 0xf3, 0x6e, 0x3c  // 0x3c6ef372ul
};

void inline QuadRound(__m128i& state0, __m128i& state1, uint64_t k1,
    uint64_t k0) NOEXCEPT
{
    const __m128i msg = _mm_set_epi64x(k1, k0);
    state1 = _mm_sha256rnds2_epu32(state1, state0, msg);
    state0 = _mm_sha256rnds2_epu32(state0, state1, _mm_shuffle_epi32(msg, 0x0e));
}

void inline QuadRound(__m128i& state0, __m128i& state1, __m128i m, uint64_t k1,
    uint64_t k0) NOEXCEPT
{
    const __m128i msg = _mm_add_epi32(m, _mm_set_epi64x(k1, k0));
    state1 = _mm_sha256rnds2_epu32(state1, state0, msg);
    state0 = _mm_sha256rnds2_epu32(state0, state1, _mm_shuffle_epi32(msg, 0x0e));
}

void inline ShiftMessageA(__m128i& m0, __m128i m1) NOEXCEPT
{
    m0 = _mm_sha256msg1_epu32(m0, m1);
}

void inline ShiftMessageC(__m128i& m0, __m128i m1, __m128i& m2) NOEXCEPT
{
    m2 = _mm_sha256msg2_epu32(_mm_add_epi32(m2, _mm_alignr_epi8(m1, m0, 4)), m1);
}

void inline ShiftMessageB(__m128i& m0, __m128i m1, __m128i& m2) NOEXCEPT
{
    ShiftMessageC(m0, m1, m2);
    ShiftMessageA(m0, m1);
}

void inline Shuffle(__m128i& s0, __m128i& s1) NOEXCEPT
{
    const __m128i t1 = _mm_shuffle_epi32(s0, 0xb1);
    const __m128i t2 = _mm_shuffle_epi32(s1, 0x1b);
    s0 = _mm_alignr_epi8(t1, t2, 0x08);
    s1 = _mm_blend_epi16(t2, t1, 0xf0);
}

void inline Unshuffle(__m128i& s0, __m128i& s1) NOEXCEPT
{
    const __m128i t1 = _mm_shuffle_epi32(s0, 0x1b);
    const __m128i t2 = _mm_shuffle_epi32(s1, 0xb1);
    s0 = _mm_blend_epi16(t1, t2, 0xf0);
    s1 = _mm_alignr_epi8(t2, t1, 0x08);
}

__m128i inline Load(const uint8_t* in) NOEXCEPT
{
    return _mm_shuffle_epi8(_mm_loadu_si128((const __m128i*)in),
        _mm_load_si128((const __m128i*)flip_mask));
}

void inline Save(uint8_t* out, __m128i s) NOEXCEPT
{
    _mm_storeu_si128((__m128i*)out,
        _mm_shuffle_epi8(s, _mm_load_si128((const __m128i*)flip_mask)));
}

// Iterate over N blocks, two lanes per block.
void sha256_shani(uint32_t* state, const uint8_t* chunk, size_t blocks) NOEXCEPT
{
    __m128i m0, m1, m2, m3, s0, s1, so0, so1;

    /* Load state */
    s0 = _mm_loadu_si128((const __m128i*)(state + 0));
    s1 = _mm_loadu_si128((const __m128i*)(state + 4));
    Shuffle(s0, s1);

    // One block in two lanes.
    while (blocks--)
    {
        /* Remember old state */
        so0 = s0;
        so1 = s1;

        /* Load data and transform */
        m0 = Load(chunk);
        QuadRound(s0, s1, m0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
        m1 = Load(chunk + 16);
        QuadRound(s0, s1, m1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
        ShiftMessageA(m0, m1);
        m2 = Load(chunk + 32);
        QuadRound(s0, s1, m2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
        ShiftMessageA(m1, m2);
        m3 = Load(chunk + 48);
        QuadRound(s0, s1, m3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
        ShiftMessageB(m2, m3, m0);
        QuadRound(s0, s1, m0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
        ShiftMessageB(m3, m0, m1);
        QuadRound(s0, s1, m1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
        ShiftMessageB(m0, m1, m2);
        QuadRound(s0, s1, m2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
        ShiftMessageB(m1, m2, m3);
        QuadRound(s0, s1, m3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
        ShiftMessageB(m2, m3, m0);
        QuadRound(s0, s1, m0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
        ShiftMessageB(m3, m0, m1);
        QuadRound(s0, s1, m1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
        ShiftMessageB(m0, m1, m2);
        QuadRound(s0, s1, m2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
        ShiftMessageB(m1, m2, m3);
        QuadRound(s0, s1, m3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
        ShiftMessageB(m2, m3, m0);
        QuadRound(s0, s1, m0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
        ShiftMessageB(m3, m0, m1);
        QuadRound(s0, s1, m1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
        ShiftMessageC(m0, m1, m2);
        QuadRound(s0, s1, m2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
        ShiftMessageC(m1, m2, m3);
        QuadRound(s0, s1, m3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);

        /* Combine with old state */
        s0 = _mm_add_epi32(s0, so0);
        s1 = _mm_add_epi32(s1, so1);

        /* Advance */
        chunk += 64;
    }

    Unshuffle(s0, s1);
    _mm_storeu_si128((__m128i*)(state + 0), s0);
    _mm_storeu_si128((__m128i*)(state + 4), s1);
}

// C-style functions, all usage verified.
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)

// One block in two lanes.
void sha256_x1_shani(uint32_t state[8], const uint8_t block[64]) NOEXCEPT
{
    return sha256_shani(state, block, 1);
}

////void sha256_x2_shani(uint8_t* out, const uint8_t in[2 * 64]) NOEXCEPT
////{
////    // TODO: two blocks in two lanes. 
////}

// One block in two lanes, doubled.
void double_sha256_x1_shani(uint8_t* out, const uint8_t in[1 * 64]) NOEXCEPT
{
    constexpr auto count = 32_size / sizeof(uint32_t);
    auto buffer = sha256x2_buffer;

    auto state = sha256_initial;
    sha256_x1_shani(state.data(), in);
    sha256_x1_shani(state.data(), sha256x2_padding.data());
    to_big_endian(
        *pointer_cast<numbers<count>>(buffer.data()),
        *pointer_cast<const numbers<count>>(state.data()));

    state = sha256_initial;
    sha256_x1_shani(state.data(), buffer.data());
    to_big_endian(
        *pointer_cast<numbers<count>>(out),
        *pointer_cast<const numbers<count>>(state.data()));
}

// Two blocks in two lanes, doubled.
void double_sha256_x2_shani(uint8_t* out, const uint8_t in[2 * 64]) NOEXCEPT
{
    __m128i am0, am1, am2, am3, as0, as1, aso0, aso1;
    __m128i bm0, bm1, bm2, bm3, bs0, bs1, bso0, bso1;

    /* Transform 1 */
    bs0 = as0 = _mm_load_si128((const __m128i*)init0);
    bs1 = as1 = _mm_load_si128((const __m128i*)init1);
    am0 = Load(in);
    bm0 = Load(in + 64);
    QuadRound(as0, as1, am0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
    QuadRound(bs0, bs1, bm0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
    am1 = Load(in + 16);
    bm1 = Load(in + 80);
    QuadRound(as0, as1, am1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    QuadRound(bs0, bs1, bm1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    ShiftMessageA(am0, am1);
    ShiftMessageA(bm0, bm1);
    am2 = Load(in + 32);
    bm2 = Load(in + 96);
    QuadRound(as0, as1, am2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    QuadRound(bs0, bs1, bm2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    ShiftMessageA(am1, am2);
    ShiftMessageA(bm1, bm2);
    am3 = Load(in + 48);
    bm3 = Load(in + 112);
    QuadRound(as0, as1, am3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
    QuadRound(bs0, bs1, bm3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
    QuadRound(bs0, bs1, bm0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    QuadRound(bs0, bs1, bm1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    ShiftMessageB(am0, am1, am2);
    ShiftMessageB(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    QuadRound(bs0, bs1, bm2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    ShiftMessageB(am1, am2, am3);
    ShiftMessageB(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    QuadRound(bs0, bs1, bm3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    QuadRound(bs0, bs1, bm0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    QuadRound(bs0, bs1, bm1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    ShiftMessageB(am0, am1, am2);
    ShiftMessageB(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
    QuadRound(bs0, bs1, bm2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
    ShiftMessageB(am1, am2, am3);
    ShiftMessageB(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    QuadRound(bs0, bs1, bm3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    QuadRound(bs0, bs1, bm0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    QuadRound(bs0, bs1, bm1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    ShiftMessageC(am0, am1, am2);
    ShiftMessageC(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    QuadRound(bs0, bs1, bm2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    ShiftMessageC(am1, am2, am3);
    ShiftMessageC(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);
    QuadRound(bs0, bs1, bm3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);
    as0 = _mm_add_epi32(as0, _mm_load_si128((const __m128i*)init0));
    bs0 = _mm_add_epi32(bs0, _mm_load_si128((const __m128i*)init0));
    as1 = _mm_add_epi32(as1, _mm_load_si128((const __m128i*)init1));
    bs1 = _mm_add_epi32(bs1, _mm_load_si128((const __m128i*)init1));

    /* Transform 2 */
    aso0 = as0;
    bso0 = bs0;
    aso1 = as1;
    bso1 = bs1;
    QuadRound(as0, as1, 0xe9b5dba5b5c0fbcfull, 0x71374491c28a2f98ull);
    QuadRound(bs0, bs1, 0xe9b5dba5b5c0fbcfull, 0x71374491c28a2f98ull);
    QuadRound(as0, as1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    QuadRound(bs0, bs1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    QuadRound(as0, as1, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    QuadRound(bs0, bs1, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    QuadRound(as0, as1, 0xc19bf3749bdc06a7ull, 0x80deb1fe72be5d74ull);
    QuadRound(bs0, bs1, 0xc19bf3749bdc06a7ull, 0x80deb1fe72be5d74ull);
    QuadRound(as0, as1, 0x240cf2540fe1edc6ull, 0xf0fe4786649b69c1ull);
    QuadRound(bs0, bs1, 0x240cf2540fe1edc6ull, 0xf0fe4786649b69c1ull);
    QuadRound(as0, as1, 0x16f988fa61b9411eull, 0x6cc984be4fe9346full);
    QuadRound(bs0, bs1, 0x16f988fa61b9411eull, 0x6cc984be4fe9346full);
    QuadRound(as0, as1, 0xb9d99ec7b019fc65ull, 0xa88e5a6df2c65152ull);
    QuadRound(bs0, bs1, 0xb9d99ec7b019fc65ull, 0xa88e5a6df2c65152ull);
    QuadRound(as0, as1, 0xc7353eb0fdb1232bull, 0xe70eeaa09a1231c3ull);
    QuadRound(bs0, bs1, 0xc7353eb0fdb1232bull, 0xe70eeaa09a1231c3ull);
    QuadRound(as0, as1, 0xdc1eeefd5a0f118full, 0xcb976d5f3069bad5ull);
    QuadRound(bs0, bs1, 0xdc1eeefd5a0f118full, 0xcb976d5f3069bad5ull);
    QuadRound(as0, as1, 0xe15d5b1658f4ca9dull, 0xde0b7a040a35b689ull);
    QuadRound(bs0, bs1, 0xe15d5b1658f4ca9dull, 0xde0b7a040a35b689ull);
    QuadRound(as0, as1, 0x6fab9537a507ea32ull, 0x37088980007f3e86ull);
    QuadRound(bs0, bs1, 0x6fab9537a507ea32ull, 0x37088980007f3e86ull);
    QuadRound(as0, as1, 0xc0bbbe37cdaa3b6dull, 0x0d8cd6f117406110ull);
    QuadRound(bs0, bs1, 0xc0bbbe37cdaa3b6dull, 0x0d8cd6f117406110ull);
    QuadRound(as0, as1, 0x6fd15ca70b02e931ull, 0xdb48a36383613bdaull);
    QuadRound(bs0, bs1, 0x6fd15ca70b02e931ull, 0xdb48a36383613bdaull);
    QuadRound(as0, as1, 0x6d4378906ed41a95ull, 0x31338431521afacaull);
    QuadRound(bs0, bs1, 0x6d4378906ed41a95ull, 0x31338431521afacaull);
    QuadRound(as0, as1, 0x532fb63cb5c9a0e6ull, 0x9eccabbdc39c91f2ull);
    QuadRound(bs0, bs1, 0x532fb63cb5c9a0e6ull, 0x9eccabbdc39c91f2ull);
    QuadRound(as0, as1, 0x4c191d76a4954b68ull, 0x07237ea3d2c741c6ull);
    QuadRound(bs0, bs1, 0x4c191d76a4954b68ull, 0x07237ea3d2c741c6ull);
    as0 = _mm_add_epi32(as0, aso0);
    bs0 = _mm_add_epi32(bs0, bso0);
    as1 = _mm_add_epi32(as1, aso1);
    bs1 = _mm_add_epi32(bs1, bso1);

    /* Extract hash */
    Unshuffle(as0, as1);
    Unshuffle(bs0, bs1);
    am0 = as0;
    bm0 = bs0;
    am1 = as1;
    bm1 = bs1;

    /* Transform 3 */
    bs0 = as0 = _mm_load_si128((const __m128i*)init0);
    bs1 = as1 = _mm_load_si128((const __m128i*)init1);
    QuadRound(as0, as1, am0, 0xe9b5dba5B5c0fbcfull, 0x71374491428a2f98ull);
    QuadRound(bs0, bs1, bm0, 0xe9b5dba5B5c0fbcfull, 0x71374491428a2f98ull);
    QuadRound(as0, as1, am1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    QuadRound(bs0, bs1, bm1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    ShiftMessageA(am0, am1);
    ShiftMessageA(bm0, bm1);
    bm2 = am2 = _mm_set_epi64x(0x0ull, 0x80000000ull);
    QuadRound(as0, as1, 0x550c7dc3243185beull, 0x12835b015807aa98ull);
    QuadRound(bs0, bs1, 0x550c7dc3243185beull, 0x12835b015807aa98ull);
    ShiftMessageA(am1, am2);
    ShiftMessageA(bm1, bm2);
    bm3 = am3 = _mm_set_epi64x(0x10000000000ull, 0x0ull);
    QuadRound(as0, as1, 0xc19bf2749bdc06a7ull, 0x80deb1fe72be5d74ull);
    QuadRound(bs0, bs1, 0xc19bf2749bdc06a7ull, 0x80deb1fe72be5d74ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x240ca1cc0fc19dc6ull, 0xefbe4786e49b69c1ull);
    QuadRound(bs0, bs1, bm0, 0x240ca1cc0fc19dc6ull, 0xefbe4786e49b69c1ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    QuadRound(bs0, bs1, bm1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    ShiftMessageB(am0, am1, am2);
    ShiftMessageB(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    QuadRound(bs0, bs1, bm2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    ShiftMessageB(am1, am2, am3);
    ShiftMessageB(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    QuadRound(bs0, bs1, bm3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    QuadRound(bs0, bs1, bm0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    QuadRound(bs0, bs1, bm1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    ShiftMessageB(am0, am1, am2);
    ShiftMessageB(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0xc76c51a3c24b8b70ull, 0xa81a664ba2bfe8A1ull);
    QuadRound(bs0, bs1, bm2, 0xc76c51a3c24b8b70ull, 0xa81a664ba2bfe8A1ull);
    ShiftMessageB(am1, am2, am3);
    ShiftMessageB(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    QuadRound(bs0, bs1, bm3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    ShiftMessageB(am2, am3, am0);
    ShiftMessageB(bm2, bm3, bm0);
    QuadRound(as0, as1, am0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    QuadRound(bs0, bs1, bm0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    ShiftMessageB(am3, am0, am1);
    ShiftMessageB(bm3, bm0, bm1);
    QuadRound(as0, as1, am1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    QuadRound(bs0, bs1, bm1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    ShiftMessageC(am0, am1, am2);
    ShiftMessageC(bm0, bm1, bm2);
    QuadRound(as0, as1, am2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    QuadRound(bs0, bs1, bm2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    ShiftMessageC(am1, am2, am3);
    ShiftMessageC(bm1, bm2, bm3);
    QuadRound(as0, as1, am3, 0xc67178f2bef9a3f7ull, 0xa4506ceb90befffaull);
    QuadRound(bs0, bs1, bm3, 0xc67178f2bef9a3f7ull, 0xa4506ceb90befffaull);
    as0 = _mm_add_epi32(as0, _mm_load_si128((const __m128i*)init0));
    bs0 = _mm_add_epi32(bs0, _mm_load_si128((const __m128i*)init0));
    as1 = _mm_add_epi32(as1, _mm_load_si128((const __m128i*)init1));
    bs1 = _mm_add_epi32(bs1, _mm_load_si128((const __m128i*)init1));

    /* Extract hash into out */
    Unshuffle(as0, as1);
    Unshuffle(bs0, bs1);
    Save(out, as0);
    Save(out + 16, as1);
    Save(out + 32, bs0);
    Save(out + 48, bs1);
}

BC_POP_WARNING()

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

#endif // WITH_SHANI
