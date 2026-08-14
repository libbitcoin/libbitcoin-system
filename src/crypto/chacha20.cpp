/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/crypto/chacha20.hpp>

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8439
// [Nir, Langley]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// rfc8439
// The ChaCha20 state is initialized as follows: The first four words
// (0-3) are constants: 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574.
constexpr std_array<uint32_t, 4> sigma
{
    0x61707865_u32, 0x3320646e_u32, 0x79622d32_u32, 0x6b206574_u32
};

// chacha20
// ----------------------------------------------------------------------------

chacha20::chacha20(const secret& key) NOEXCEPT
{
    set_key(key);
}

void chacha20::set_key(const secret& key) NOEXCEPT
{
    // rfc8439
    // The next eight words (4-11) are taken from the 256-bit key by reading
    // the bytes in little-endian order, in 4-byte chunks.
    from_little_endians(key_, array_cast<uint32_t>(key));
    nonce_ = {};
    counter_ = {};
    offset_ = block_size;
}

void chacha20::seek(uint32_t nonce32, uint64_t nonce64,
    uint32_t counter) NOEXCEPT
{
    // rfc8439
    // Words 13-15 are a nonce, which MUST not be repeated for the same key.
    nonce_[0] = nonce32;
    nonce_[1] = narrow_cast<uint32_t>(nonce64);
    nonce_[2] = narrow_cast<uint32_t>(shift_right(nonce64, 32u));
    counter_ = counter;
    offset_ = block_size;
}

// static/private
template <size_t A, size_t B, size_t C, size_t D>
constexpr void chacha20::quarter(std_array<uint32_t, 16>& x) NOEXCEPT
{
    // rfc8439
    // The basic operation of the ChaCha algorithm is the quarter round.
    x[A] += x[B]; x[D] = rotl<16>(x[D] ^ x[A]);
    x[C] += x[D]; x[B] = rotl<12>(x[B] ^ x[C]);
    x[A] += x[B]; x[D] = rotl< 8>(x[D] ^ x[A]);
    x[C] += x[D]; x[B] = rotl< 7>(x[B] ^ x[C]);
}

// private
void chacha20::next(block& out) NOEXCEPT
{
    const std_array<uint32_t, 16> state
    {
        sigma[0], sigma[1], sigma[2], sigma[3],
        key_[0], key_[1], key_[2], key_[3],
        key_[4], key_[5], key_[6], key_[7],
        counter_, nonce_[0], nonce_[1], nonce_[2]
    };

    auto x = state;

    // rfc8439
    // ChaCha20 runs 20 rounds, alternating between column and diagonal rounds.
    for (size_t round{}; round < 10_size; ++round)
    {
        quarter<0, 4,  8, 12>(x);
        quarter<1, 5,  9, 13>(x);
        quarter<2, 6, 10, 14>(x);
        quarter<3, 7, 11, 15>(x);
        quarter<0, 5, 10, 15>(x);
        quarter<1, 6, 11, 12>(x);
        quarter<2, 7,  8, 13>(x);
        quarter<3, 4,  9, 14>(x);
    }

    // rfc8439
    // At the end of 20 rounds, we add the original input words to the output
    // words, and serialize the result by sequencing the words one-by-one in
    // little-endian order.
    for (size_t word{}; word < x.size(); ++word)
        x[word] += state[word];

    to_little_endians(array_cast<uint32_t>(out), x);
    ++counter_;
}

void chacha20::crypt(std::span<const uint8_t> in,
    std::span<uint8_t> out) NOEXCEPT
{
    BC_ASSERT(in.size() == out.size());

    for (size_t byte{}; byte < in.size(); ++byte)
    {
        if (offset_ == block_size)
        {
            next(buffer_);
            offset_ = zero;
        }

        out[byte] = in[byte] ^ buffer_[offset_++];
    }
}

void chacha20::stream(std::span<uint8_t> out) NOEXCEPT
{
    for (size_t byte{}; byte < out.size(); ++byte)
    {
        if (offset_ == block_size)
        {
            next(buffer_);
            offset_ = zero;
        }

        out[byte] = buffer_[offset_++];
    }
}

// fschacha20
// ----------------------------------------------------------------------------

fschacha20::fschacha20(const chacha20::secret& key, uint32_t interval) NOEXCEPT
  : cipher_(key), interval_(interval)
{
}

void fschacha20::crypt(std::span<const uint8_t> in,
    std::span<uint8_t> out) NOEXCEPT
{
    cipher_.crypt(in, out);

    // bip324
    // The key is rotated after every rekey_interval chunks, to the next 32
    // keystream bytes, and the nonce set to {0, rekey counter}.
    if (++chunks_ == interval_)
    {
        chacha20::secret key{};
        cipher_.stream(key);
        cipher_.set_key(key);
        cipher_.seek(0, ++rekeys_, 0);
        key = {};
        chunks_ = 0;
    }
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
