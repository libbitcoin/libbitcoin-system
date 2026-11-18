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
#ifndef LIBBITCOIN_SYSTEM_POOL_REPORT_HPP
#define LIBBITCOIN_SYSTEM_POOL_REPORT_HPP

#include <chrono>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace pool {

/// What a scan of the pool columns did.
struct report_t
{
    using duration = std::chrono::nanoseconds;

    /// Rows scanned, and passes over them.
    size_t rows{};
    size_t passes{};

    /// Time of the scan, and of its transfer to and from the device.
    duration time{};
    duration transfer{};

    /// The scan ran on the device.
    bool device{};
};

} // namespace pool
} // namespace system
} // namespace libbitcoin

#endif
