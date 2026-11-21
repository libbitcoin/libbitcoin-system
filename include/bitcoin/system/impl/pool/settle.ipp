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
#ifndef LIBBITCOIN_SYSTEM_POOL_SETTLE_IPP
#define LIBBITCOIN_SYSTEM_POOL_SETTLE_IPP

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

// Columns (whole table).
// ----------------------------------------------------------------------------
// The derived causes are recomputed from none, so absence only rises during
// the passes and each pass is monotone.

constexpr bool algorithm::is_clear(
    const std::span<const link_t>& cells) NOEXCEPT
{
    return std::ranges::none_of(cells, [](link_t value) NOEXCEPT
    {
        const auto cell = from_column(value);
        return is_boundary(cell) || is_absent(reduced(cell));
    });
}

template <typename Word>
constexpr void algorithm::reset(const columns_t& columns) NOEXCEPT
{
    reduce_<Word>(zero, columns.cells);
    contest_<Word>(zero, graph_t{ columns });
}

template <typename Word>
constexpr bool algorithm::descend(const columns_t& columns) NOEXCEPT
{
    link_t row{};
    return descend_<Word>(zero, row, graph_t{ columns });
}

// Inputs.
// ----------------------------------------------------------------------------

constexpr algorithm::link_t algorithm::reduced(link_t cell) NOEXCEPT
{
    return lowered(cell, derived);
}

template <typename Owner, typename Raise>
constexpr bool algorithm::contest(const graph_t& graph, link_t row,
    link_t input, const Owner& owning, const Raise& raise) NOEXCEPT
{
    const auto own = cell(graph, row);
    if (is_boundary(own) || (is_absent(own) && is_nonzero(bit_and(own,
        rivaled))))
        return false;

    auto confirmed = false;
    each_rival(graph, input, [&](link_t other) NOEXCEPT
    {
        if (is_boundary(cell(graph, owning(other))))
            confirmed = true;
    });

    if (confirmed)
        raise(row, raised(own, rivaled));

    return confirmed;
}

template <typename Raise>
constexpr bool algorithm::inherit(const graph_t& graph, link_t row,
    link_t input, const Raise& raise) NOEXCEPT
{
    const auto own = cell(graph, row);
    if (is_boundary(own) || (is_absent(own) && is_nonzero(bit_and(own,
        ancestor))))
        return false;

    const auto up = parent(graph, input);
    if (up == none || !is_absent(cell(graph, up)))
        return false;

    raise(row, raised(own, ancestor));
    return true;
}

constexpr bool algorithm::contest(const graph_t& graph, link_t input) NOEXCEPT
{
    const auto owning = [&](link_t at) NOEXCEPT
    {
        return owner(graph, at);
    };

    const auto raise = [&](link_t to, link_t value) NOEXCEPT
    {
        set_cell(graph, to, value);
    };

    return contest(graph, owning(input), input, owning, raise);
}

constexpr bool algorithm::inherit(const graph_t& graph, link_t row,
    link_t input) NOEXCEPT
{
    const auto raise = [&](link_t to, link_t value) NOEXCEPT
    {
        set_cell(graph, to, value);
    };

    return inherit(graph, row, input, raise);
}

// Lanes.
// ----------------------------------------------------------------------------

template <typename Word>
constexpr void algorithm::reduce_(size_t index,
    std::span<link_t> cells) NOEXCEPT
{
    const auto count = cells.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lane = bits<link_t>;
            constexpr auto lanes = link_lanes<Word>;
            const auto bound = f::broadcast<Word>(unlabeled);
            const auto bases = f::broadcast<Word>(absent);
            const auto bounds = f::broadcast<Word>(boundary);
            const auto stored = f::broadcast<Word>(bit_not(derived));
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = native(load<Word>(&cells[index]));
                const auto present = f::eq<lane>(f::min<lane>(word, bound),
                    word);

                const auto kept = f::or_(present, f::eq<lane>(word, bounds));
                const auto left = f::and_(word, stored);
                const auto none_left = f::eq<lane>(left, bases);
                const auto lowered = f::select(none_left, bound, left);

                store(&cells[index], native(f::select(kept, word, lowered)));
            }
        }

        reduce_<narrower<Word, link_t>>(index, cells);
    }
    else
    {
        for (; index < count; ++index)
            cells[index] = to_column(reduced(from_column(cells[index])));
    }
}

template <typename Word>
constexpr void algorithm::contest_(size_t index,
    const graph_t& graph) NOEXCEPT
{
    const auto& rivals = graph.table.rivals;
    const auto count = rivals.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lanes = link_lanes<Word>;
            const auto nones = f::broadcast<Word>(none);
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = load<Word>(&rivals[index]);
                const auto sole = f::eq<bits<link_t>>(word, nones);

                if (f::any(f::not_(sole)))
                    for (auto input = index; input < index + lanes; ++input)
                        if (rivals[input] != none)
                            contest(graph, possible_narrow_cast<link_t>(input));
            }
        }

        contest_<narrower<Word, link_t>>(index, graph);
    }
    else
    {
        for (; index < count; ++index)
            if (rivals[index] != none)
                contest(graph, possible_narrow_cast<link_t>(index));
    }
}

template <typename Word>
constexpr bool algorithm::descend_(size_t index, link_t& row,
    const graph_t& graph) NOEXCEPT
{
    auto changed = false;
    const auto& table = graph.table;
    const auto count = table.rows.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lane = bits<link_t>;
            constexpr auto lanes = link_lanes<Word>;
            const auto cells = table.cells.data();
            const auto nones = f::broadcast<Word>(none);
            const auto bound = f::broadcast<Word>(unlabeled);
            const auto bounds = f::broadcast<Word>(boundary);
            std_array<link_t, lanes> live{};

            for (; index + lanes <= count; index += lanes)
            {
                // Lanes order by minimum: min(a, b) == a is a <= b.
                const auto links = native(load<Word>(&table.rows[index]));
                const auto missing = f::eq<lane>(links, nones);
                const auto found = f::gather(cells, f::andnot(missing, links));
                const auto up = native(found);
                const auto above = f::eq<lane>(f::min<lane>(up, bound), up);
                const auto idle = f::or_(f::or_(above, missing),
                    f::eq<lane>(up, bounds));

                if (!f::any(f::not_(idle)))
                    continue;

                // A raise may be of a parent that a later lane read before it.
                auto stale = false;
                store(live.data(), f::not_(idle));
                for (size_t slot{}; slot < lanes; ++slot)
                {
                    if (!stale && is_zero(live[slot]))
                        continue;

                    const auto at = index + slot;
                    const auto input = possible_narrow_cast<link_t>(at);
                    row = advance(graph, row, input);
                    if (holds(graph, row, input) && inherit(graph, row, input))
                        changed = stale = true;
                }
            }
        }

        using next = narrower<Word, link_t>;
        return descend_<next>(index, row, graph) || changed;
    }
    else
    {
        for (; index < count; ++index)
        {
            const auto input = possible_narrow_cast<link_t>(index);
            row = advance(graph, row, input);
            if (holds(graph, row, input) && inherit(graph, row, input))
                changed = true;
        }

        return changed;
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
