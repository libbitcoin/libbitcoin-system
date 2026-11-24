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
#ifndef LIBBITCOIN_SYSTEM_POOL_SUFFICE_IPP
#define LIBBITCOIN_SYSTEM_POOL_SUFFICE_IPP

#include <span>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pool {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

// Contexts.
// ----------------------------------------------------------------------------

constexpr bool algorithm::is_sufficient(const context_t& stored,
    const context_t& current, uint32_t required) NOEXCEPT
{
    return bit_and(stored.flags, current.flags) == current.flags &&
        bit_and(stored.flags, required) == required &&
        stored.height <= current.height &&
        stored.median <= current.median;
}

// Columns (whole table).
// ----------------------------------------------------------------------------

template <typename Word>
constexpr size_t algorithm::suffice(const columns_t& columns,
    const context_t& context, uint32_t required) NOEXCEPT
{
    return suffice_<Word>(zero, columns, context, required);
}

// Rows.
// ----------------------------------------------------------------------------

constexpr bool algorithm::suffice(const columns_t& columns, link_t row,
    const context_t& context, uint32_t required) NOEXCEPT
{
    const auto own = from_column(columns.cells[row]);
    const auto gone = is_absent(own) && is_nonzero(bit_and(own, purged));
    if (gone || is_boundary(own))
        return false;

    const context_t stored
    {
        from_column(columns.flags[row]),
        from_column(columns.heights[row]),
        from_column(columns.medians[row])
    };

    const auto sufficient = is_sufficient(stored, context, required);
    const auto next = sufficient ? lowered(own, insufficient) :
        raised(own, insufficient);

    if (next != own)
        columns.cells[row] = to_column(next);

    return !sufficient;
}

// Lanes.
// ----------------------------------------------------------------------------

template <typename Word>
constexpr size_t algorithm::suffice_(size_t index, const columns_t& columns,
    const context_t& context, uint32_t required) NOEXCEPT
{
    size_t count{};
    const auto size = columns.cells.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lane = bits<link_t>;
            constexpr auto lanes = link_lanes<Word>;
            const auto zeros = f::broadcast<Word>(link_t{});
            const auto bound = f::broadcast<Word>(unlabeled);
            const auto bounds = f::broadcast<Word>(boundary);
            const auto cause = f::broadcast<Word>(insufficient);
            const auto needs = f::broadcast<Word>(required);
            const auto flags = f::broadcast<Word>(context.flags);
            const auto height = f::broadcast<Word>(context.height);
            const auto median = f::broadcast<Word>(context.median);

            for (; index + lanes <= size; index += lanes)
            {
                // Lanes order by minimum: min(a, b) == a is a <= b.
                const auto flag = native(load<Word>(&columns.flags[index]));
                const auto high = native(load<Word>(&columns.heights[index]));
                const auto time = native(load<Word>(&columns.medians[index]));
                const auto low = f::eq<lane>(f::min<lane>(high, height), high);
                const auto old = f::eq<lane>(f::min<lane>(time, median), time);
                const auto same = f::eq<lane>(f::and_(flag, flags), flags);
                const auto able = f::eq<lane>(f::and_(flag, needs), needs);
                const auto sufficient = f::and_(f::and_(same, able),
                    f::and_(low, old));

                // The low bits of a present cell are its label, not causes.
                const auto own = native(load<Word>(&columns.cells[index]));
                const auto here = f::eq<lane>(f::min<lane>(own, bound), own);
                const auto clear = f::eq<lane>(f::and_(own, cause), zeros);
                const auto idle = f::or_(f::eq<lane>(own, bounds),
                    f::and_(sufficient, f::or_(here, clear)));

                if (!f::any(f::not_(idle)))
                    continue;

                for (auto row = index; row < index + lanes; ++row)
                    if (suffice(columns, possible_narrow_cast<link_t>(row),
                        context, required))
                        ++count;
            }
        }

        using next = narrower<Word, link_t>;
        return count + suffice_<next>(index, columns, context, required);
    }
    else
    {
        for (; index < size; ++index)
            if (suffice(columns, possible_narrow_cast<link_t>(index), context,
                required))
                ++count;

        return count;
    }
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace pool
} // namespace system
} // namespace libbitcoin

#endif
