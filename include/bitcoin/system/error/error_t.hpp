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
#ifndef LIBBITCOIN_SYSTEM_ERROR_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_ERROR_T_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

// Zero-valued code is the only one producing false in std::error_code bool
// operator override (false, no error). Each enumeration has independent success
// code to produce same type for ternaries. Codes of distinct enumerations can
// be assigned to each other as all are std::system type. However ternary with
// two different enums cannot assign/return to same code instance, as compiler
// cannot know which type to promote. Common success code would make this
// tedious, and explicit success code for given category improves consistency as
// source of propagated success can be extracted.

namespace libbitcoin {
namespace system {
namespace error {

enum error_t
{
    success = 0,
    not_found,
    not_implemented,

    // no serialization, used for test
    error_last
};

DECLARE_ERROR_T_CODE_CATEGORY(error, BC_API);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::error)

#endif
