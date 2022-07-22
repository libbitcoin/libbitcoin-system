/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/crypto/sha256.hpp>

#include <algorithm>
#include <bit>
#include <iterator>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// sha256: movable-type.co.uk/scripts/sha256.html
// Use inline vs. constexpr to obtain intrinsic std::rotr.

namespace libbitcoin {
namespace system {
namespace sha256 {

constexpr auto choice(auto x, auto y, auto z) NOEXCEPT
{
    return (x & (y ^ z)) ^ z;
}

constexpr auto majority(auto x, auto y, auto z) NOEXCEPT
{
    return (x & (y | z)) | (y & z);
}

inline auto SIGMA0(auto a) NOEXCEPT
{
    return std::rotr(a, 2) ^ std::rotr(a, 13) ^ std::rotr(a, 22);
}

inline auto SIGMA1(auto a) NOEXCEPT
{
    return std::rotr(a, 6) ^ std::rotr(a, 11) ^ std::rotr(a, 25);
}

inline auto sigma0(auto a) NOEXCEPT
{
    return std::rotr(a, 7) ^ std::rotr(a, 18) ^ (a >> 3);
}

inline auto sigma1(auto a) NOEXCEPT
{
    return std::rotr(a, 17) ^ std::rotr(a, 19) ^ (a >> 10);
}

inline void round(auto a, auto b, auto c, auto& out_d, auto e, auto f, auto g,
    auto& out_h, auto k) NOEXCEPT
{
    const auto t0 = SIGMA1(e) + choice(e, f, g) + out_h + k;
    const auto t1 = SIGMA0(a) + majority(a, b, c);
    out_d += t0;
    out_h = t0 + t1;
}

template<uint8_t Round, uint32_t K>
constexpr void round(state& out, const buffer& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    round(
        out[(block_size + 0 - Round) % state_size],
        out[(block_size + 1 - Round) % state_size],
        out[(block_size + 2 - Round) % state_size],
        out[(block_size + 3 - Round) % state_size], // in/out
        out[(block_size + 4 - Round) % state_size],
        out[(block_size + 5 - Round) % state_size],
        out[(block_size + 6 - Round) % state_size],
        out[(block_size + 7 - Round) % state_size], // in/out
        in[Round] + K);
    BC_POP_WARNING()
}

template<uint8_t Offset>
inline void set(buffer& out) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[Offset] =
        sigma1(out[Offset -  2]) + out[Offset -  7] +
        sigma0(out[Offset - 15]) + out[Offset - 16];
    BC_POP_WARNING()
}

inline void expand48(buffer& out) NOEXCEPT
{
    set<16>(out);
    set<17>(out);
    set<18>(out);
    set<19>(out);
    set<20>(out);
    set<21>(out);
    set<22>(out);
    set<23>(out);
    set<24>(out);
    set<25>(out);
    set<26>(out);
    set<27>(out);
    set<28>(out);
    set<29>(out);
    set<30>(out);
    set<31>(out);
    set<32>(out);
    set<33>(out);
    set<34>(out);
    set<35>(out);
    set<36>(out);
    set<37>(out);
    set<38>(out);
    set<39>(out);
    set<40>(out);
    set<41>(out);
    set<42>(out);
    set<43>(out);
    set<44>(out);
    set<45>(out);
    set<46>(out);
    set<47>(out);
    set<48>(out);
    set<49>(out);
    set<50>(out);
    set<51>(out);
    set<52>(out);
    set<53>(out);
    set<54>(out);
    set<55>(out);
    set<56>(out);
    set<57>(out);
    set<58>(out);
    set<59>(out);
    set<60>(out);
    set<61>(out);
    set<62>(out);
    set<63>(out);
}

inline void rounds64(state& out, const buffer& in) NOEXCEPT
{
    round< 0, 0x428a2f98>(out, in);
    round< 1, 0x71374491>(out, in);
    round< 2, 0xb5c0fbcf>(out, in);
    round< 3, 0xe9b5dba5>(out, in);
    round< 4, 0x3956c25b>(out, in);
    round< 5, 0x59f111f1>(out, in);
    round< 6, 0x923f82a4>(out, in);
    round< 7, 0xab1c5ed5>(out, in);
    round< 8, 0xd807aa98>(out, in);
    round< 9, 0x12835b01>(out, in);
    round<10, 0x243185be>(out, in);
    round<11, 0x550c7dc3>(out, in);
    round<12, 0x72be5d74>(out, in);
    round<13, 0x80deb1fe>(out, in);
    round<14, 0x9bdc06a7>(out, in);
    round<15, 0xc19bf174>(out, in);
    round<16, 0xe49b69c1>(out, in);
    round<17, 0xefbe4786>(out, in);
    round<18, 0x0fc19dc6>(out, in);
    round<19, 0x240ca1cc>(out, in);
    round<20, 0x2de92c6f>(out, in);
    round<21, 0x4a7484aa>(out, in);
    round<22, 0x5cb0a9dc>(out, in);
    round<23, 0x76f988da>(out, in);
    round<24, 0x983e5152>(out, in);
    round<25, 0xa831c66d>(out, in);
    round<26, 0xb00327c8>(out, in);
    round<27, 0xbf597fc7>(out, in);
    round<28, 0xc6e00bf3>(out, in);
    round<29, 0xd5a79147>(out, in);
    round<30, 0x06ca6351>(out, in);
    round<31, 0x14292967>(out, in);
    round<32, 0x27b70a85>(out, in);
    round<33, 0x2e1b2138>(out, in);
    round<34, 0x4d2c6dfc>(out, in);
    round<35, 0x53380d13>(out, in);
    round<36, 0x650a7354>(out, in);
    round<37, 0x766a0abb>(out, in);
    round<38, 0x81c2c92e>(out, in);
    round<39, 0x92722c85>(out, in);
    round<40, 0xa2bfe8a1>(out, in);
    round<41, 0xa81a664b>(out, in);
    round<42, 0xc24b8b70>(out, in);
    round<43, 0xc76c51a3>(out, in);
    round<44, 0xd192e819>(out, in);
    round<45, 0xd6990624>(out, in);
    round<46, 0xf40e3585>(out, in);
    round<47, 0x106aa070>(out, in);
    round<48, 0x19a4c116>(out, in);
    round<49, 0x1e376c08>(out, in);
    round<50, 0x2748774c>(out, in);
    round<51, 0x34b0bcb5>(out, in);
    round<52, 0x391c0cb3>(out, in);
    round<53, 0x4ed8aa4a>(out, in);
    round<54, 0x5b9cca4f>(out, in);
    round<55, 0x682e6ff3>(out, in);
    round<56, 0x748f82ee>(out, in);
    round<57, 0x78a5636f>(out, in);
    round<58, 0x84c87814>(out, in);
    round<59, 0x8cc70208>(out, in);
    round<60, 0x90befffa>(out, in);
    round<61, 0xa4506ceb>(out, in);
    round<62, 0xbef9a3f7>(out, in);
    round<63, 0xc67178f2>(out, in);
}

inline void summary8(state& out, const state& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[0] += in[0];
    out[1] += in[1];
    out[2] += in[2];
    out[3] += in[3];
    out[4] += in[4];
    out[5] += in[5];
    out[6] += in[6];
    out[7] += in[7];
    BC_POP_WARNING()
}

inline void copying8(buffer& out, const state& in) NOEXCEPT
{
    auto& to = narrow_array_cast<uint32_t, state_size>(out);
    to = in;
}

inline void copyin64(buffer& out, const buffer& in) NOEXCEPT
{
    out = in;
}

inline void bigend16(buffer& out, const block& in) NOEXCEPT
{
    constexpr auto size = block_size / sizeof(uint32_t);
    auto& from = array_cast<uint32_t>(in);
    auto& to = narrow_array_cast<uint32_t, size>(out);
    from_big_endians(to, from);
}

inline void bigend08(digest& out, const state& in) NOEXCEPT
{
    auto& to = array_cast<uint32_t>(out);
    to_big_endians(to, in);
}

inline void padding8(buffer& out) NOEXCEPT
{
    // TODO: safe offsetting array cast.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    auto& to = unsafe_array_cast<uint32_t, state_size>(&out[state_size]);
    BC_POP_WARNING()

    to = pad32;
}

// This requires 32 more words of memory than a circular variables buffer.
// According to FIP180 this is more performant given no memory constraint.
void hash_native(state& state, const block& block) NOEXCEPT
{
    buffer words;
    const sha256::state start{ state };
    bigend16(words, block);
    expand48(words);
    rounds64(state, words);
    summary8(state, start);
}

// TODO: template with sized array of blocks.
void hash_native(state& state, const block1& blocks) NOEXCEPT
{
    buffer words;

    for (auto& block: blocks)
    {
        const sha256::state start{ state };
        bigend16(words, block);
        expand48(words);
        rounds64(state, words);
        summary8(state, start);
    }
}

void hash_finalize(digest& digest, const state& state) NOEXCEPT
{
    bigend08(digest, state);
}

#ifndef NORMALIZED_NATIVE_MERKLE

// TODO: template with equally-sized arrays of blocks/digests.
// This is an optimization for merkle root computation.
// It leverages fixed message size to avoid padding computations.
// It leverages fixed iteration count to avoid internal endian conversions.
// It leverages a common buffer to avoid stack calls and buffer allocations.
// expanded_pad64 is the output of expand48(pad64).
void merkle_hash(digest1& digest, const block1& blocks) NOEXCEPT
{
    buffer words;
    auto out = digest.begin();

    for (auto& block: blocks)
    {
        bigend16(words, block);
        expand48(words);
        auto state = sha256::initial;
        rounds64(state, words);
        summary8(state, sha256::initial);

        copyin64(words, expanded_pad64);
        const auto save = state;
        rounds64(state, words);
        summary8(state, save);

        copying8(words, state);
        padding8(words);
        expand48(words);
        state = sha256::initial;
        rounds64(state, words);
        summary8(state, sha256::initial);

        bigend08(*(out++), state);
    }
}

#else
void merkle_hash(digest1& out, const block1& blocks) NOEXCEPT
{
    auto state = sha256::initial;
    hash_native(state, blocks);
    hash_native(state, array_cast<block>(sha256::pad_64));
    auto buffer = sha256::pad_32;
    to_big_endians(narrow_array_cast<uint32_t, state_size>(buffer), state);
    state = sha256::initial;
    hash_native(state, array_cast<block>(buffer));
    to_big_endians(array_cast<uint32_t>(out.front()), state);
}

////// Specialized for single block padding.
////template <size_t Offset,
////    if_not_greater<Offset, integers> = true>
////void constexpr pad(auto& buffer) NOEXCEPT
////{
////    constexpr auto sentinel = bit_hi<uint32_t>;
////    constexpr auto bitcount = possible_narrow_cast<uint32_t>(
////        (integers - Offset) * bits<uint32_t>);
////
////    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
////    buffer[Offset] = sentinel;
////    buffer[sub1(integers)] = bitcount;
////    std::fill(&buffer[add1(Offset)], &buffer[sub1(integers)], 0);
////    BC_POP_WARNING()
////}
#endif // NORMALIZED_NATIVE_MERKLE

} // namespace sha256
} // namespace system
} // namespace libbitcoin
