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
#ifndef LIBBITCOIN_SYSTEM_HASH_HASH_HPP
#define LIBBITCOIN_SYSTEM_HASH_HASH_HPP

#include <bitcoin/system/hash/accumulator.hpp>
#include <bitcoin/system/hash/checksum.hpp>
#include <bitcoin/system/hash/functions.hpp>
#include <bitcoin/system/hash/hmac_sha256.hpp>
#include <bitcoin/system/hash/hmac_sha512.hpp>
#include <bitcoin/system/hash/pbkd_sha256.hpp>
#include <bitcoin/system/hash/pbkd_sha512.hpp>
#include <bitcoin/system/hash/ripemd160.hpp>
#include <bitcoin/system/hash/rmd_algorithm.hpp>
#include <bitcoin/system/hash/scrypt.hpp>
#include <bitcoin/system/hash/sha_algorithm.hpp>
#include <bitcoin/system/hash/siphash.hpp>

// vectorization intrinsics into algorithms using math templates.
// sha-ni intrinsics into sha algorithm using shani math template.

// sha    (algorithm)
// ripemd (algorithm)
// accumulator<algorithm>
// hmac<accumulator<algorithm>>
// pbkd<hmac<accumulator<algorithm>>>
// scrypt          -> hash
// checksum        -> hash
// siphash         -> hash

#endif
