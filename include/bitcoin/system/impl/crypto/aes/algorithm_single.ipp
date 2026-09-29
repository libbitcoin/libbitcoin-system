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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_SINGLE_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_SINGLE_IPP

// Single block
// ============================================================================

namespace libbitcoin {
namespace system {
namespace aes {

// public
// ----------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
encrypt(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        encrypt_sliced(block, schedule);
    }
    else if constexpr (use_aes)
    {
        encrypt_native(block, schedule);
    }
    else
    {
        encrypt_sliced(block, schedule);
    }
}

TEMPLATE
constexpr void CLASS::
decrypt(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        decrypt_sliced(block, schedule);
    }
    else if constexpr (use_aes)
    {
        decrypt_native(block, schedule);
    }
    else
    {
        decrypt_sliced(block, schedule);
    }
}

TEMPLATE
constexpr void CLASS::
encrypt(block_t& block, const key_t& key) NOEXCEPT
{
    encrypt(block, expand(key));
}

TEMPLATE
constexpr void CLASS::
decrypt(block_t& block, const key_t& key) NOEXCEPT
{
    decrypt(block, expand(key));
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
