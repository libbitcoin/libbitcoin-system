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
// Standard library includes only, as each slice pays for what it includes.
#include "generator.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {

constinit const std::array<precompute::word, precompute::slice_words>
generator_slice_15 = precompute::slice<15>();

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin
