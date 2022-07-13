// Based on:
/* libsodium: hash_sha256_.c, v0.4.5 2014/04/16 */
/**
 * Copyright 2005,2007,2009 Colin Percival. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <bitcoin/system/crypto/sha256.hpp>

#include <array>
#include <bit>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

// movable-type.co.uk/scripts/sha256.html

constexpr auto choice(auto x, auto y, auto z) noexcept
{
    return (x & (y ^ z)) ^ z;
}

constexpr auto majority(auto x, auto y, auto z) noexcept
{
    return (x & (y | z)) | (y & z);
}

// use inline vs. constexpr for intrinsic std::rotr.

inline auto SIGMA0(auto x) noexcept
{
    return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22);
}

inline auto SIGMA1(auto x) noexcept
{
    return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25);
}

inline auto sigma0(auto x) noexcept
{
    return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
}

inline auto sigma1(auto x) noexcept
{
    return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
}

inline void round(auto a, auto b, auto c, auto& out_d, auto e, auto f, auto g,
    auto& out_h, auto k) noexcept
{
    const auto t0 = SIGMA1(e) + choice(e, f, g) + out_h + k;
    const auto t1 = SIGMA0(a) + majority(a, b, c);
    out_d += t0;
    out_h = t0 + t1;
}

template<uint8_t Offset, uint32_t K>
constexpr void round(auto& state, const auto& buffer) noexcept
{
    static_assert(Offset < 64u);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    round(
        state[(64 - Offset) % 8],
        state[(65 - Offset) % 8],
        state[(66 - Offset) % 8],
        state[(67 - Offset) % 8], // in/out
        state[(68 - Offset) % 8],
        state[(69 - Offset) % 8],
        state[(70 - Offset) % 8],
        state[(71 - Offset) % 8], // in/out
        buffer[Offset] + K);
    BC_POP_WARNING()
}

template<uint8_t Offset>
inline void set(auto& buffer) noexcept
{
    static_assert(Offset < 64u);
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    buffer[Offset] =
        sigma1(buffer[Offset -  2]) + buffer[Offset -  7] +
        sigma0(buffer[Offset - 15]) + buffer[Offset - 16];
    BC_POP_WARNING()
}
    
////void single_hash(state& state, const blocks& blocks) NOEXCEPT;
static void single_hash(state& state, const block& block) NOEXCEPT
{
    constexpr auto integers = block_size / sizeof(uint32_t);
    std::array<uint32_t, block_size> buffer;
    const sha256::state copy{ state };

    // Set buffer[0..15] with all block big-endian integers (16 X 4 = 64 bytes).
    to_big_endian_set(
        narrowing_array_cast<uint32_t, integers>(buffer),
        array_cast<uint32_t>(block));

    // Set buffer[16..63].
    set<16>(buffer);
    set<17>(buffer);
    set<18>(buffer);
    set<19>(buffer);
    set<20>(buffer);
    set<21>(buffer);
    set<22>(buffer);
    set<23>(buffer);
    set<24>(buffer);
    set<25>(buffer);
    set<26>(buffer);
    set<27>(buffer);
    set<28>(buffer);
    set<29>(buffer);
    set<30>(buffer);
    set<31>(buffer);
    set<32>(buffer);
    set<33>(buffer);
    set<34>(buffer);
    set<35>(buffer);
    set<36>(buffer);
    set<37>(buffer);
    set<38>(buffer);
    set<39>(buffer);
    set<40>(buffer);
    set<41>(buffer);
    set<42>(buffer);
    set<43>(buffer);
    set<44>(buffer);
    set<45>(buffer);
    set<46>(buffer);
    set<47>(buffer);
    set<48>(buffer);
    set<49>(buffer);
    set<50>(buffer);
    set<51>(buffer);
    set<52>(buffer);
    set<53>(buffer);
    set<54>(buffer);
    set<55>(buffer);
    set<56>(buffer);
    set<57>(buffer);
    set<58>(buffer);
    set<59>(buffer);
    set<60>(buffer);
    set<61>(buffer);
    set<62>(buffer);
    set<63>(buffer);

    // 64 rounds.
    round< 0, 0x428a2f98>(state, buffer);
    round< 1, 0x71374491>(state, buffer);
    round< 2, 0xb5c0fbcf>(state, buffer);
    round< 3, 0xe9b5dba5>(state, buffer);
    round< 4, 0x3956c25b>(state, buffer);
    round< 5, 0x59f111f1>(state, buffer);
    round< 6, 0x923f82a4>(state, buffer);
    round< 7, 0xab1c5ed5>(state, buffer);
    round< 8, 0xd807aa98>(state, buffer);
    round< 9, 0x12835b01>(state, buffer);
    round<10, 0x243185be>(state, buffer);
    round<11, 0x550c7dc3>(state, buffer);
    round<12, 0x72be5d74>(state, buffer);
    round<13, 0x80deb1fe>(state, buffer);
    round<14, 0x9bdc06a7>(state, buffer);
    round<15, 0xc19bf174>(state, buffer);
    round<16, 0xe49b69c1>(state, buffer);
    round<17, 0xefbe4786>(state, buffer);
    round<18, 0x0fc19dc6>(state, buffer);
    round<19, 0x240ca1cc>(state, buffer);
    round<20, 0x2de92c6f>(state, buffer);
    round<21, 0x4a7484aa>(state, buffer);
    round<22, 0x5cb0a9dc>(state, buffer);
    round<23, 0x76f988da>(state, buffer);
    round<24, 0x983e5152>(state, buffer);
    round<25, 0xa831c66d>(state, buffer);
    round<26, 0xb00327c8>(state, buffer);
    round<27, 0xbf597fc7>(state, buffer);
    round<28, 0xc6e00bf3>(state, buffer);
    round<29, 0xd5a79147>(state, buffer);
    round<30, 0x06ca6351>(state, buffer);
    round<31, 0x14292967>(state, buffer);
    round<32, 0x27b70a85>(state, buffer);
    round<33, 0x2e1b2138>(state, buffer);
    round<34, 0x4d2c6dfc>(state, buffer);
    round<35, 0x53380d13>(state, buffer);
    round<36, 0x650a7354>(state, buffer);
    round<37, 0x766a0abb>(state, buffer);
    round<38, 0x81c2c92e>(state, buffer);
    round<39, 0x92722c85>(state, buffer);
    round<40, 0xa2bfe8a1>(state, buffer);
    round<41, 0xa81a664b>(state, buffer);
    round<42, 0xc24b8b70>(state, buffer);
    round<43, 0xc76c51a3>(state, buffer);
    round<44, 0xd192e819>(state, buffer);
    round<45, 0xd6990624>(state, buffer);
    round<46, 0xf40e3585>(state, buffer);
    round<47, 0x106aa070>(state, buffer);
    round<48, 0x19a4c116>(state, buffer);
    round<49, 0x1e376c08>(state, buffer);
    round<50, 0x2748774c>(state, buffer);
    round<51, 0x34b0bcb5>(state, buffer);
    round<52, 0x391c0cb3>(state, buffer);
    round<53, 0x4ed8aa4a>(state, buffer);
    round<54, 0x5b9cca4f>(state, buffer);
    round<55, 0x682e6ff3>(state, buffer);
    round<56, 0x748f82ee>(state, buffer);
    round<57, 0x78a5636f>(state, buffer);
    round<58, 0x84c87814>(state, buffer);
    round<59, 0x8cc70208>(state, buffer);
    round<60, 0x90befffa>(state, buffer);
    round<61, 0xa4506ceb>(state, buffer);
    round<62, 0xbef9a3f7>(state, buffer);
    round<63, 0xc67178f2>(state, buffer);
    
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    state[0] += copy[0];
    state[1] += copy[1];
    state[2] += copy[2];
    state[3] += copy[3];
    state[4] += copy[4];
    state[5] += copy[5];
    state[6] += copy[6];
    state[7] += copy[7];
    BC_POP_WARNING()
}

// ----------------------------------------------------------------------------

void single_hash(state& state, const block1& blocks) NOEXCEPT
{
    single_hash(state, blocks.front());
}

void double_hash(digest1& out, const block1& blocks) NOEXCEPT
{
    auto state = sha256::initial;
    single_hash(state, blocks);
    single_hash(state, sha256::pad_64);
    auto buffer = sha256::padded_32;
    to_big_endian_set(narrowing_array_cast<uint32_t, state_size>(buffer), state);
    state = sha256::initial;
    single_hash(state, buffer);
    to_big_endian_set(array_cast<uint32_t>(out.front()), state);
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin
