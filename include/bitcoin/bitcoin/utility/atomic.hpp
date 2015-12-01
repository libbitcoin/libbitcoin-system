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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_ATOMIC_POINTER_HPP
#define LIBBITCOIN_ATOMIC_POINTER_HPP

#include <mutex>

namespace libbitcoin {

template <typename Type>
class atomic
{
public:

    /// Create an atomically-accessible default instance of the type.
    atomic()
    {
    }

    /// Create an atomically-accessible copied instance of the type.
    atomic(const Type& instance)
      : store(instance)
    {
    }

    Type load()
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock(mutex_);

        return instance_;
        ///////////////////////////////////////////////////////////////////////
    }

    void store(const Type& instance)
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock(mutex_);

        instance_ = instance;
        ///////////////////////////////////////////////////////////////////////
    }

private:
    Type instance_;
    std::mutex mutex_;
};

} // namespace libbitcoin

#endif
