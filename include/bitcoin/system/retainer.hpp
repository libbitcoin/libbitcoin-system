/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_RETAINER_HPP
#define LIBBITCOIN_SYSTEM_RETAINER_HPP

#include <memory>
#include <shared_mutex>
#include <bitcoin/system/arena.hpp>

namespace libbitcoin {

/// Shared lock object to inform allocator that memory may be freed.
class BC_API retainer final
{
public:
    using ptr = std::shared_ptr<retainer>;

    DELETE_COPY_MOVE_DESTRUCT(retainer);

    retainer() NOEXCEPT
      : shared_lock_{}
    {
    }

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    retainer(std::shared_mutex& mutex) NOEXCEPT
      : shared_lock_(mutex)
    {
    }
    BC_POP_WARNING()

private:
    // This is thread safe.
    std::shared_lock<std::shared_mutex> shared_lock_;
};

} // namespace libbitcoin

#endif
