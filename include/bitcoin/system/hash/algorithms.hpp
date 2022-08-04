/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_ALGORITHMS_HPP
#define LIBBITCOIN_SYSTEM_HASH_ALGORITHMS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/rmd/algorithm.hpp>
#include <bitcoin/system/hash/sha/algorithm.hpp>

namespace libbitcoin {
namespace system {

/// bc::system rmd algorithm aliases (concurrent).
using rmd128     = rmd::algorithm<rmd::h128<>,    true>;
using rmd128_256 = rmd::algorithm<rmd::h128<256>, true>; // not fully implemented
using rmd160     = rmd::algorithm<rmd::h160<>,    true>;
using rmd160_320 = rmd::algorithm<rmd::h160<320>, true>; // not fully implemented

/// bc::system sha algorithm aliases.
using sha160     = sha::algorithm<sha::h160,      true>;
using sha256_224 = sha::algorithm<sha::h256<224>, true>; // not fully implemented
using sha256     = sha::algorithm<sha::h256<>,    true>;
using sha512_256 = sha::algorithm<sha::h512<256>, true>; // not fully implemented
using sha512_224 = sha::algorithm<sha::h512<224>, true>; // not fully implemented
using sha512_384 = sha::algorithm<sha::h512<384>, true>; // not fully implemented
using sha512     = sha::algorithm<sha::h512<>,    true>;

} // namespace system
} // namespace libbitcoin

#endif
