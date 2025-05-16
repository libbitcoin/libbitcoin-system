/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_EXTENSION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_EXTENSION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// An integer in range 0-127 used for indicating extensions are appended to 
/// the output of signature hash message [bip141].
enum class extension : uint8_t
{
    /// Undocumented, from Satoshi source.
    taproot = 0,

    /// We define the tapscript message extension ext to bip341 Common 
    /// Signature Message, indicated by extension flag of 1 [bip342].
    tapscript = 1
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
