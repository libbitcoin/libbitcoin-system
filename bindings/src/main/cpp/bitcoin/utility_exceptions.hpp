/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_EXCEPTIONS_HPP
#define LIBBITCOIN__UTILITY_EXCEPTIONS_HPP

#include <bitcoin/bitcoin/utility/exceptions.hpp>

namespace libbitcoin {
namespace api {

class BC_API utility_end_of_stream
{
public:
	libbitcoin::end_of_stream getValue() {
        return value;
    }

    void setValue(libbitcoin::end_of_stream value) {
        this->value = value;
    }
private:
    libbitcoin::end_of_stream value;
};

} // namespace api
} // namespace libbitcoin

#endif
