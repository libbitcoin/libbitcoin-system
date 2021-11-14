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
#ifndef LIBBITCOIN_SYSTEM_ERROR_ERROR_CATEGORY_HPP
#define LIBBITCOIN_SYSTEM_ERROR_ERROR_CATEGORY_HPP

#include <string>
#include <system_error>

namespace libbitcoin {
namespace system {
namespace error {

class error_category
  : public std::error_category
{
public:
    static const error_category singleton;

    virtual const char* name() const noexcept;
    virtual std::string message(int value) const noexcept;
    virtual std::error_condition default_error_condition(
        int value) const noexcept;

private:
    static const char* category_name;
};

} // namespace error
} // namespace system
} // namespace libbitcoin

#endif
