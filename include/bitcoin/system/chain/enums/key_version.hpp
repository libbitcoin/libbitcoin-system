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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_KEY_VERSION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_KEY_VERSION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// For future extensions that define unknown public key types, making sure
/// signatures can't be moved from one key type to another [bip342].
enum class key_version : uint8_t
{
    /// Version of public keys in the tapscript signature opcode execution.
    tapscript = 0
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
