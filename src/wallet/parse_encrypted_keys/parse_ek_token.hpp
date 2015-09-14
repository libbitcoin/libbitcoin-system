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
#ifndef LIBBITCOIN_PARSE_EK_TOKEN_HPP
#define LIBBITCOIN_PARSE_EK_TOKEN_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_ek_key.hpp"

namespace libbitcoin {
namespace wallet {

class parse_ek_token
  : public parse_ek_prefix<0x2c, 8>
{
public:
    static byte_array<prefix_size> prefix_factory(bool lot_sequence);

    parse_ek_token(const ek_token& value);

    bool lot_sequence() const;
    ////uint8_t address_version() const;
    hash_digest data() const;
    ek_entropy entropy() const;
    one_byte sign() const;

private:
    bool verify_context() const;
    bool verify_magic() const;

    static const byte_array<magic_size> magic;
    static constexpr uint8_t lot_context = 0x51;
    static constexpr uint8_t default_context = 0x53;

    const ek_entropy entropy_;
    const one_byte sign_;
    const hash_digest data_;
};

} // namespace wallet
} // namespace libbitcoin

#endif
