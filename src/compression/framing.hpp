/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_SRC_COMPRESSION_FRAMING_HPP
#define LIBBITCOIN_SYSTEM_SRC_COMPRESSION_FRAMING_HPP

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Moves bytes of in from consumed to pending, until pending holds size.
inline bool gather(data_chunk& pending, size_t& consumed, const data_slice& in,
    size_t size) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    const auto count = std::min(size - pending.size(), in.size() - consumed);
    const auto begin = std::next(in.begin(), consumed);
    pending.insert(pending.end(), begin, std::next(begin, count));
    BC_POP_WARNING()

    consumed += count;
    return pending.size() == size;
}

/// The growth at which a decode of limited output pauses, and then fails.
constexpr size_t pause_of(size_t limit) NOEXCEPT
{
    return is_zero(limit) || limit == max_size_t ? zero : add1(limit);
}

/// The remaining growth of out from start before the limit (zero unlimited).
constexpr size_t remaining_of(size_t limit, size_t grown) NOEXCEPT
{
    return is_zero(limit) ? zero : limit - std::min(limit, grown);
}

} // namespace system
} // namespace libbitcoin

#endif
