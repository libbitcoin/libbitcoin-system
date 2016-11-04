/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_CONDITIONAL_STACK_HPP
#define LIBBITCOIN_CHAIN_CONDITIONAL_STACK_HPP

#include <cstddef>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

class BC_API conditional_stack
{
public:
    conditional_stack(size_t initial_capacity);

    // This prevents inadvertent copy or move of the full evaluation context.
    conditional_stack& operator=(conditional_stack&& other) = delete;
    conditional_stack& operator=(const conditional_stack& other) = delete;

    /// Conditions.
    bool closed() const;
    bool succeeded() const;

    /// Operations.
    void open(bool value);
    void clear();
    void negate();
    void close();

private:
    std::vector<bool> stack_;
};

} // namespace chain
} // namespace libbitcoin

#endif
