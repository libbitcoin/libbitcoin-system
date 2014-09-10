/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_AMOUNT_HPP
#define LIBBITCOIN_AMOUNT_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

/**
 * Validates and parses an amount string according to the BIP 21 grammar.
 * @param decmial_places the location of the decimal point. The default
 * value converts bitcoins to satoshis.
 * @return parsed value, or invalid_amount for failure.
 */
BC_API uint64_t parse_amount(const std::string& amount,
    uint8_t decimal_places=8);

/**
 * Writes a Bitcoin amount to a string, following the BIP 21 grmmar.
 * Avoids the rounding issues often seen with floating-point methods.
 * @param decmial_places the location of the decimal point. The default
 * value converts satoshis to bitcoins.
 */
BC_API std::string format_amount(uint64_t amount, uint8_t decimal_places=8);

} // namespace libbitcoin

#endif

