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
#ifndef LIBBITCOIN_SYSTEM_ERROR_ERROR_CONDITION_HPP
#define LIBBITCOIN_SYSTEM_ERROR_ERROR_CONDITION_HPP

#include <system_error>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {
namespace error {

enum error_condition_t {};

}
}
}

namespace std {

// Make eligible for std::error_condition automatic conversions.
template <>
struct is_error_condition_enum<bc::system::error::error_condition_t>
  : public true_type
{
};

// Required by is_error_condition_enum, see: std::error_condition source.
std::error_condition make_error_condition(
    bc::system::error::error_t value) noexcept;

} // namespace std

#endif
