/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/select_outputs.hpp>

#include <algorithm>
#include <cstdint>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace wallet {

using namespace bc::chain;

void select_outputs::select(points_info& out, output_info::list unspent,
    uint64_t minimum_value, algorithm DEBUG_ONLY(option))
{
    out.change = 0;
    out.points.clear();

    if (unspent.empty())
        return;

    const auto below_minimum = [minimum_value](const output_info& out_info)
    {
        return out_info.value < minimum_value;
    };

    const auto lesser = [](const output_info& left, const output_info& right)
    {
        return left.value < right.value;
    };

    const auto greater = [](const output_info& left, const output_info& right)
    {
        return left.value > right.value;
    };

    const auto lesser_begin = unspent.begin();
    const auto lesser_end = std::partition(unspent.begin(), unspent.end(),
        below_minimum);

    const auto greater_begin = lesser_end;
    const auto greater_end = unspent.end();
    const auto minimum_greater = std::min_element(greater_begin, greater_end,
        lesser);

    if (minimum_greater != greater_end)
    {
        BITCOIN_ASSERT(minimum_greater->value >= minimum_value);
        out.change = minimum_greater->value - minimum_value;
        out.points.push_back(minimum_greater->point);
        return;
    }

    // Not found in greaters so try several lessers instead.
    // Sort descending, to use the fewest inputs possible.
    std::sort(lesser_begin, lesser_end, greater);

    for (auto it = lesser_begin; it != lesser_end; ++it)
    {
        BITCOIN_ASSERT(out.change <= max_uint64 - it->value);
        out.change += it->value;
        out.points.push_back(it->point);

        if (out.change >= minimum_value)
        {
            out.change -= minimum_value;
            return;
        }
    }

    out.change = 0;
    out.points.clear();
}

} // namspace wallet
} // namspace libbitcoin
