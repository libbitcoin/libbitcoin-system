/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__FORMATS_BASE_10_HPP
#define LIBBITCOIN__FORMATS_BASE_10_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <p_uint64_t.hpp>
#include <p_std_string.hpp>

namespace libbitcoin {
namespace api {

BC_CONSTEXPR uint8_t btc_decimal_places = 8;
BC_CONSTEXPR uint8_t mbtc_decimal_places = 5;
BC_CONSTEXPR uint8_t ubtc_decimal_places = 2;

/**
 * Validates and parses an amount string according to the BIP 21 grammar.
 * @param decmial_places the location of the decimal point.
 * The default is 0, which treats the input as a normal integer.
 * @param strict true to treat fractional results as an error,
 * or false to round them upwards.
 * @return false for failure.
 */
BC_API bool decode_base10(p_uint64_t& out, const p_std_string amount,
    uint8_t decimal_places=0, bool strict=true);

/**
 * Writes a Bitcoin amount to a string, following the BIP 21 grammar.
 * Avoids the rounding issues often seen with floating-point methods.
 * @param decmial_places the location of the decimal point.
 * The default is 0, which treats the input as a normal integer.
 */
BC_API std::string encode_base10(uint64_t amount,
    uint8_t decimal_places=0);

// Old names:
BC_API bool btc_to_satoshi(p_uint64_t& satoshi, const p_std_string btc);
BC_API std::string satoshi_to_btc(uint64_t satoshi);

} // namespace api
} // namespace libbitcoin

#endif

