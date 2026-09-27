/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_ALGORITHMS_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_ALGORITHMS_HPP

#include <bitcoin/system/crypto/aes/algorithm.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// bc::system aes algorithm aliases (native, vectorized).
using aes128 = aes::algorithm<aes::k128>;
using aes256 = aes::algorithm<aes::k256>;

} // namespace system
} // namespace libbitcoin

#endif
