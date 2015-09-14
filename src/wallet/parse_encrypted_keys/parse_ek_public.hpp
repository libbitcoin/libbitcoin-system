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
#ifndef LIBBITCOIN_PARSE_EK_PUBLIC_HPP
#define LIBBITCOIN_PARSE_EK_PUBLIC_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include "parse_ek_key.hpp"

namespace libbitcoin {
namespace wallet {

class parse_ek_public
  : public parse_ek_key<5>
{
public:
    static byte_array<prefix_size> prefix_factory(uint8_t address);

    parse_ek_public(const ek_public& key);

    uint8_t address_version() const;

    one_byte sign() const;
    hash_digest data() const;

private:
    static constexpr uint8_t default_context = 0x9a;

    const one_byte sign_;
    const hash_digest data_;
};

} // namespace wallet
} // namespace libbitcoin

#endif
