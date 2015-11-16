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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/select_outputs.hpp>

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace wallet {

select_outputs_result select_outputs(output_info_list unspent,
    uint64_t min_value, select_outputs_algorithm DEBUG_ONLY(algorithm))
{
    // Just one default implementation for now.
    // Consider a switch case with greedy_select_outputs(min_value) etc.
    // if this is ever extended with more algorithms.
    BITCOIN_ASSERT(algorithm == select_outputs_algorithm::greedy);

    if (unspent.empty())
        return select_outputs_result();

    const auto less_than_min_value = [min_value](const output_info& out_info)
    {
        return out_info.value < min_value;
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
        less_than_min_value);

    const auto greater_begin = lesser_end;
    const auto greater_end = unspent.end();
    const auto min_greater = std::min_element(greater_begin, greater_end,
        lesser);

    select_outputs_result result;
    if (min_greater != greater_end)
    {
        result.change = min_greater->value - min_value;
        result.points.push_back(min_greater->point);
        return result;
    }

    // Not found in greaters so try several lessers instead. Rearrange from
    // biggest to smallest. We want to use the fewest inputs possible.
    std::sort(lesser_begin, lesser_end, greater);

    uint64_t accumulator = 0;
    for (auto it = lesser_begin; it != lesser_end; ++it)
    {
        result.points.push_back(it->point);
        accumulator += it->value;
        if (accumulator >= min_value)
        {
            result.change = accumulator - min_value;
            return result;
        }
    }

    return select_outputs_result();
}

} // namspace wallet
} // namspace libbitcoin
