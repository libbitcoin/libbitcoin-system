/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CONDITIONAL_STACK_HPP
#define LIBBITCOIN_CONDITIONAL_STACK_HPP

#include <vector>
#include <bitcoin/bitcoin/primitives/blockchain/operation.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

class conditional_stack
{
public:

    bool closed() const;

    bool has_failed_branches() const;

    void clear();

    void open(bool value);

    void else_();

    void close();

private:

    std::vector<bool> stack_;
};

}

#endif
