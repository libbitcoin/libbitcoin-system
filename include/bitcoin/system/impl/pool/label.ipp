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
#ifndef LIBBITCOIN_SYSTEM_POOL_LABEL_IPP
#define LIBBITCOIN_SYSTEM_POOL_LABEL_IPP

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

template <typename Word>
constexpr void algorithm::initialize(const columns_t& columns) NOEXCEPT
{
    unlabel_<Word>(zero, columns.cells);
    seed_<Word>(zero, graph_t{ columns });
}

template <typename Word>
constexpr bool algorithm::propagate(const columns_t& columns) NOEXCEPT
{
    link_t row{};
    return propagate_<Word>(zero, row, graph_t{ columns });
}

// Inputs.
// ----------------------------------------------------------------------------

constexpr algorithm::link_t algorithm::advance(const graph_t& graph,
    link_t row, link_t input) NOEXCEPT
{
    const auto count = rows(graph);
    const auto end = [&](link_t at) NOEXCEPT
    {
        const auto slice = edges(graph, at);
        return ceilinged_add(slice.first, slice.count);
    };

    while (row < count && end(row) <= input)
        ++row;

    return row;
}

constexpr bool algorithm::holds(const graph_t& graph, link_t row,
    link_t input) NOEXCEPT
{
    if (row >= rows(graph))
        return false;

    const auto slice = edges(graph, row);
    const auto end = ceilinged_add(slice.first, slice.count);
    return input >= slice.first && input < end;
}

template <typename Owner>
constexpr void algorithm::seed(const graph_t& graph, link_t row, link_t input,
    const Owner& owning) NOEXCEPT
{
    if (cell(graph, row) != unlabeled)
        return;

    auto rivaled = false;
    each_rival(graph, input, [&](link_t other) NOEXCEPT
    {
        if (is_present(cell(graph, owning(other))))
            rivaled = true;
    });

    if (rivaled)
        set_cell(graph, row, row);
}

template <typename Owner, typename Lower>
constexpr bool algorithm::exchange(const graph_t& graph, link_t row,
    link_t input, const Owner& owning, const Lower& lower) NOEXCEPT
{
    const auto own = cell(graph, row);
    if (!is_present(own))
        return false;

    const auto up = parent(graph, input);
    const auto above = (up == none) ? unlabeled : cell(graph, up);
    auto low = std::min(own, above);
    each_rival(graph, input, [&](link_t other) NOEXCEPT
    {
        low = std::min(low, cell(graph, owning(other)));
    });

    // An unlabeled parent is not relevant, so takes no label from a child.
    const auto pull = low < own;
    const auto push = is_labeled(above) && low < above;

    if (pull)
        lower(row, low);

    if (push)
        lower(up, low);

    return pull || push;
}

constexpr void algorithm::seed(const graph_t& graph, link_t input) NOEXCEPT
{
    const auto owning = [&](link_t at) NOEXCEPT
    {
        return owner(graph, at);
    };

    seed(graph, owning(input), input, owning);
}

constexpr bool algorithm::exchange(const graph_t& graph, link_t row,
    link_t input) NOEXCEPT
{
    const auto owning = [&](link_t at) NOEXCEPT
    {
        return owner(graph, at);
    };

    const auto lower = [&](link_t to, link_t label) NOEXCEPT
    {
        set_cell(graph, to, label);
    };

    return exchange(graph, row, input, owning, lower);
}

// Lanes.
// ----------------------------------------------------------------------------

template <typename Word>
INLINE constexpr Word algorithm::active(Word own, Word parents, Word rivals,
    const link_t* cells) NOEXCEPT
{
    constexpr auto lane = bits<link_t>;
    const auto nones = f::broadcast<Word>(none);
    const auto bound = f::broadcast<Word>(unlabeled);
    const auto missing = f::eq<lane>(parents, nones);
    const auto found = native(f::gather(cells, f::andnot(missing, parents)));
    const auto up = f::select(missing, bound, found);
    const auto low = f::min<lane>(own, up);

    // Lanes order by minimum: min(a, b) == a is a <= b.
    const auto present = f::eq<lane>(f::min<lane>(own, bound), own);
    const auto parentless = f::eq<lane>(f::min<lane>(up, bound), bound);
    const auto pull = f::andnot(f::eq<lane>(low, own), present);
    const auto push = f::not_(f::or_(f::eq<lane>(low, up), parentless));
    const auto ring = f::andnot(f::eq<lane>(rivals, nones), present);
    return f::or_(f::or_(pull, push), ring);
}

template <typename Word>
constexpr void algorithm::unlabel_(size_t index,
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
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = native(load<Word>(&cells[index]));
                const auto least = f::min<lane>(word, bound);
                const auto present = f::eq<lane>(least, word);
                const auto reset = f::select(present, bound, word);

                store(&cells[index], native(reset));
            }
        }

        unlabel_<narrower<Word, link_t>>(index, cells);
    }
    else
    {
        for (; index < count; ++index)
            if (is_present(from_column(cells[index])))
                cells[index] = to_column(unlabeled);
    }
}

template <typename Word>
constexpr void algorithm::seed_(size_t index, const graph_t& graph) NOEXCEPT
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
                            seed(graph, possible_narrow_cast<link_t>(input));
            }
        }

        seed_<narrower<Word, link_t>>(index, graph);
    }
    else
    {
        for (; index < count; ++index)
            if (rivals[index] != none)
                seed(graph, possible_narrow_cast<link_t>(index));
    }
}

template <typename Word>
constexpr bool algorithm::propagate_(size_t index, link_t& row,
    const graph_t& graph) NOEXCEPT
{
    auto changed = false;
    const auto& table = graph.table;
    const auto count = table.rows.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lanes = link_lanes<Word>;
            const auto cells = table.cells.data();
            std_array<link_t, lanes> owners{};
            std_array<link_t, lanes> owns{};
            std_array<link_t, lanes> live{};

            for (; index + lanes <= count; index += lanes)
            {
                for (size_t lane{}; lane < lanes; ++lane)
                {
                    const auto at = index + lane;
                    const auto input = possible_narrow_cast<link_t>(at);
                    row = advance(graph, row, input);
                    owners[lane] = row;
                    const auto held = holds(graph, row, input);
                    owns[lane] = held ? cell(graph, row) : absent;
                }

                const auto own = load<Word>(owns.data());
                const auto parents = native(load<Word>(&table.rows[index]));
                const auto rivals = load<Word>(&table.rivals[index]);
                const auto work = active(own, parents, rivals, cells);
                if (!f::any(work))
                    continue;

                // A label lowered may be one that a later lane read before it.
                auto stale = false;
                store(live.data(), work);
                for (size_t lane{}; lane < lanes; ++lane)
                {
                    const auto at = index + lane;
                    const auto input = possible_narrow_cast<link_t>(at);
                    const auto held = stale ?
                        holds(graph, owners[lane], input) :
                        is_nonzero(live[lane]);

                    if (held && exchange(graph, owners[lane], input))
                        changed = stale = true;
                }
            }
        }

        using next = narrower<Word, link_t>;
        return propagate_<next>(index, row, graph) || changed;
    }
    else
    {
        for (; index < count; ++index)
        {
            const auto input = possible_narrow_cast<link_t>(index);
            row = advance(graph, row, input);
            if (holds(graph, row, input) && exchange(graph, row, input))
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
