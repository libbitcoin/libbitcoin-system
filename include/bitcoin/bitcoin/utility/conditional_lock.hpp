/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CONDITIONAL_LOCK_HPP
#define LIBBITCOIN_CONDITIONAL_LOCK_HPP

#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

class BC_API conditional_lock
{
public:
    /// Conditional lock using specified mutex pointer.
    conditional_lock(std::shared_ptr<shared_mutex> mutex_ptr);

    /// Unlock.
    ~conditional_lock();

private:
    const std::shared_ptr<shared_mutex> mutex_ptr_;
};

} // namespace libbitcoin

#endif
