/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_PUBLIC_HPP
#define LIBBITCOIN_PARSE_ENCRYPTED_PUBLIC_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include "parse_encrypted_key.hpp"

namespace libbitcoin {
namespace wallet {

// Swap not defined.
class parse_encrypted_public
  : public parse_encrypted_key<5u>
{
public:
    static byte_array<prefix_size> prefix_factory(uint8_t address);

    explicit parse_encrypted_public(const encrypted_public& key);

    uint8_t address_version() const;

    one_byte sign() const;
    hash_digest data() const;

private:
    bool verify_magic() const;

    static constexpr uint8_t default_context_ = 0x9a;
    static const byte_array<magic_size> magic_;

    const one_byte sign_;
    const hash_digest data_;
};

} // namespace wallet
} // namespace libbitcoin

#endif
