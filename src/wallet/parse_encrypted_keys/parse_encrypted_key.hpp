/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_KEY_HPP
#define LIBBITCOIN_PARSE_ENCRYPTED_KEY_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_encrypted_prefix.hpp"

namespace libbitcoin {
namespace wallet {

template<uint8_t Version, size_t Size>
class parse_encrypted_key
  : public parse_encrypted_prefix<Version, Size>
{
public:
    parse_encrypted_key(const byte_array<Size>& prefix, const one_byte& flags,
        const salt& salt, const entropy& entropy);

    bool compressed() const;
    entropy entropy() const;
    uint8_t flags() const;
    bool lot_sequence() const;
    data_chunk owner_salt() const;
    salt salt() const;

private:
    const one_byte flags_;
    const wallet::salt salt_;
    const wallet::entropy entropy_;
};

} // namespace wallet
} // namespace libbitcoin

#include "parse_encrypted_key.ipp"

#endif
