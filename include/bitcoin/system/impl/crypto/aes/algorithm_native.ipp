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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_NATIVE_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_NATIVE_IPP

// Native (AES-NI/VAES or Crypto)
// ============================================================================
// Round keys are replicated to each 128 bit lane of an extended word, so a
// VAES word ciphers one block per lane. Decryption applies the equivalent
// inverse cipher, with inverse mix columns applied to the middle round keys.

namespace libbitcoin {
namespace system {
namespace aes {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template <typename xWord>
INLINE typename CLASS::template native_keys_t<xWord> CLASS::
native_encrypt_keys(const schedule_t& schedule) NOEXCEPT
{
    native_keys_t<xWord> keys{};
    for (size_t round{}; round < K::round_keys; ++round)
    {
        const auto& key = array_cast<xint128_t>(schedule[round]).front();
        keys[round] = aes::replicate<xWord>(f::load<uint8_t>(key));
    }

    return keys;
}

TEMPLATE
template <typename xWord>
INLINE typename CLASS::template native_keys_t<xWord> CLASS::
native_decrypt_keys(const schedule_t& schedule) NOEXCEPT
{
    native_keys_t<xWord> keys{};
    const auto& first = array_cast<xint128_t>(schedule.front()).front();
    const auto& last = array_cast<xint128_t>(schedule.back()).front();
    keys.front() = f::load<uint8_t>(last);
    for (auto round = one; round < K::rounds; ++round)
    {
        const auto& key = array_cast<xint128_t>(schedule[K::rounds - round]);
        keys[round] = aes::inverse(f::load<uint8_t>(key.front()));
    }

    keys.back() = f::load<uint8_t>(first);
    return keys;
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
native_encrypt(native_state_t<xWord>& state,
    const native_keys_t<xWord>& keys) NOEXCEPT
{
    aes::encrypt<K::rounds>(state, keys);
}

TEMPLATE
void CLASS::
encrypt_native(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    auto& word = array_cast<xint128_t>(block).front();
    std_array<xint128_t, one> state{ f::load<uint8_t>(word) };
    aes::encrypt<K::rounds>(state, native_encrypt_keys<xint128_t>(schedule));
    f::store<uint8_t>(word, state.front());
}

TEMPLATE
void CLASS::
decrypt_native(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    auto& word = array_cast<xint128_t>(block).front();
    std_array<xint128_t, one> state{ f::load<uint8_t>(word) };
    aes::decrypt<K::rounds>(state, native_decrypt_keys<xint128_t>(schedule));
    f::store<uint8_t>(word, state.front());
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
