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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_VERIFY_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_VERIFY_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// TODO: rename.
namespace batched {
using link = data_array<3>;
using link_t = unsigned_type<sizeof(link)>;
using links_t = std::vector<link_t>;
constexpr link terminal{ 0xff, 0xff, 0xff };

/// True iff hardware (GPU) batch acceleration is compiled in.
BC_API bool compiled() NOEXCEPT;

/// True iff batch verification is hardware (GPU) accelerated.
BC_API bool accelerated() NOEXCEPT;

} // namespace batched
} // namespace system
} // namespace libbitcoin

#endif
