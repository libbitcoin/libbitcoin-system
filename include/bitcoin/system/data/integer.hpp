/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_INTEGER_HPP
#define LIBBITCOIN_SYSTEM_DATA_INTEGER_HPP

#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>

namespace libbitcoin {
namespace system {
   
template <uint32_t Bits>
using uintx_t = boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<Bits, Bits,
        boost::multiprecision::unsigned_magnitude,
        boost::multiprecision::unchecked, void>>;

typedef uintx_t<5> uint5_t;
typedef uintx_t<11> uint11_t;
typedef uintx_t<48> uint48_t;
typedef uintx_t<128> uint128_t;
typedef uintx_t<160> uint160_t;
typedef uintx_t<256> uint256_t;
typedef uintx_t<512> uint512_t;

} // namespace system
} // namespace libbitcoin

#endif
