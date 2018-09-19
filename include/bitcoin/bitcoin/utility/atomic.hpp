/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_ATOMIC_POINTER_HPP
#define LIBBITCOIN_ATOMIC_POINTER_HPP

#include <type_traits>
#include <utility>
#include <bitcoin/bitcoin/utility/thread.hpp>

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
      : instance_(instance)
    {
    }

    /// Create an atomically-accessible moved instance of the type.
    atomic(Type&& instance)
      : instance_(std::forward<Type>(instance))
    {
    }

    Type load() const
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        shared_lock lock(mutex_);

        return instance_;
        ///////////////////////////////////////////////////////////////////////
    }

    void store(const Type& instance)
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        unique_lock lock(mutex_);

        instance_ = instance;
        ///////////////////////////////////////////////////////////////////////
    }

    void store(Type&& instance)
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        unique_lock lock(mutex_);

        instance_ = std::forward<Type>(instance);
        ///////////////////////////////////////////////////////////////////////
    }

private:
    typedef typename std::decay<Type>::type decay_type;

    decay_type instance_;
    mutable shared_mutex mutex_;
};

} // namespace libbitcoin

#endif
