// Based on:
// sha256-x86.c - Intel SHA extensions using C intrinsics
// Written and place in public domain by Jeffrey Walton
// Based on code from Intel, and by Sean Gulley for the miTLS project.

#include <bitcoin/system/crypto/sha256.hpp>

#include <iterator>
#include <stdint.h>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

#if !defined(HAVE_XCPU)

void hash_shani(state&, const block1&) NOEXCEPT
{
    BC_ASSERT_MSG(false, "hash_shani undefined");
}

void merkle_shani(digest1&, const block1&) NOEXCEPT
{
    BC_ASSERT_MSG(false, "merkle_shani[1] undefined");
}

void merkle_shani(digest2&, const block2&) NOEXCEPT
{
    BC_ASSERT_MSG(false, "merkle_shani[2] undefined");
}

#else

using namespace i128;

#ifndef VISUAL

alignas(mint128_t) constexpr uint8_t mask[sizeof(mint128_t)]
{
    0x03, 0x02, 0x01, 0x00, // 0x00010203ul
    0x07, 0x06, 0x05, 0x04, // 0x04050607ul
    0x0b, 0x0a, 0x09, 0x08, // 0x08090a0bul
    0x0f, 0x0e, 0x0d, 0x0c  // 0x0c0d0e0ful
};

alignas(mint128_t) constexpr uint8_t initial0[sizeof(mint128_t)]
{
    0x8c, 0x68, 0x05, 0x9b, // 0x9b05688cul
    0x7f, 0x52, 0x0e, 0x51, // 0x510e527ful
    0x85, 0xae, 0x67, 0xbb, // 0xbb67ae85ul
    0x67, 0xe6, 0x09, 0x6a  // 0x6a09e667ul
};

alignas(mint128_t) constexpr uint8_t initial1[sizeof(mint128_t)]
{
    0x19, 0xcd, 0xe0, 0x5b, // 0x5be0cd19ul
    0xab, 0xd9, 0x83, 0x1f, // 0x1f83d9abul
    0x3a, 0xf5, 0x4f, 0xa5, // 0xa54ff53aul
    0x72, 0xf3, 0x6e, 0x3c  // 0x3c6ef372ul
};

// load/store i128
// ----------------------------------------------------------------------------

mint128_t inline load08x16a(const uint8_t& bytes) NOEXCEPT
{
    // Aligned only, do not use with unaligned values.
    return _mm_load_si128(pointer_cast<const mint128_t>(&bytes));
}

mint128_t inline load08x16u(const uint32_t& bytes) NOEXCEPT
{
    return _mm_loadu_si128(pointer_cast<const mint128_t>(&bytes));
}

mint128_t inline load32x4u(const uint32_t& bytes) NOEXCEPT
{
    return _mm_loadu_si128(pointer_cast<const mint128_t>(&bytes));
}

void inline store8x16u(uint8_t& bytes, mint128_t value) NOEXCEPT
{
    _mm_storeu_si128(pointer_cast<mint128_t>(&bytes), value);
}

void inline store32x4u(uint32_t& bytes, mint128_t value) NOEXCEPT
{
    _mm_storeu_si128(pointer_cast<mint128_t>(&bytes), value);
}

mint128_t inline load(const uint8_t& data) NOEXCEPT
{
    static const auto flipper = load08x16a(mask[0]);
    return i128::shuffle(load08x16u(data), flipper);
}

void inline store(uint8_t& out, mint128_t value) NOEXCEPT
{
    static const auto flipper = load08x16a(mask[0]);
    store8x16u(out, i128::shuffle(value, flipper));
}

// sha256
// ----------------------------------------------------------------------------

void inline round(mint128_t& s0, mint128_t& s1, uint64_t k1,
    uint64_t k0) NOEXCEPT
{
    const auto value = set(k1, k0);
    s1 = _mm_sha256rnds2_epu32(s1, s0, value);
    s0 = _mm_sha256rnds2_epu32(s0, s1, i128::shuffle<0x0e>(value));
}

void inline round(mint128_t& s0, mint128_t& s1, mint128_t m, uint64_t k1,
    uint64_t k0) NOEXCEPT
{
    const auto value = sum(m, set(k1, k0));
    s1 = _mm_sha256rnds2_epu32(s1, s0, value);
    s0 = _mm_sha256rnds2_epu32(s0, s1, i128::shuffle<0x0e>(value));
}

void inline shift_message(mint128_t& out, mint128_t m) NOEXCEPT
{
    out = _mm_sha256msg1_epu32(out, m);
}

void inline shift_message(mint128_t m0, mint128_t m1, mint128_t& out) NOEXCEPT
{
    constexpr auto shift = sizeof(uint32_t);
    out = _mm_sha256msg2_epu32(sum(out, align_right<shift>(m1, m0)), m1);
}

void inline shift_messages(mint128_t& out0, mint128_t m,
    mint128_t& out1) NOEXCEPT
{
    shift_message(out0, m, out1);
    shift_message(out0, m);
}

// endianness
// ----------------------------------------------------------------------------

void inline shuffle(mint128_t& s0, mint128_t& s1) NOEXCEPT
{
    const auto t1 = i128::shuffle<0xb1>(s0);
    const auto t2 = i128::shuffle<0x1b>(s1);
    s0 = align_right<8>(t1, t2);
    s1 = blend<15>(t2, t1);
}

void inline unshuffle(mint128_t& s0, mint128_t& s1) NOEXCEPT
{
    const mint128_t t1 = i128::shuffle<0x1b>(s0);
    const mint128_t t2 = i128::shuffle<0xb1>(s1);
    s0 = blend<15>(t1, t2);
    s1 = align_right<8>(t2, t1);
}

#endif

////void hash_shani(state& state, const blocks& blocks) NOEXCEPT;
void hash_shani(state& state, const block1& blocks) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)

    mint128_t m0, m1, m2, m3, so0, so1;

    auto s0 = load32x4u(state[0]);
    auto s1 = load32x4u(state[4]);
    shuffle(s0, s1);

    // TODO: array_cast blocks into array of mint128_t.
    // TODO: this is currently precluded by an integral integer constraint.
    // TODO: this will eliminate pointer casts in load(block[n]).

    // One block in two lanes.
    for (auto& block: blocks)
    {
        // Remember old state.
        so0 = s0;
        so1 = s1;

        // load data and transform.
        m0 = load(block[0]);
        round(s0, s1, m0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
        m1 = load(block[16]);
        round(s0, s1, m1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
        shift_message(m0, m1);
        m2 = load(block[32]);
        round(s0, s1, m2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
        shift_message(m1, m2);
        m3 = load(block[48]);
        round(s0, s1, m3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
        shift_messages(m2, m3, m0);
        round(s0, s1, m0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
        shift_messages(m3, m0, m1);
        round(s0, s1, m1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
        shift_messages(m0, m1, m2);
        round(s0, s1, m2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
        shift_messages(m1, m2, m3);
        round(s0, s1, m3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
        shift_messages(m2, m3, m0);
        round(s0, s1, m0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
        shift_messages(m3, m0, m1);
        round(s0, s1, m1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
        shift_messages(m0, m1, m2);
        round(s0, s1, m2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
        shift_messages(m1, m2, m3);
        round(s0, s1, m3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
        shift_messages(m2, m3, m0);
        round(s0, s1, m0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
        shift_messages(m3, m0, m1);
        round(s0, s1, m1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
        shift_message(m0, m1, m2);
        round(s0, s1, m2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
        shift_message(m1, m2, m3);
        round(s0, s1, m3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);

        // Combine with old state.
        s0 = sum(s0, so0);
        s1 = sum(s1, so1);
    }

    unshuffle(s0, s1);
    store32x4u(state[0], s0);
    store32x4u(state[4], s1);

    BC_POP_WARNING()
}

void merkle_shani(digest2& out, const block2& block) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)

    static const auto init0 = load08x16a(initial0[0]);
    static const auto init1 = load08x16a(initial1[0]);

    // Transform 1.
    auto as0 = init0;
    auto as1 = init1;
    auto bs0 = init0;
    auto bs1 = init1;
    auto am0 = load(block[0][0]);
    auto bm0 = load(block[1][0]);
    round(as0, as1, am0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
    round(bs0, bs1, bm0, 0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull);
    auto am1 = load(block[0][16]);
    auto bm1 = load(block[1][16]);
    round(as0, as1, am1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    round(bs0, bs1, bm1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    shift_message(am0, am1);
    shift_message(bm0, bm1);
    auto am2 = load(block[0][32]);
    auto bm2 = load(block[1][32]);
    round(as0, as1, am2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    round(bs0, bs1, bm2, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    shift_message(am1, am2);
    shift_message(bm1, bm2);
    auto am3 = load(block[0][48]);
    auto bm3 = load(block[1][48]);
    round(as0, as1, am3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
    round(bs0, bs1, bm3, 0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
    round(bs0, bs1, bm0, 0x240ca1cc0fc19dc6ull, 0xefbe4786E49b69c1ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    round(bs0, bs1, bm1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    shift_messages(am0, am1, am2);
    shift_messages(bm0, bm1, bm2);
    round(as0, as1, am2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    round(bs0, bs1, bm2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    shift_messages(am1, am2, am3);
    shift_messages(bm1, bm2, bm3);
    round(as0, as1, am3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    round(bs0, bs1, bm3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    round(bs0, bs1, bm0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    round(bs0, bs1, bm1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    shift_messages(am0, am1, am2);
    shift_messages(bm0, bm1, bm2);
    round(as0, as1, am2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
    round(bs0, bs1, bm2, 0xc76c51A3c24b8b70ull, 0xa81a664ba2bfe8a1ull);
    shift_messages(am1, am2, am3);
    shift_messages(bm1, bm2, bm3);
    round(as0, as1, am3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    round(bs0, bs1, bm3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    round(bs0, bs1, bm0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    round(bs0, bs1, bm1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    shift_message(am0, am1, am2);
    shift_message(bm0, bm1, bm2);
    round(as0, as1, am2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    round(bs0, bs1, bm2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    shift_message(am1, am2, am3);
    shift_message(bm1, bm2, bm3);
    round(as0, as1, am3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);
    round(bs0, bs1, bm3, 0xc67178f2bef9A3f7ull, 0xa4506ceb90befffaull);
    as0 = sum(as0, init0);
    bs0 = sum(bs0, init0);
    as1 = sum(as1, init1);
    bs1 = sum(bs1, init1);

    // Transform 2.
    const auto aso0 = as0;
    const auto bso0 = bs0;
    const auto aso1 = as1;
    const auto bso1 = bs1;
    round(as0, as1, 0xe9b5dba5b5c0fbcfull, 0x71374491c28a2f98ull);
    round(bs0, bs1, 0xe9b5dba5b5c0fbcfull, 0x71374491c28a2f98ull);
    round(as0, as1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    round(bs0, bs1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    round(as0, as1, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    round(bs0, bs1, 0x550c7dc3243185beull, 0x12835b01d807aa98ull);
    round(as0, as1, 0xc19bf3749bdc06a7ull, 0x80deb1fe72be5d74ull);
    round(bs0, bs1, 0xc19bf3749bdc06a7ull, 0x80deb1fe72be5d74ull);
    round(as0, as1, 0x240cf2540fe1edc6ull, 0xf0fe4786649b69c1ull);
    round(bs0, bs1, 0x240cf2540fe1edc6ull, 0xf0fe4786649b69c1ull);
    round(as0, as1, 0x16f988fa61b9411eull, 0x6cc984be4fe9346full);
    round(bs0, bs1, 0x16f988fa61b9411eull, 0x6cc984be4fe9346full);
    round(as0, as1, 0xb9d99ec7b019fc65ull, 0xa88e5a6df2c65152ull);
    round(bs0, bs1, 0xb9d99ec7b019fc65ull, 0xa88e5a6df2c65152ull);
    round(as0, as1, 0xc7353eb0fdb1232bull, 0xe70eeaa09a1231c3ull);
    round(bs0, bs1, 0xc7353eb0fdb1232bull, 0xe70eeaa09a1231c3ull);
    round(as0, as1, 0xdc1eeefd5a0f118full, 0xcb976d5f3069bad5ull);
    round(bs0, bs1, 0xdc1eeefd5a0f118full, 0xcb976d5f3069bad5ull);
    round(as0, as1, 0xe15d5b1658f4ca9dull, 0xde0b7a040a35b689ull);
    round(bs0, bs1, 0xe15d5b1658f4ca9dull, 0xde0b7a040a35b689ull);
    round(as0, as1, 0x6fab9537a507ea32ull, 0x37088980007f3e86ull);
    round(bs0, bs1, 0x6fab9537a507ea32ull, 0x37088980007f3e86ull);
    round(as0, as1, 0xc0bbbe37cdaa3b6dull, 0x0d8cd6f117406110ull);
    round(bs0, bs1, 0xc0bbbe37cdaa3b6dull, 0x0d8cd6f117406110ull);
    round(as0, as1, 0x6fd15ca70b02e931ull, 0xdb48a36383613bdaull);
    round(bs0, bs1, 0x6fd15ca70b02e931ull, 0xdb48a36383613bdaull);
    round(as0, as1, 0x6d4378906ed41a95ull, 0x31338431521afacaull);
    round(bs0, bs1, 0x6d4378906ed41a95ull, 0x31338431521afacaull);
    round(as0, as1, 0x532fb63cb5c9a0e6ull, 0x9eccabbdc39c91f2ull);
    round(bs0, bs1, 0x532fb63cb5c9a0e6ull, 0x9eccabbdc39c91f2ull);
    round(as0, as1, 0x4c191d76a4954b68ull, 0x07237ea3d2c741c6ull);
    round(bs0, bs1, 0x4c191d76a4954b68ull, 0x07237ea3d2c741c6ull);
    as0 = sum(as0, aso0);
    bs0 = sum(bs0, bso0);
    as1 = sum(as1, aso1);
    bs1 = sum(bs1, bso1);

    // Extract hash.
    unshuffle(as0, as1);
    unshuffle(bs0, bs1);
    am0 = as0;
    bm0 = bs0;
    am1 = as1;
    bm1 = bs1;

    // Transform 3.
    bs0 = as0 = init0;
    bs1 = as1 = init1;
    round(as0, as1, am0, 0xe9b5dba5B5c0fbcfull, 0x71374491428a2f98ull);
    round(bs0, bs1, bm0, 0xe9b5dba5B5c0fbcfull, 0x71374491428a2f98ull);
    round(as0, as1, am1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    round(bs0, bs1, bm1, 0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull);
    shift_message(am0, am1);
    shift_message(bm0, bm1);
    bm2 = am2 = set(0x0ull, 0x80000000ull);
    round(as0, as1, 0x550c7dc3243185beull, 0x12835b015807aa98ull);
    round(bs0, bs1, 0x550c7dc3243185beull, 0x12835b015807aa98ull);
    shift_message(am1, am2);
    shift_message(bm1, bm2);
    bm3 = am3 = set(0x10000000000ull, 0x0ull);
    round(as0, as1, 0xc19bf2749bdc06a7ull, 0x80deb1fe72be5d74ull);
    round(bs0, bs1, 0xc19bf2749bdc06a7ull, 0x80deb1fe72be5d74ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x240ca1cc0fc19dc6ull, 0xefbe4786e49b69c1ull);
    round(bs0, bs1, bm0, 0x240ca1cc0fc19dc6ull, 0xefbe4786e49b69c1ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    round(bs0, bs1, bm1, 0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full);
    shift_messages(am0, am1, am2);
    shift_messages(bm0, bm1, bm2);
    round(as0, as1, am2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    round(bs0, bs1, bm2, 0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull);
    shift_messages(am1, am2, am3);
    shift_messages(bm1, bm2, bm3);
    round(as0, as1, am3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    round(bs0, bs1, bm3, 0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    round(bs0, bs1, bm0, 0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    round(bs0, bs1, bm1, 0x92722c8581c2c92eull, 0x766a0abb650a7354ull);
    shift_messages(am0, am1, am2);
    shift_messages(bm0, bm1, bm2);
    round(as0, as1, am2, 0xc76c51a3c24b8b70ull, 0xa81a664ba2bfe8A1ull);
    round(bs0, bs1, bm2, 0xc76c51a3c24b8b70ull, 0xa81a664ba2bfe8A1ull);
    shift_messages(am1, am2, am3);
    shift_messages(bm1, bm2, bm3);
    round(as0, as1, am3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    round(bs0, bs1, bm3, 0x106aa070f40e3585ull, 0xd6990624d192e819ull);
    shift_messages(am2, am3, am0);
    shift_messages(bm2, bm3, bm0);
    round(as0, as1, am0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    round(bs0, bs1, bm0, 0x34b0bcb52748774cull, 0x1e376c0819a4c116ull);
    shift_messages(am3, am0, am1);
    shift_messages(bm3, bm0, bm1);
    round(as0, as1, am1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    round(bs0, bs1, bm1, 0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull);
    shift_message(am0, am1, am2);
    shift_message(bm0, bm1, bm2);
    round(as0, as1, am2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    round(bs0, bs1, bm2, 0x8cc7020884c87814ull, 0x78a5636f748f82eeull);
    shift_message(am1, am2, am3);
    shift_message(bm1, bm2, bm3);
    round(as0, as1, am3, 0xc67178f2bef9a3f7ull, 0xa4506ceb90befffaull);
    round(bs0, bs1, bm3, 0xc67178f2bef9a3f7ull, 0xa4506ceb90befffaull);
    as0 = sum(as0, init0);
    bs0 = sum(bs0, init0);
    as1 = sum(as1, init1);
    bs1 = sum(bs1, init1);

    // Extract hash into out.
    unshuffle(as0, as1);
    unshuffle(bs0, bs1);
    store(out[0][ 0], as0);
    store(out[0][16], as1);
    store(out[1][ 0], bs0);
    store(out[1][16], bs1);

    BC_POP_WARNING()
}

void merkle_shani(digest1& out, const block1& blocks) NOEXCEPT
{
    auto state = sha256::initial;
    hash_shani(state, blocks);
    hash_shani(state, array_cast<block>(sha256::pad_64));
    auto buffer = sha256::pad_32;
    to_big_endians(narrowing_array_cast<uint32_t, state_size>(buffer), state);
    state = sha256::initial;
    hash_shani(state, array_cast<block>(buffer));
    to_big_endians(array_cast<uint32_t>(out.front()), state);
}

#endif // HAVE_XCPU

} // namespace sha256
} // namespace system
} // namespace libbitcoin
