/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_PARSE_ENCRYPTED_PRIVATE_HPP
#define LIBBITCOIN_SYSTEM_PARSE_ENCRYPTED_PRIVATE_HPP

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include "parse_encrypted_key.hpp"

namespace libbitcoin {
namespace system {
namespace wallet {

// Swap not defined.
class parse_encrypted_private
  : public parse_encrypted_key<2u>
{
public:
    static data_array<prefix_size> prefix_factory(uint8_t address,
        bool multiplied) NOEXCEPT;

    explicit parse_encrypted_private(const encrypted_private& key) NOEXCEPT;

    bool multiplied() const NOEXCEPT;
    uint8_t address_version() const NOEXCEPT;

    quarter_hash data1() const NOEXCEPT;
    half_hash data2() const NOEXCEPT;

private:
    bool verify_magic() const NOEXCEPT;

    static constexpr uint8_t default_context_ = 0x42;
    static constexpr uint8_t multiplied_context_ = 0x43;
    static const data_array<magic_size> magic_;

    const quarter_hash data1_;
    const half_hash data2_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
