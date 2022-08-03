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

void merkle_avx2(digest8& out, const block8& blocks) NOEXCEPT
{
    BC_ASSERT_MSG(false, "merkle_avx2 undefined");
}

#else

namespace i256 {

using mint256_t = __m256i;

template <uint32_t Offset>
uint32_t get(mint256_t a) noexcept
{
    return _mm256_extract_epi32(a, Offset);
}

mint256_t set(uint32_t a) noexcept 
{
    return _mm256_set1_epi32(a);
}

mint256_t set(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
    uint32_t e, uint32_t f, uint32_t g, uint32_t h) noexcept
{
    return _mm256_set_epi32(a, b, c, d, e, f, g, h);
}

mint256_t shuffle(mint256_t a, mint256_t b) noexcept
{
    return _mm256_shuffle_epi8(a, b);
}

mint256_t sum(mint256_t a, mint256_t b) noexcept
{
    return _mm256_add_epi32(a, b);
}

mint256_t sum(mint256_t a, mint256_t b, mint256_t c) noexcept
{
    return sum(sum(a, b), c);
}

mint256_t sum(mint256_t a, mint256_t b, mint256_t c,
    mint256_t d) noexcept
{
    return sum(sum(a, b), sum(c, d));
}

mint256_t sum(mint256_t a, mint256_t b, mint256_t c, mint256_t d,
    mint256_t e) noexcept
{
    return sum(sum(a, b, c), sum(d, e));
}

mint256_t inc(mint256_t& outa, mint256_t b) noexcept
{
    return ((outa = sum(outa, b)));
}

mint256_t inc(mint256_t& outa, mint256_t b, mint256_t c) noexcept
{
    return ((outa = sum(outa, b, c)));
}

mint256_t inc(mint256_t& outa, mint256_t b, mint256_t c,
    mint256_t d) noexcept
{
    return ((outa = sum(outa, b, c, d)));
}

mint256_t exc(mint256_t a, mint256_t b) noexcept
{
    return _mm256_xor_si256(a, b);
}

mint256_t exc(mint256_t a, mint256_t b, mint256_t c) noexcept
{
    return exc(exc(a, b), c);
}

mint256_t dis(mint256_t a, mint256_t b) noexcept
{
    return _mm256_or_si256(a, b);
}

mint256_t con(mint256_t a, mint256_t b) noexcept
{
    return _mm256_and_si256(a, b);
}

mint256_t shr(mint256_t a, uint32_t bits) noexcept
{
    return _mm256_srli_epi32(a, bits);
}

mint256_t shl(mint256_t a, uint32_t bits) noexcept
{
    return _mm256_slli_epi32(a, bits);
}

} // namespace i256

using namespace i256;

mint256_t inline SIGMA0(mint256_t x) NOEXCEPT { return exc(dis(shr(x,  2), shl(x, 30)), dis(shr(x, 13), shl(x, 19)), dis(shr(x, 22), shl(x, 10))); }
mint256_t inline SIGMA1(mint256_t x) NOEXCEPT { return exc(dis(shr(x,  6), shl(x, 26)), dis(shr(x, 11), shl(x, 21)), dis(shr(x, 25), shl(x, 7))); }
mint256_t inline sigma0(mint256_t x) NOEXCEPT { return exc(dis(shr(x,  7), shl(x, 25)), dis(shr(x, 18), shl(x, 14)), shr(x, 3)); }
mint256_t inline sigma1(mint256_t x) NOEXCEPT { return exc(dis(shr(x, 17), shl(x, 15)), dis(shr(x, 19), shl(x, 13)), shr(x, 10)); }
mint256_t inline choice(  mint256_t x, mint256_t y, mint256_t z) NOEXCEPT { return exc(z, con(x, exc(y, z))); }
mint256_t inline majority(mint256_t x, mint256_t y, mint256_t z) NOEXCEPT { return dis(con(x, y), con(z, dis(x, y))); }

void inline round(mint256_t a, mint256_t b, mint256_t c, mint256_t& d,
    mint256_t e, mint256_t f, mint256_t g, mint256_t& h, mint256_t k) NOEXCEPT
{
    const auto t1 = sum(h, SIGMA1(e), choice(e, f, g), k);
    const auto t2 = sum(   SIGMA0(a), majority(a, b, c));
    d = sum(d,  t1);
    h = sum(t1, t2);
}

template <size_t Offset>
mint256_t inline read8(const block8& blocks) NOEXCEPT
{
    constexpr auto four = sizeof(uint32_t);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    const auto value = set(
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[0])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[1])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[2])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[3])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[4])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[5])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[6])),
        from_little_endian(array_cast<uint8_t, four, Offset>(blocks[7])));
    BC_POP_WARNING()

    return shuffle(value, set(
        0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul,  0x00010203ul,
        0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul));
}

template <size_t Offset>
void inline write8(digest8& hashes, mint256_t value) NOEXCEPT
{
    value = shuffle(value, set(
        0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul,
        0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul));

    constexpr auto four = sizeof(uint32_t);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    array_cast<uint8_t, four, Offset>(hashes[0]) = to_little_endian(get<7>(value));
    array_cast<uint8_t, four, Offset>(hashes[1]) = to_little_endian(get<6>(value));
    array_cast<uint8_t, four, Offset>(hashes[2]) = to_little_endian(get<5>(value));
    array_cast<uint8_t, four, Offset>(hashes[3]) = to_little_endian(get<4>(value));
    array_cast<uint8_t, four, Offset>(hashes[4]) = to_little_endian(get<3>(value));
    array_cast<uint8_t, four, Offset>(hashes[5]) = to_little_endian(get<2>(value));
    array_cast<uint8_t, four, Offset>(hashes[6]) = to_little_endian(get<1>(value));
    array_cast<uint8_t, four, Offset>(hashes[7]) = to_little_endian(get<0>(value));
    BC_POP_WARNING()
}

// Eight blocks in eight lanes, doubled.
void merkle_avx2(digest8& out, const block8& blocks) NOEXCEPT
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

    mint256_t w00, w01, w02, w03, w04, w05, w06, w07;
    mint256_t w08, w09, w10, w11, w12, w13, w14, w15;

    round(a, b, c, d, e, f, g, h, sum(set(0x428a2f98ul), w00 = read8< 0>(blocks)));
    round(h, a, b, c, d, e, f, g, sum(set(0x71374491ul), w01 = read8< 4>(blocks)));
    round(g, h, a, b, c, d, e, f, sum(set(0xb5c0fbcful), w02 = read8< 8>(blocks)));
    round(f, g, h, a, b, c, d, e, sum(set(0xe9b5dba5ul), w03 = read8<12>(blocks)));
    round(e, f, g, h, a, b, c, d, sum(set(0x3956c25bul), w04 = read8<16>(blocks)));
    round(d, e, f, g, h, a, b, c, sum(set(0x59f111f1ul), w05 = read8<20>(blocks)));
    round(c, d, e, f, g, h, a, b, sum(set(0x923f82a4ul), w06 = read8<24>(blocks)));
    round(b, c, d, e, f, g, h, a, sum(set(0xab1c5ed5ul), w07 = read8<28>(blocks)));
    round(a, b, c, d, e, f, g, h, sum(set(0xd807aa98ul), w08 = read8<32>(blocks)));
    round(h, a, b, c, d, e, f, g, sum(set(0x12835b01ul), w09 = read8<36>(blocks)));
    round(g, h, a, b, c, d, e, f, sum(set(0x243185beul), w10 = read8<40>(blocks)));
    round(f, g, h, a, b, c, d, e, sum(set(0x550c7dc3ul), w11 = read8<44>(blocks)));
    round(e, f, g, h, a, b, c, d, sum(set(0x72be5d74ul), w12 = read8<48>(blocks)));
    round(d, e, f, g, h, a, b, c, sum(set(0x80deb1feul), w13 = read8<52>(blocks)));
    round(c, d, e, f, g, h, a, b, sum(set(0x9bdc06a7ul), w14 = read8<56>(blocks)));
    round(b, c, d, e, f, g, h, a, sum(set(0xc19bf174ul), w15 = read8<60>(blocks)));
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

    const mint256_t t0 = a, t1 = b, t2 = c, t3 = d, t4 = e, t5 = f, t6 = g, t7 = h;

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
    write8< 0>(out, sum(a, set(0x6a09e667ul)));
    write8< 4>(out, sum(b, set(0xbb67ae85ul)));
    write8< 8>(out, sum(c, set(0x3c6ef372ul)));
    write8<12>(out, sum(d, set(0xa54ff53aul)));
    write8<16>(out, sum(e, set(0x510e527ful)));
    write8<20>(out, sum(f, set(0x9b05688cul)));
    write8<24>(out, sum(g, set(0x1f83d9abul)));
    write8<28>(out, sum(h, set(0x5be0cd19ul)));
}

#endif // HAVE_XCPU

#endif // DISABLED

} // namespace sha256
} // namespace system
} // namespace libbitcoin
