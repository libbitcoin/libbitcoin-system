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
#include <bitcoin/system/define.hpp>

// This is a maintainer file to force rebuild of /system only.

// define.hpp is the common include for /system.
// All non-system headers include define.hpp.
// System inclusions are chained as follows.

// version        : <generated>
// have           : version
// warnings       : have        [for all]
// boost          : warnings    [for suppression]
// exceptions     : boost
// types          : exceptions  [for boost]
// constants      : types
// literals       : constants   [for types]
// funclets       : literals    [for constants]
// typelets       : funclets
// constraints    : typelets
// define         : constraints [for all]

// Other directory common includes are not internally chained.
// Each headers include only its required common headers.

// /error         : define
// /unicode       : define
// /math          : define
// /data          : /math /unicode
// /endian        : /data
// /words         : /data
// /radix         : /words
// /serial        : /radix
// /crypto        : /radix {TODO: /endian -> /math}
// /stream        : /crypto /endian /error
// /chain         : /stream [/settings]
// settings       : /chain
// /machine       : /chain
// /config        : /chain
// /wallet        : /chain

// All /crypto/external can be made cpp/constexpr except the lax
// parser, which wraps libsecp256k1 (as with elliptic_curve).
// TODO: convert and move to /system/hash/ directory along with hash.hpp.

namespace libbitcoin {
namespace system {

} // namespace system
} // namespace libbitcoin
