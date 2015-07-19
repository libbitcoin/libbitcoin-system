/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHECKPOINT_HPP
#define LIBBITCOIN_CHECKPOINT_HPP

#include <cstddef>
#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {
namespace config {

class BC_API checkpoint_type
{
public:
    checkpoint_type();
    checkpoint_type(const std::string& value);
    checkpoint_type(const checkpoint_type& other);

    const bc::hash_digest& get_hash() const;
    const size_t get_height() const;

    friend std::istream& operator>>(std::istream& input,
        checkpoint_type& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const checkpoint_type& argument);

private:
    bc::hash_digest hash_;
    size_t height_;
};

} // namespace config
} // namespace libbitcoin

#endif
