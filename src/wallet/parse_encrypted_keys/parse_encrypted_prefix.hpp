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
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_PREFIX_HPP
#define LIBBITCOIN_PARSE_ENCRYPTED_PREFIX_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

// BIP38
// Alt-chain implementers should exploit the address hash for [identification].
// Since each operation in this proposal involves hashing a text representation
// of a coin address which (for Bitcoin) includes the leading '1', an alt-chain
// can easily be denoted simply by using the alt-chain's preferred format for
// representing an address.... Alt-chain implementers may also change the prefix
// such that encrypted addresses do not start with "6P".

namespace libbitcoin {
namespace wallet {

static constexpr size_t mainnet = 0x00;

// BIP38
// It is requested that the unused flag bytes NOT be used for denoting that
// the key belongs to an alt-chain [This shoud read "flag bits"].
enum encrypted_key_flag : uint8_t
{
    none = 0,
    lot_sequence = 1 << 2,
    ec_compressed = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,

    // Two bits are used to represent "not multiplied".
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};

template<uint8_t Version, size_t Size>
class parse_encrypted_prefix
{
public:
    bool valid() const;

    static constexpr uint8_t prefix_size = Size;

    parse_encrypted_prefix(const byte_array<Size>& value);

protected:
    uint8_t context() const;
    byte_array<Size> prefix() const;
    void valid(bool value);
    uint8_t version() const;

    static constexpr uint8_t default_version = Version;
    static constexpr uint8_t magic_size = prefix_size - 2;

private:
    bool verify_version() const;

    const one_byte version_;
    const byte_array<magic_size> magic_;
    const one_byte context_;
    bool valid_;
};

} // namespace wallet
} // namespace libbitcoin

#include "parse_encrypted_prefix.ipp"

#endif
