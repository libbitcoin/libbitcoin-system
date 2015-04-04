/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_TIMED_SECTION_HPP
#define LIBBITCOIN_TIMED_SECTION_HPP

#include <ctime>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/time.hpp>

namespace libbitcoin {

class timed_section
{
public:
    BC_API timed_section(
        const std::string& context, const std::string& section);
    BC_API ~timed_section();
    BC_API double elapsed() const;

private:
    const std::string context_, section_;
    timespec start_;
};

} // namespace libbitcoin

#endif
