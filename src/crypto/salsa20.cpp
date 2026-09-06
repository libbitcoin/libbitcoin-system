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
#include <bitcoin/system/crypto/salsa20.hpp>

#include <bit>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// cr.yp.to/snuffle/spec.pdf (Salsa20)
// cr.yp.to/highspeed/naclcrypto-20090310.pdf (HSalsa20, XSalsa20)
// [Bernstein]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr std_array<uint32_t, 4> sigma
{
    0x61707865_u32, 0x3320646e_u32, 0x79622d32_u32, 0x6b206574_u32
};

// rounds
// ----------------------------------------------------------------------------

template <size_t A, size_t B, size_t C, size_t D>
constexpr void salsa20::quarter(state_t& x) NOEXCEPT
{
    x[B] ^= std::rotl(x[A] + x[D], 7);
    x[C] ^= std::rotl(x[B] + x[A], 9);
    x[D] ^= std::rotl(x[C] + x[B], 13);
    x[A] ^= std::rotl(x[D] + x[C], 18);
}

// A double round is a column round followed by a row round.
constexpr void salsa20::double_round(state_t& x) NOEXCEPT
{
    quarter< 0,  4,  8, 12>(x);
    quarter< 5,  9, 13,  1>(x);
    quarter<10, 14,  2,  6>(x);
    quarter<15,  3,  7, 11>(x);
    quarter< 0,  1,  2,  3>(x);
    quarter< 5,  6,  7,  4>(x);
    quarter<10, 11,  8,  9>(x);
    quarter<15, 12, 13, 14>(x);
}

constexpr void salsa20::rounds(state_t& x) NOEXCEPT
{
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
}

constexpr salsa20::state_t salsa20::initial(const keys_t& key,
    const words_t& words) NOEXCEPT
{
    return
    {
        sigma[0], key[0],   key[1],   key[2],
        key[3],   sigma[1], words[0], words[1],
        words[2], words[3], sigma[2], key[4],
        key[5],   key[6],   key[7],   sigma[3]
    };
}

// hsalsa20
// ----------------------------------------------------------------------------

void salsa20::hsalsa20(secret& out, const secret& key, const input& in) NOEXCEPT
{
    keys_t keys{};
    words_t words{};
    from_little_endians(keys, array_cast<uint32_t>(key));
    from_little_endians(words, array_cast<uint32_t>(in));

    // The rounds are applied without the final summation.
    auto x = initial(keys, words);
    rounds(x);

    // The output is the diagonal and the words 6-9 of the state.
    const keys_t result
    {
        x[0], x[5], x[10], x[15], x[6], x[7], x[8], x[9]
    };

    to_little_endians(array_cast<uint32_t>(out), result);
    keys = {};
}

// salsa20
// ----------------------------------------------------------------------------

salsa20::salsa20(const secret& key) NOEXCEPT
{
    set_key(key);
}

salsa20::salsa20(const secret& key, const extended_nonce& nonce) NOEXCEPT
{
    // The subkey is derived from the first 16 bytes of the extended nonce.
    secret subkey{};
    hsalsa20(subkey, key, slice<zero, input_size>(nonce));
    set_key(subkey);
    subkey = {};

    // The remaining 8 bytes are the Salsa20 nonce.
    seek(from_little_array<uint64_t>(
        slice<input_size, extended_nonce_size>(nonce)), zero);
}

void salsa20::set_key(const secret& key) NOEXCEPT
{
    from_little_endians(key_, array_cast<uint32_t>(key));
    nonce_ = {};
    counter_ = {};
    offset_ = block_size;
}

void salsa20::seek(uint64_t nonce, uint64_t counter) NOEXCEPT
{
    // Words 6-7 are the nonce, which MUST not be repeated for the same key.
    nonce_[0] = narrow_cast<uint32_t>(nonce);
    nonce_[1] = narrow_cast<uint32_t>(shift_right(nonce, 32));
    counter_ = counter;
    offset_ = block_size;
}

// private
void salsa20::next(block& out) NOEXCEPT
{
    // Words 8-9 are the block counter.
    nonce_[2] = narrow_cast<uint32_t>(counter_);
    nonce_[3] = narrow_cast<uint32_t>(shift_right(counter_, 32));

    auto x = initial(key_, nonce_);
    const auto start = x;
    rounds(x);

    for (size_t index{}; index < state_words; ++index)
        x[index] += start[index];

    to_little_endians(array_cast<uint32_t>(out), x);
    ++counter_;
}

void salsa20::crypt(const_byte_span in, byte_span out) NOEXCEPT
{
    BC_ASSERT(in.empty() || in.size() == out.size());
    const auto size = out.size();
    size_t byte{};

    // Continue buffered keystream.
    for (; (offset_ < block_size) && (byte < size); ++offset_, ++byte)
        out[byte] = in.empty() ? buffer_[offset_] :
            bit_xor(in[byte], buffer_[offset_]);

    // Sequential whole blocks fully consume buffered keystream.
    for (; (size - byte) >= block_size; byte += block_size)
    {
        next(buffer_);
        for (size_t index{}; index < block_size; ++index)
            out[byte + index] = in.empty() ? buffer_[index] :
                bit_xor(in[byte + index], buffer_[index]);
    }

    // Partial block, keystream remainder buffered.
    if (byte < size)
    {
        next(buffer_);
        offset_ = zero;

        for (; byte < size; ++byte)
            out[byte] = in.empty() ? buffer_[offset_++] :
                bit_xor(in[byte], buffer_[offset_++]);
    }
}

void salsa20::stream(byte_span out) NOEXCEPT
{
    crypt({}, out);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
