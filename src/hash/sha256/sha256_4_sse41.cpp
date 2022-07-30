// Based on:
// sha256-x86.c - Intel SHA extensions using C intrinsics
// Written and place in public domain by Jeffrey Walton
// Based on code from Intel, and by Sean Gulley for the miTLS project.

#include <stdint.h>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

#if defined (DISABLED)

#if !defined(HAVE_XCPU)

void merkle_sse41(digest4& out, const block4& blocks) NOEXCEPT
{
    BC_ASSERT_MSG(false, "merkle_sse41 undefined");
}

#else

namespace i128 {

using mint128_t = __m128i;

template <uint32_t Offset>
uint32_t get(mint128_t a) noexcept
{
    return _mm_extract_epi32(a, Offset);
}

mint128_t set(uint32_t a) noexcept
{
    return _mm_set1_epi32(a);
}

mint128_t set(uint64_t a, uint64_t b) noexcept
{
    return _mm_set_epi64x(a, b);
}

mint128_t set(uint32_t a, uint32_t b, uint32_t c, uint32_t d) noexcept
{
    return _mm_set_epi32(a, b, c, d);
}

mint128_t sum(mint128_t a, mint128_t b) noexcept
{
    return _mm_add_epi32(a, b);
}

mint128_t sum(mint128_t a, mint128_t b, mint128_t c) noexcept
{
    
    return sum(sum(a, b), c);
}

mint128_t sum(mint128_t a, mint128_t b, mint128_t c,
    mint128_t d) noexcept
{
    return sum(sum(a, b), sum(c, d));
}

mint128_t sum(mint128_t a, mint128_t b, mint128_t c, mint128_t d,
    mint128_t e) noexcept
{
    return sum(sum(a, b, c), sum(d, e));
}

mint128_t inc(mint128_t& outa, mint128_t b) noexcept
{
    return ((outa = sum(outa, b)));
}

mint128_t inc(mint128_t& outa, mint128_t b, mint128_t c) noexcept
{
    return ((outa = sum(outa, b, c)));
}

mint128_t inc(mint128_t& outa, mint128_t b, mint128_t c,
    mint128_t d) noexcept
{
    return ((outa = sum(outa, b, c, d)));
}

mint128_t exc(mint128_t a, mint128_t b) noexcept
{
    return _mm_xor_si128(a, b);
}

mint128_t exc(mint128_t a, mint128_t b, mint128_t c) noexcept
{
    return exc(exc(a, b), c);
}

mint128_t dis(mint128_t a, mint128_t b) noexcept
{
    return _mm_or_si128(a, b);
}

mint128_t con(mint128_t a, mint128_t b) noexcept
{
    return _mm_and_si128(a, b);
}

mint128_t shr(mint128_t a, uint32_t bits) noexcept
{
    return _mm_srli_epi32(a, bits);
}

mint128_t shl(mint128_t a, uint32_t bits) noexcept
{
    return _mm_slli_epi32(a, bits);
}

/// Concatenate two 16-byte blocks into a 32-byte temporary result, shift the 
/// result right by Shift bytes, and return the low 16 bytes.
template <uint32_t Shift>
mint128_t align_right(mint128_t a, mint128_t b) noexcept
{
    return _mm_alignr_epi8(a, b, Shift);
}

/// Blend two packed 16-bit integers using Mask.
template <uint32_t Mask>
mint128_t blend(mint128_t a, mint128_t b) noexcept
{
    return _mm_blend_epi16(a, b, Mask);
}

/// Shuffle 32-bit integers using Control.
template <uint32_t Control>
mint128_t shuffle(mint128_t a) noexcept
{
    return _mm_shuffle_epi32(a, Control);
}

/// Shuffle packed 8-bit integers in a according to shuffle control mask in the
/// corresponding 8-bit element of b.
mint128_t shuffle(mint128_t a, mint128_t b) noexcept
{
    return _mm_shuffle_epi8(a, b);
}

} // namespace i128

using namespace i128;

mint128_t inline SIGMA0(mint128_t x) NOEXCEPT { return exc(dis(shr(x,  2), shl(x, 30)), dis(shr(x, 13), shl(x, 19)), dis(shr(x, 22), shl(x, 10))); }
mint128_t inline SIGMA1(mint128_t x) NOEXCEPT { return exc(dis(shr(x,  6), shl(x, 26)), dis(shr(x, 11), shl(x, 21)), dis(shr(x, 25), shl(x, 7))); }
mint128_t inline sigma0(mint128_t x) NOEXCEPT { return exc(dis(shr(x,  7), shl(x, 25)), dis(shr(x, 18), shl(x, 14)), shr(x, 3)); }
mint128_t inline sigma1(mint128_t x) NOEXCEPT { return exc(dis(shr(x, 17), shl(x, 15)), dis(shr(x, 19), shl(x, 13)), shr(x, 10)); }
mint128_t inline choice(  mint128_t x, mint128_t y, mint128_t z) NOEXCEPT { return exc(z, con(x, exc(y, z))); }
mint128_t inline majority(mint128_t x, mint128_t y, mint128_t z) NOEXCEPT { return dis(con(x, y), con(z, dis(x, y))); }

void inline round(mint128_t a, mint128_t b, mint128_t c, mint128_t& d,
    mint128_t e, mint128_t f, mint128_t g, mint128_t& h, mint128_t k) NOEXCEPT
{
    const auto t1 = sum(h, SIGMA1(e), choice(e, f, g), k);
    const auto t2 = sum(   SIGMA0(a), majority(a, b, c));
    d = sum(d,  t1);
    h = sum(t1, t2);
}

template <size_t Offset>
mint128_t inline read4(const block4& blocks) NOEXCEPT
{
    constexpr auto four = sizeof(uint32_t);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    const auto value = set(
        from_little_endian(unsafe_array_cast<uint8_t, four>(&blocks[0][Offset])),
        from_little_endian(unsafe_array_cast<uint8_t, four>(&blocks[1][Offset])),
        from_little_endian(unsafe_array_cast<uint8_t, four>(&blocks[2][Offset])),
        from_little_endian(unsafe_array_cast<uint8_t, four>(&blocks[3][Offset])));
    BC_POP_WARNING()

    return shuffle(value, set(
        0x0c0d0e0ful, 0x08090a0bul,  0x04050607ul, 0x00010203ul));
}

template <size_t Offset>
void inline write4(digest4& hashes, mint128_t value) NOEXCEPT
{
    value = shuffle(value, set(
        0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul));

    constexpr auto four = sizeof(uint32_t);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    unsafe_array_cast<uint8_t, four>(&hashes[0][Offset]) = to_little_endian(get<3>(value));
    unsafe_array_cast<uint8_t, four>(&hashes[1][Offset]) = to_little_endian(get<2>(value));
    unsafe_array_cast<uint8_t, four>(&hashes[2][Offset]) = to_little_endian(get<1>(value));
    unsafe_array_cast<uint8_t, four>(&hashes[3][Offset]) = to_little_endian(get<0>(value));
    BC_POP_WARNING()
}

// Four blocks in four lanes, doubled.
void merkle_sse41(digest4& out, const block4& blocks) NOEXCEPT
{
    // Transform 1.
    auto a = set(0x6a09e667ul);
    auto b = set(0xbb67ae85ul);
    auto c = set(0x3c6ef372ul);
    auto d = set(0xa54ff53aul);
    auto e = set(0x510e527ful);
    auto f = set(0x9b05688cul);
    auto g = set(0x1f83d9abul);
    auto h = set(0x5be0cd19ul);

    mint128_t w00, w01, w02, w03, w04, w05, w06, w07;
    mint128_t w08, w09, w10, w11, w12, w13, w14, w15;

    round(a, b, c, d, e, f, g, h, sum(set(0x428a2f98ul), w00 = read4< 0>(blocks)));
    round(h, a, b, c, d, e, f, g, sum(set(0x71374491ul), w01 = read4< 4>(blocks)));
    round(g, h, a, b, c, d, e, f, sum(set(0xb5c0fbcful), w02 = read4< 8>(blocks)));
    round(f, g, h, a, b, c, d, e, sum(set(0xe9b5dba5ul), w03 = read4<12>(blocks)));
    round(e, f, g, h, a, b, c, d, sum(set(0x3956c25bul), w04 = read4<16>(blocks)));
    round(d, e, f, g, h, a, b, c, sum(set(0x59f111f1ul), w05 = read4<20>(blocks)));
    round(c, d, e, f, g, h, a, b, sum(set(0x923f82a4ul), w06 = read4<24>(blocks)));
    round(b, c, d, e, f, g, h, a, sum(set(0xab1c5ed5ul), w07 = read4<28>(blocks)));
    round(a, b, c, d, e, f, g, h, sum(set(0xd807aa98ul), w08 = read4<32>(blocks)));
    round(h, a, b, c, d, e, f, g, sum(set(0x12835b01ul), w09 = read4<36>(blocks)));
    round(g, h, a, b, c, d, e, f, sum(set(0x243185beul), w10 = read4<40>(blocks)));
    round(f, g, h, a, b, c, d, e, sum(set(0x550c7dc3ul), w11 = read4<44>(blocks)));
    round(e, f, g, h, a, b, c, d, sum(set(0x72be5d74ul), w12 = read4<48>(blocks)));
    round(d, e, f, g, h, a, b, c, sum(set(0x80deb1feul), w13 = read4<52>(blocks)));
    round(c, d, e, f, g, h, a, b, sum(set(0x9bdc06a7ul), w14 = read4<56>(blocks)));
    round(b, c, d, e, f, g, h, a, sum(set(0xc19bf174ul), w15 = read4<60>(blocks)));
    round(a, b, c, d, e, f, g, h, sum(set(0xe49b69c1ul), inc(w00, sigma1(w14), w09, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0xefbe4786ul), inc(w01, sigma1(w15), w10, sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x0fc19dc6ul), inc(w02, sigma1(w00), w11, sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x240ca1ccul), inc(w03, sigma1(w01), w12, sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x2de92c6ful), inc(w04, sigma1(w02), w13, sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x4a7484aaul), inc(w05, sigma1(w03), w14, sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x5cb0a9dcul), inc(w06, sigma1(w04), w15, sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x76f988daul), inc(w07, sigma1(w05), w00, sigma0(w08))));
    round(a, b, c, d, e, f, g, h, sum(set(0x983e5152ul), inc(w08, sigma1(w06), w01, sigma0(w09))));
    round(h, a, b, c, d, e, f, g, sum(set(0xa831c66dul), inc(w09, sigma1(w07), w02, sigma0(w10))));
    round(g, h, a, b, c, d, e, f, sum(set(0xb00327c8ul), inc(w10, sigma1(w08), w03, sigma0(w11))));
    round(f, g, h, a, b, c, d, e, sum(set(0xbf597fc7ul), inc(w11, sigma1(w09), w04, sigma0(w12))));
    round(e, f, g, h, a, b, c, d, sum(set(0xc6e00bf3ul), inc(w12, sigma1(w10), w05, sigma0(w13))));
    round(d, e, f, g, h, a, b, c, sum(set(0xd5a79147ul), inc(w13, sigma1(w11), w06, sigma0(w14))));
    round(c, d, e, f, g, h, a, b, sum(set(0x06ca6351ul), inc(w14, sigma1(w12), w07, sigma0(w15))));
    round(b, c, d, e, f, g, h, a, sum(set(0x14292967ul), inc(w15, sigma1(w13), w08, sigma0(w00))));
    round(a, b, c, d, e, f, g, h, sum(set(0x27b70a85ul), inc(w00, sigma1(w14), w09, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0x2e1b2138ul), inc(w01, sigma1(w15), w10, sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x4d2c6dfcul), inc(w02, sigma1(w00), w11, sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x53380d13ul), inc(w03, sigma1(w01), w12, sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x650a7354ul), inc(w04, sigma1(w02), w13, sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x766a0abbul), inc(w05, sigma1(w03), w14, sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x81c2c92eul), inc(w06, sigma1(w04), w15, sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x92722c85ul), inc(w07, sigma1(w05), w00, sigma0(w08))));
    round(a, b, c, d, e, f, g, h, sum(set(0xa2bfe8a1ul), inc(w08, sigma1(w06), w01, sigma0(w09))));
    round(h, a, b, c, d, e, f, g, sum(set(0xa81a664bul), inc(w09, sigma1(w07), w02, sigma0(w10))));
    round(g, h, a, b, c, d, e, f, sum(set(0xc24b8b70ul), inc(w10, sigma1(w08), w03, sigma0(w11))));
    round(f, g, h, a, b, c, d, e, sum(set(0xc76c51a3ul), inc(w11, sigma1(w09), w04, sigma0(w12))));
    round(e, f, g, h, a, b, c, d, sum(set(0xd192e819ul), inc(w12, sigma1(w10), w05, sigma0(w13))));
    round(d, e, f, g, h, a, b, c, sum(set(0xd6990624ul), inc(w13, sigma1(w11), w06, sigma0(w14))));
    round(c, d, e, f, g, h, a, b, sum(set(0xf40e3585ul), inc(w14, sigma1(w12), w07, sigma0(w15))));
    round(b, c, d, e, f, g, h, a, sum(set(0x106aa070ul), inc(w15, sigma1(w13), w08, sigma0(w00))));
    round(a, b, c, d, e, f, g, h, sum(set(0x19a4c116ul), inc(w00, sigma1(w14), w09, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0x1e376c08ul), inc(w01, sigma1(w15), w10, sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x2748774cul), inc(w02, sigma1(w00), w11, sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x34b0bcb5ul), inc(w03, sigma1(w01), w12, sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x391c0cb3ul), inc(w04, sigma1(w02), w13, sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x4ed8aa4aul), inc(w05, sigma1(w03), w14, sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x5b9cca4ful), inc(w06, sigma1(w04), w15, sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x682e6ff3ul), inc(w07, sigma1(w05), w00, sigma0(w08))));
    round(a, b, c, d, e, f, g, h, sum(set(0x748f82eeul), inc(w08, sigma1(w06), w01, sigma0(w09))));
    round(h, a, b, c, d, e, f, g, sum(set(0x78a5636ful), inc(w09, sigma1(w07), w02, sigma0(w10))));
    round(g, h, a, b, c, d, e, f, sum(set(0x84c87814ul), inc(w10, sigma1(w08), w03, sigma0(w11))));
    round(f, g, h, a, b, c, d, e, sum(set(0x8cc70208ul), inc(w11, sigma1(w09), w04, sigma0(w12))));
    round(e, f, g, h, a, b, c, d, sum(set(0x90befffaul), inc(w12, sigma1(w10), w05, sigma0(w13))));
    round(d, e, f, g, h, a, b, c, sum(set(0xa4506cebul), inc(w13, sigma1(w11), w06, sigma0(w14))));
    round(c, d, e, f, g, h, a, b, sum(set(0xbef9a3f7ul), inc(w14, sigma1(w12), w07, sigma0(w15))));
    round(b, c, d, e, f, g, h, a, sum(set(0xc67178f2ul), inc(w15, sigma1(w13), w08, sigma0(w00))));

    a = sum(a, set(0x6a09e667ul));
    b = sum(b, set(0xbb67ae85ul));
    c = sum(c, set(0x3c6ef372ul));
    d = sum(d, set(0xa54ff53aul));
    e = sum(e, set(0x510e527ful));
    f = sum(f, set(0x9b05688cul));
    g = sum(g, set(0x1f83d9abul));
    h = sum(h, set(0x5be0cd19ul));

    const mint128_t t0 = a, t1 = b, t2 = c, t3 = d, t4 = e, t5 = f, t6 = g, t7 = h;

    // Transform 2.
    round(a, b, c, d, e, f, g, h, set(0xc28a2f98ul));
    round(h, a, b, c, d, e, f, g, set(0x71374491ul));
    round(g, h, a, b, c, d, e, f, set(0xb5c0fbcful));
    round(f, g, h, a, b, c, d, e, set(0xe9b5dba5ul));
    round(e, f, g, h, a, b, c, d, set(0x3956c25bul));
    round(d, e, f, g, h, a, b, c, set(0x59f111f1ul));
    round(c, d, e, f, g, h, a, b, set(0x923f82a4ul));
    round(b, c, d, e, f, g, h, a, set(0xab1c5ed5ul));
    round(a, b, c, d, e, f, g, h, set(0xd807aa98ul));
    round(h, a, b, c, d, e, f, g, set(0x12835b01ul));
    round(g, h, a, b, c, d, e, f, set(0x243185beul));
    round(f, g, h, a, b, c, d, e, set(0x550c7dc3ul));
    round(e, f, g, h, a, b, c, d, set(0x72be5d74ul));
    round(d, e, f, g, h, a, b, c, set(0x80deb1feul));
    round(c, d, e, f, g, h, a, b, set(0x9bdc06a7ul));
    round(b, c, d, e, f, g, h, a, set(0xc19bf374ul));
    round(a, b, c, d, e, f, g, h, set(0x649b69c1ul));
    round(h, a, b, c, d, e, f, g, set(0xf0fe4786ul));
    round(g, h, a, b, c, d, e, f, set(0x0fe1edc6ul));
    round(f, g, h, a, b, c, d, e, set(0x240cf254ul));
    round(e, f, g, h, a, b, c, d, set(0x4fe9346ful));
    round(d, e, f, g, h, a, b, c, set(0x6cc984beul));
    round(c, d, e, f, g, h, a, b, set(0x61b9411eul));
    round(b, c, d, e, f, g, h, a, set(0x16f988faul));
    round(a, b, c, d, e, f, g, h, set(0xf2c65152ul));
    round(h, a, b, c, d, e, f, g, set(0xa88e5a6dul));
    round(g, h, a, b, c, d, e, f, set(0xb019fc65ul));
    round(f, g, h, a, b, c, d, e, set(0xb9d99ec7ul));
    round(e, f, g, h, a, b, c, d, set(0x9a1231c3ul));
    round(d, e, f, g, h, a, b, c, set(0xe70eeaa0ul));
    round(c, d, e, f, g, h, a, b, set(0xfdb1232bul));
    round(b, c, d, e, f, g, h, a, set(0xc7353eb0ul));
    round(a, b, c, d, e, f, g, h, set(0x3069bad5ul));
    round(h, a, b, c, d, e, f, g, set(0xcb976d5ful));
    round(g, h, a, b, c, d, e, f, set(0x5a0f118ful));
    round(f, g, h, a, b, c, d, e, set(0xdc1eeefdul));
    round(e, f, g, h, a, b, c, d, set(0x0a35b689ul));
    round(d, e, f, g, h, a, b, c, set(0xde0b7a04ul));
    round(c, d, e, f, g, h, a, b, set(0x58f4ca9dul));
    round(b, c, d, e, f, g, h, a, set(0xe15d5b16ul));
    round(a, b, c, d, e, f, g, h, set(0x007f3e86ul));
    round(h, a, b, c, d, e, f, g, set(0x37088980ul));
    round(g, h, a, b, c, d, e, f, set(0xa507ea32ul));
    round(f, g, h, a, b, c, d, e, set(0x6fab9537ul));
    round(e, f, g, h, a, b, c, d, set(0x17406110ul));
    round(d, e, f, g, h, a, b, c, set(0x0d8cd6f1ul));
    round(c, d, e, f, g, h, a, b, set(0xcdaa3b6dul));
    round(b, c, d, e, f, g, h, a, set(0xc0bbbe37ul));
    round(a, b, c, d, e, f, g, h, set(0x83613bdaul));
    round(h, a, b, c, d, e, f, g, set(0xdb48a363ul));
    round(g, h, a, b, c, d, e, f, set(0x0b02e931ul));
    round(f, g, h, a, b, c, d, e, set(0x6fd15ca7ul));
    round(e, f, g, h, a, b, c, d, set(0x521afacaul));
    round(d, e, f, g, h, a, b, c, set(0x31338431ul));
    round(c, d, e, f, g, h, a, b, set(0x6ed41a95ul));
    round(b, c, d, e, f, g, h, a, set(0x6d437890ul));
    round(a, b, c, d, e, f, g, h, set(0xc39c91f2ul));
    round(h, a, b, c, d, e, f, g, set(0x9eccabbdul));
    round(g, h, a, b, c, d, e, f, set(0xb5c9a0e6ul));
    round(f, g, h, a, b, c, d, e, set(0x532fb63cul));
    round(e, f, g, h, a, b, c, d, set(0xd2c741c6ul));
    round(d, e, f, g, h, a, b, c, set(0x07237ea3ul));
    round(c, d, e, f, g, h, a, b, set(0xa4954b68ul));
    round(b, c, d, e, f, g, h, a, set(0x4c191d76ul));

    w00 = sum(t0, a);
    w01 = sum(t1, b);
    w02 = sum(t2, c);
    w03 = sum(t3, d);
    w04 = sum(t4, e);
    w05 = sum(t5, f);
    w06 = sum(t6, g);
    w07 = sum(t7, h);

    // Transform 3.
    a = set(0x6a09e667ul);
    b = set(0xbb67ae85ul);
    c = set(0x3c6ef372ul);
    d = set(0xa54ff53aul);
    e = set(0x510e527ful);
    f = set(0x9b05688cul);
    g = set(0x1f83d9abul);
    h = set(0x5be0cd19ul);

    round(a, b, c, d, e, f, g, h, sum(set(0x428a2f98ul), w00));
    round(h, a, b, c, d, e, f, g, sum(set(0x71374491ul), w01));
    round(g, h, a, b, c, d, e, f, sum(set(0xb5c0fbcful), w02));
    round(f, g, h, a, b, c, d, e, sum(set(0xe9b5dba5ul), w03));
    round(e, f, g, h, a, b, c, d, sum(set(0x3956c25bul), w04));
    round(d, e, f, g, h, a, b, c, sum(set(0x59f111f1ul), w05));
    round(c, d, e, f, g, h, a, b, sum(set(0x923f82a4ul), w06));
    round(b, c, d, e, f, g, h, a, sum(set(0xab1c5ed5ul), w07));
    round(a, b, c, d, e, f, g, h, set(0x5807aa98ul));
    round(h, a, b, c, d, e, f, g, set(0x12835b01ul));
    round(g, h, a, b, c, d, e, f, set(0x243185beul));
    round(f, g, h, a, b, c, d, e, set(0x550c7dc3ul));
    round(e, f, g, h, a, b, c, d, set(0x72be5d74ul));
    round(d, e, f, g, h, a, b, c, set(0x80deb1feul));
    round(c, d, e, f, g, h, a, b, set(0x9bdc06a7ul));
    round(b, c, d, e, f, g, h, a, set(0xc19bf274ul));
    round(a, b, c, d, e, f, g, h, sum(set(0xe49b69c1ul), inc(w00, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0xefbe4786ul), inc(w01, set(0xa00000ul), sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x0fc19dc6ul), inc(w02, sigma1(w00), sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x240ca1ccul), inc(w03, sigma1(w01), sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x2de92c6ful), inc(w04, sigma1(w02), sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x4a7484aaul), inc(w05, sigma1(w03), sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x5cb0a9dcul), inc(w06, sigma1(w04), set(0x100ul), sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x76f988daul), inc(w07, sigma1(w05), w00, set(0x11002000ul))));
    round(a, b, c, d, e, f, g, h, sum(set(0x983e5152ul), w08 = sum(set(0x80000000ul), sigma1(w06), w01)));
    round(h, a, b, c, d, e, f, g, sum(set(0xa831c66dul), w09 = sum(sigma1(w07), w02)));
    round(g, h, a, b, c, d, e, f, sum(set(0xb00327c8ul), w10 = sum(sigma1(w08), w03)));
    round(f, g, h, a, b, c, d, e, sum(set(0xbf597fc7ul), w11 = sum(sigma1(w09), w04)));
    round(e, f, g, h, a, b, c, d, sum(set(0xc6e00bf3ul), w12 = sum(sigma1(w10), w05)));
    round(d, e, f, g, h, a, b, c, sum(set(0xd5a79147ul), w13 = sum(sigma1(w11), w06)));
    round(c, d, e, f, g, h, a, b, sum(set(0x06ca6351ul), w14 = sum(sigma1(w12), w07, set(0x400022ul))));
    round(b, c, d, e, f, g, h, a, sum(set(0x14292967ul), w15 = sum(set(0x100ul), sigma1(w13), w08, sigma0(w00))));
    round(a, b, c, d, e, f, g, h, sum(set(0x27b70a85ul), inc(w00, sigma1(w14), w09, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0x2e1b2138ul), inc(w01, sigma1(w15), w10, sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x4d2c6dfcul), inc(w02, sigma1(w00), w11, sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x53380d13ul), inc(w03, sigma1(w01), w12, sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x650a7354ul), inc(w04, sigma1(w02), w13, sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x766a0abbul), inc(w05, sigma1(w03), w14, sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x81c2c92eul), inc(w06, sigma1(w04), w15, sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x92722c85ul), inc(w07, sigma1(w05), w00, sigma0(w08))));
    round(a, b, c, d, e, f, g, h, sum(set(0xa2bfe8a1ul), inc(w08, sigma1(w06), w01, sigma0(w09))));
    round(h, a, b, c, d, e, f, g, sum(set(0xa81a664bul), inc(w09, sigma1(w07), w02, sigma0(w10))));
    round(g, h, a, b, c, d, e, f, sum(set(0xc24b8b70ul), inc(w10, sigma1(w08), w03, sigma0(w11))));
    round(f, g, h, a, b, c, d, e, sum(set(0xc76c51a3ul), inc(w11, sigma1(w09), w04, sigma0(w12))));
    round(e, f, g, h, a, b, c, d, sum(set(0xd192e819ul), inc(w12, sigma1(w10), w05, sigma0(w13))));
    round(d, e, f, g, h, a, b, c, sum(set(0xd6990624ul), inc(w13, sigma1(w11), w06, sigma0(w14))));
    round(c, d, e, f, g, h, a, b, sum(set(0xf40e3585ul), inc(w14, sigma1(w12), w07, sigma0(w15))));
    round(b, c, d, e, f, g, h, a, sum(set(0x106aa070ul), inc(w15, sigma1(w13), w08, sigma0(w00))));
    round(a, b, c, d, e, f, g, h, sum(set(0x19a4c116ul), inc(w00, sigma1(w14), w09, sigma0(w01))));
    round(h, a, b, c, d, e, f, g, sum(set(0x1e376c08ul), inc(w01, sigma1(w15), w10, sigma0(w02))));
    round(g, h, a, b, c, d, e, f, sum(set(0x2748774cul), inc(w02, sigma1(w00), w11, sigma0(w03))));
    round(f, g, h, a, b, c, d, e, sum(set(0x34b0bcb5ul), inc(w03, sigma1(w01), w12, sigma0(w04))));
    round(e, f, g, h, a, b, c, d, sum(set(0x391c0cb3ul), inc(w04, sigma1(w02), w13, sigma0(w05))));
    round(d, e, f, g, h, a, b, c, sum(set(0x4ed8aa4aul), inc(w05, sigma1(w03), w14, sigma0(w06))));
    round(c, d, e, f, g, h, a, b, sum(set(0x5b9cca4ful), inc(w06, sigma1(w04), w15, sigma0(w07))));
    round(b, c, d, e, f, g, h, a, sum(set(0x682e6ff3ul), inc(w07, sigma1(w05), w00, sigma0(w08))));
    round(a, b, c, d, e, f, g, h, sum(set(0x748f82eeul), inc(w08, sigma1(w06), w01, sigma0(w09))));
    round(h, a, b, c, d, e, f, g, sum(set(0x78a5636ful), inc(w09, sigma1(w07), w02, sigma0(w10))));
    round(g, h, a, b, c, d, e, f, sum(set(0x84c87814ul), inc(w10, sigma1(w08), w03, sigma0(w11))));
    round(f, g, h, a, b, c, d, e, sum(set(0x8cc70208ul), inc(w11, sigma1(w09), w04, sigma0(w12))));
    round(e, f, g, h, a, b, c, d, sum(set(0x90befffaul), inc(w12, sigma1(w10), w05, sigma0(w13))));
    round(d, e, f, g, h, a, b, c, sum(set(0xa4506cebul), inc(w13, sigma1(w11), w06, sigma0(w14))));
    round(c, d, e, f, g, h, a, b, sum(set(0xbef9a3f7ul),     w14, sigma1(w12), w07, sigma0(w15)));
    round(b, c, d, e, f, g, h, a, sum(set(0xc67178f2ul),     w15, sigma1(w13), w08, sigma0(w00)));

    // Output.
    write4< 0>(out, sum(a, set(0x6a09e667ul)));
    write4< 4>(out, sum(b, set(0xbb67ae85ul)));
    write4< 8>(out, sum(c, set(0x3c6ef372ul)));
    write4<12>(out, sum(d, set(0xa54ff53aul)));
    write4<16>(out, sum(e, set(0x510e527ful)));
    write4<20>(out, sum(f, set(0x9b05688cul)));
    write4<24>(out, sum(g, set(0x1f83d9abul)));
    write4<28>(out, sum(h, set(0x5be0cd19ul)));
}

#endif // HAVE_XCPU

#endif // DISABLED

} // namespace sha256
} // namespace system
} // namespace libbitcoin
