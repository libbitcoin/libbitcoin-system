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
#include <bitcoin/system/pool/label.hpp>

#include <chrono>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/pool/algorithm.hpp>
#include <bitcoin/system/pool/report.hpp>
#include "../crypto/secp256k1/cuda/driver.hpp"

namespace libbitcoin {
namespace system {
namespace pool {

report_t label(const algorithm::columns_t& columns) NOEXCEPT
{
    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    const auto& cells = columns.cells;
    const auto& edges = columns.edges;
    const auto& rows = columns.rows;
    const auto& rivals = columns.rivals;
    report_t out{ cells.size() };

    out.device = secp256k1::cuda::label(out, cells, edges, rows, rivals);
    if (!out.device)
    {
        algorithm::initialize(columns);

        do
        {
            ++out.passes;
        }
        while (algorithm::propagate(columns));
    }

    out.time = clock::now() - start;
    return out;
}

} // namespace pool
} // namespace system
} // namespace libbitcoin
