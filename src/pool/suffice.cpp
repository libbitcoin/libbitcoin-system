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
#include <bitcoin/system/pool/suffice.hpp>

#include <chrono>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/pool/algorithm.hpp>
#include <bitcoin/system/pool/report.hpp>
#include "../cuda/driver.hpp"

namespace libbitcoin {
namespace system {
namespace pool {

report_t suffice(size_t& out, const algorithm::columns_t& columns,
    const algorithm::context_t& context, uint32_t required) NOEXCEPT
{
    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    const auto& cells = columns.cells;
    const auto& flags = columns.flags;
    const auto& heights = columns.heights;
    const auto& medians = columns.medians;
    report_t report{ cells.size() };

    report.device = cuda::suffice(report, out, cells, flags,
        heights, medians, context, required);

    if (!report.device)
    {
        out = algorithm::suffice(columns, context, required);
        report.passes = one;
    }

    report.time = clock::now() - start;
    return report;
}

} // namespace pool
} // namespace system
} // namespace libbitcoin
