/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_CURSOR_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_CURSOR_HPP

#include <utility>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A move-only row cursor for streaming captured signature rows into caller
/// storage. Opened per capture (script scope) with all rows preallocated by
/// the caller. Writes exactly `rows`, releasing caller state (e.g. store
/// transactor) automatically upon the final write. A default-constructed
/// (terminal) cursor signals allocation decline. An open cursor implies
/// pending rows; the matched script pattern guarantees the final sigop is
/// positionally last, so an opened cursor always completes.
template <typename Writer>
struct cursor
{
    using writer = std::function<Writer>;
    using closer = std::function<void()>;

    bool is_open() const NOEXCEPT
    {
        return !!done;
    }

    template <typename... Args>
    void write(Args&&... args) NOEXCEPT
    {
        BC_ASSERT(is_open() && (row < rows));

        // Since all rows allocation precedes this, the only possible failure
        // is non-recoverable in any case, propagated internal to put().
        /* bool */ put(std::forward<Args>(args)...);
        if (++row == rows)
            close();
    }

    /// Release caller state (idempotent).
    void close() NOEXCEPT
    {
        if (is_open())
        {
            done();
            done = {};
            put = {};
        }
    }

    writer put{};
    closer done{};
    size_t rows{};
    size_t row{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
