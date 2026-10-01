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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_CTR_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_CTR_IPP

// Modes
// ============================================================================
// Counter blocks are independent, so are ciphered concurrently, widest words
// first. The narrowest word also ciphers any final partial pass.

namespace libbitcoin {
namespace system {
namespace aes {

// public
// ----------------------------------------------------------------------------

TEMPLATE
void CLASS::
ctr(byte_span out, const_byte_span in, block_t& counter,
    const schedule_t& schedule) NOEXCEPT
{
    BC_ASSERT(in.empty() || in.size() == out.size());

    if constexpr (use_aes)
    {
        auto byte = native_ctr<xint512_t>(out, in, zero, counter, schedule);
        byte = native_ctr<xint256_t>(out, in, byte, counter, schedule);
        native_ctr<xint128_t>(out, in, byte, counter, schedule);
    }
    else
    {
        const auto keys = slice(schedule);
        auto byte = sliced_ctr<xint512_t>(out, in, zero, counter, keys);
        byte = sliced_ctr<xint256_t>(out, in, byte, counter, keys);
        byte = sliced_ctr<xint128_t>(out, in, byte, counter, keys);
        sliced_ctr<uint64_t>(out, in, byte, counter, keys);
    }
}

TEMPLATE
void CLASS::
cbc_decrypt(byte_span out, const_byte_span in, block_t& iv,
    const schedule_t& schedule) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());
    BC_ASSERT(is_zero(in.size() % block_bytes));
    const auto count = in.size() / block_bytes;

    if constexpr (use_aes)
    {
        const auto keys = native_decrypt_keys<xint128_t>(schedule);
        for (size_t index{}; index < count; ++index)
        {
            const auto at = index * block_bytes;
            block_t cipher{};
            std::copy_n(std::next(in.begin(), at), block_bytes, cipher.begin());

            auto plain = cipher;
            auto& word = array_cast<xint128_t>(plain).front();
            std_array<xint128_t, one> state{ f::load(word) };
            aes::decrypt<K::rounds>(state, keys);
            f::store(word, state.front());

            for (size_t byte{}; byte < block_bytes; ++byte)
                out[at + byte] = bit_xor(plain[byte], iv[byte]);

            iv = cipher;
        }
    }
    else
    {
        const auto keys = slice(schedule);
        for (size_t index{}; index < count; index += slice_blocks)
        {
            const auto used = std::min(slice_blocks, count - index);
            xblocks_t<slice_blocks> cipher{};
            for (size_t block{}; block < used; ++block)
            {
                const auto at = (index + block) * block_bytes;
                const auto from = std::next(in.begin(), at);
                std::copy_n(from, block_bytes, cipher[block].begin());
            }

            auto plain = cipher;
            xdecrypt<uint64_t>(plain, keys);

            for (size_t block{}; block < used; ++block)
            {
                const auto at = (index + block) * block_bytes;
                for (size_t byte{}; byte < block_bytes; ++byte)
                    out[at + byte] = bit_xor(plain[block][byte], iv[byte]);

                iv = cipher[block];
            }
        }
    }
}

// protected
// ----------------------------------------------------------------------------

// Increment the big-endian low order 32 bits (sp800-38d inc32).
TEMPLATE
INLINE void CLASS::
increment(block_t& counter) NOEXCEPT
{
    to_big<12>(counter, add1(from_big<uint32_t, 12>(counter)));
}

TEMPLATE
INLINE void CLASS::
apply(byte_span out, const_byte_span in, size_t start,
    const_byte_span stream) NOEXCEPT
{
    if (in.empty())
        for (size_t byte{}; byte < stream.size(); ++byte)
            out[start + byte] = stream[byte];
    else
        for (size_t byte{}; byte < stream.size(); ++byte)
            out[start + byte] = bit_xor(in[start + byte], stream[byte]);
}

// Crypts whole passes (and any final partial pass for xint128_t), returns
// bytes consumed.
TEMPLATE
template <typename xWord>
INLINE size_t CLASS::
native_ctr(byte_span out, const_byte_span in, size_t start,
    block_t& counter, const schedule_t& schedule) NOEXCEPT
{
    constexpr auto last = is_same_type<xWord, xint128_t>;
    auto byte = start;

    if constexpr (last || (use_vaes && have<xWord>))
    {
        constexpr auto lanes = native_words * capacity<xWord, uint64_t, 2>;
        constexpr auto size = lanes * block_bytes;
        const auto more = [&]() NOEXCEPT
        {
            const auto remaining = out.size() - byte;
            return (remaining >= size) || (last && !is_zero(remaining));
        };

        if (!more())
            return byte;

        // The counter prefix is constant, only its low order word changes.
        const auto keys = native_encrypt_keys<xWord>(schedule);
        alignas(xWord) xblocks_t<lanes> xstream{};
        xstream.fill(counter);

        do
        {
            const auto value = from_big<uint32_t, 12>(counter);
            for (size_t block{}; block < lanes; ++block)
                to_big<12>(xstream[block], value +
                    possible_narrow_cast<uint32_t>(block));

            auto& words = array_cast<xWord>(xstream);
            native_state_t<xWord> state{};
            for (size_t word{}; word < native_words; ++word)
                state[word] = f::load(words[word]);

            native_encrypt(state, keys);

            // Whole passes are applied directly to out, through bytes, as
            // caller buffers are not vector aligned.
            if ((out.size() - byte) >= size)
            {
                const auto to = std::next(out.data(), byte);
                if (in.empty())
                {
                    for (size_t word{}; word < native_words; ++word)
                        f::store(std::next(to, word * sizeof(xWord)),
                            state[word]);
                }
                else
                {
                    const auto from = std::next(in.data(), byte);
                    for (size_t word{}; word < native_words; ++word)
                    {
                        const auto at = word * sizeof(xWord);
                        f::store(std::next(to, at), f::xor_(state[word],
                            f::load<xWord>(std::next(from, at))));
                    }
                }

                to_big<12>(counter, value +
                    possible_narrow_cast<uint32_t>(lanes));
                byte += size;
                continue;
            }

            for (size_t word{}; word < native_words; ++word)
                f::store(words[word], state[word]);

            for (size_t block{}; (block < xstream.size()) &&
                (byte < out.size()); ++block)
            {
                const auto used = std::min(block_bytes, out.size() - byte);
                apply(out, in, byte, { xstream[block].data(), used });
                increment(counter);
                byte += used;
            }
        }
        while (more());
    }

    return byte;
}

// Crypts whole passes (and any final partial pass for uint64_t), returns
// bytes consumed.
TEMPLATE
template <typename Word>
INLINE size_t CLASS::
sliced_ctr(byte_span out, const_byte_span in, size_t start,
    block_t& counter, const xkeys_t<uint64_t>& keys) NOEXCEPT
{
    constexpr auto last = is_same_type<Word, uint64_t>;
    constexpr auto enabled = []() NOEXCEPT
    {
        if constexpr (last)
            return true;
        else
            return Vector && have<Word>;
    }();

    auto byte = start;
    if constexpr (enabled)
    {
        constexpr auto lanes = slice_blocks * capacity<Word, uint64_t>;
        constexpr auto size = lanes * block_bytes;
        const auto more = [&]() NOEXCEPT
        {
            const auto remaining = out.size() - byte;
            return (remaining >= size) || (last && !is_zero(remaining));
        };

        if (!more())
            return byte;

        xkeys_t<Word> wide{};
        if constexpr (last)
            wide = keys;
        else
            wide = broadcast<Word>(keys);

        do
        {
            xblocks_t<lanes> xstream{};
            auto next = counter;
            for (auto& block: xstream)
            {
                block = next;
                increment(next);
            }

            xencrypt<Word>(xstream, wide);

            // Whole passes are applied directly to out, in bytes, as caller
            // buffers are not word aligned. A loop over word casts of them
            // may be vectorized with aligned stores, which fault.
            if ((out.size() - byte) >= size)
            {
                const auto& keystream = array_cast<uint8_t>(xstream);
                auto& to = unsafe_array_cast<uint8_t, size>(
                    std::next(out.data(), byte));

                if (in.empty())
                {
                    to = keystream;
                }
                else
                {
                    const auto& from = unsafe_array_cast<uint8_t, size>(
                        std::next(in.data(), byte));

                    for (size_t index{}; index < size; ++index)
                        to[index] = bit_xor(from[index], keystream[index]);
                }

                counter = next;
                byte += size;
                continue;
            }

            for (size_t block{}; (block < xstream.size()) &&
                (byte < out.size()); ++block)
            {
                const auto used = std::min(block_bytes, out.size() - byte);
                apply(out, in, byte, { xstream[block].data(), used });
                increment(counter);
                byte += used;
            }
        }
        while (more());
    }

    return byte;
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
