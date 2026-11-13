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
#ifndef LIBBITCOIN_SYSTEM_POOL_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_POOL_ALGORITHM_IPP

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

// Cells.
// ----------------------------------------------------------------------------

constexpr bool algorithm::is_present(link_t cell) NOEXCEPT
{
    return cell < absent;
}

constexpr bool algorithm::is_labeled(link_t cell) NOEXCEPT
{
    return cell < unlabeled;
}

constexpr bool algorithm::is_boundary(link_t cell) NOEXCEPT
{
    return cell == boundary;
}

constexpr uint64_t algorithm::chargeable(const totals_t& totals,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    return std::max(possible_wide_cast<uint64_t>(totals.size),
        ceilinged_multiply(possible_wide_cast<uint64_t>(totals.sigops),
            possible_wide_cast<uint64_t>(bytes_per_sigop)));
}

// Candidates.
// ----------------------------------------------------------------------------

constexpr void algorithm::initialize(const graph_t& graph,
    const std::span<const link_t>& candidates) NOEXCEPT
{
    for (const auto row: candidates)
        cell(graph, row) = unlabeled;

    // Relevance closes over the candidates in at most count passes.
    for (auto changed = true; changed;)
    {
        changed = false;
        for (const auto row: candidates)
        {
            if (is_labeled(cell(graph, row)))
                continue;

            if (contested(graph, row) || descended(graph, row))
            {
                cell(graph, row) = row;
                changed = true;
            }
        }
    }
}

// Components (frontier).
// ----------------------------------------------------------------------------

constexpr bool algorithm::collect(component_t& out, const graph_t& graph,
    link_t row) NOEXCEPT
{
    out = {};
    const auto add = [&](link_t found) NOEXCEPT
    {
        if (index(out, found) != maximum_members)
            return true;

        if (out.count == maximum_members)
            return false;

        out.rows[out.count++] = found;
        return true;
    };

    if (!add(row))
        return false;

    for (size_t at{}; at < out.count; ++at)
    {
        const auto member = out.rows[at];
        const auto slice = edges(graph, member);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up != none && is_labeled(cell(graph, up)) && !add(up))
                return false;

            auto added = true;
            each_rival(graph, input, [&](link_t other) NOEXCEPT
            {
                const auto rival_row = owner(graph, other);
                if (is_present(cell(graph, rival_row)) && !add(rival_row))
                    added = false;
            });

            if (!added)
                return false;
        }

        auto added = true;
        each_child(graph, member, [&](link_t down) NOEXCEPT
        {
            if (is_present(cell(graph, down)) && !add(down))
                added = false;
        });

        if (!added)
            return false;
    }

    std::sort(out.rows.begin(), std::next(out.rows.begin(), out.count));
    return true;
}

constexpr bool algorithm::relate(component_t& out,
    const graph_t& graph) NOEXCEPT
{
    out.contests = zero;
    for (size_t member{}; member < out.count; ++member)
    {
        mask_t direct{};
        const auto row = out.rows[member];
        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = index(out, parent(graph, input));
            if (up != maximum_members)
                set_right_into(direct, up);

            auto clique = set_right(mask_t{}, member);
            each_rival(graph, input, [&](link_t other) NOEXCEPT
            {
                const auto rival_member = index(out, owner(graph, other));
                if (rival_member != maximum_members)
                    set_right_into(clique, rival_member);
            });

            if (is_one(ones_count(clique)))
                continue;

            const auto end = std::next(out.cliques.begin(), out.contests);
            if (std::find(out.cliques.begin(), end, clique) != end)
                continue;

            if (out.contests == maximum_members)
                return false;

            out.cliques[out.contests++] = clique;
        }

        out.ancestors[member] = direct;
    }

    // Ancestry closes within the component in at most count passes.
    for (auto changed = true; changed;)
    {
        changed = false;
        for (size_t member{}; member < out.count; ++member)
        {
            auto ancestors = out.ancestors[member];
            for (size_t up{}; up < out.count; ++up)
                if (get_right(ancestors, up))
                    ancestors |= out.ancestors[up];

            if (ancestors != out.ancestors[member])
            {
                out.ancestors[member] = ancestors;
                changed = true;
            }
        }
    }

    return true;
}

constexpr algorithm::mask_t algorithm::members(const component_t& component,
    const std::span<const link_t>& rows) NOEXCEPT
{
    mask_t out{};
    for (const auto row: rows)
    {
        const auto member = index(component, row);
        if (member != maximum_members)
            set_right_into(out, member);
    }

    return out;
}

inline bool algorithm::ancestry(uint64_t& out, const graph_t& graph,
    const component_t& component, uint32_t bytes_per_sigop,
    uint64_t limit) NOEXCEPT
{
    out = zero;
    std::vector<link_t> visited{};
    std::vector<link_t> pending{};
    const auto push = [&](link_t row) NOEXCEPT
    {
        if (index(component, row) != maximum_members ||
            std::ranges::find(visited, row) != visited.end())
            return;

        visited.push_back(row);
        pending.push_back(row);
    };

    const auto charge = [&](link_t row) NOEXCEPT
    {
        out = ceilinged_add(out, chargeable(totals(graph, row),
            bytes_per_sigop));

        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up != none && is_present(cell(graph, up)))
                push(up);
        }

        return out <= limit;
    };

    for (size_t member{}; member < component.count; ++member)
        if (!charge(component.rows[member]))
            return false;

    while (!pending.empty())
    {
        const auto row = pending.back();
        pending.pop_back();
        if (!charge(row))
            return false;
    }

    return true;
}

// Outcomes.
// ----------------------------------------------------------------------------

constexpr size_t algorithm::outcomes(const component_t& component) NOEXCEPT
{
    size_t out{ one };
    for (size_t contest{}; contest < component.contests; ++contest)
        out = ceilinged_multiply(out, ones_count(component.cliques[contest]));

    return out;
}

constexpr algorithm::mask_t algorithm::outcome(const component_t& component,
    size_t index) NOEXCEPT
{
    const auto lost = losers(component, index);
    auto out = unmask_right<mask_t>(component.count) & ~lost;
    for (size_t member{}; member < component.count; ++member)
        if (!is_zero(component.ancestors[member] & lost))
            out = set_right(out, member, false);

    return out;
}

template <typename Word>
constexpr void algorithm::evaluate(uint64_t& best, uint64_t& best_without,
    const std::span<const mask_t>& outcomes, mask_t candidate,
    const component_t& component, const graph_t& graph) NOEXCEPT
{
    best = zero;
    best_without = zero;
    const auto consider = [&](mask_t outcome, uint64_t fee) NOEXCEPT
    {
        best = std::max(best, fee);
        if (is_zero(outcome & candidate))
            best_without = std::max(best_without, fee);
    };

    const auto count = outcomes.size();
    size_t index{};

    if constexpr (is_extended<Word>)
    {
        constexpr auto lanes = mask_lanes<Word>;
        aligned_array<uint64_t, lanes> sums{};
        for (; index + lanes <= count; index += lanes)
        {
            store(sums.data(), fees<Word>(load<Word>(&outcomes[index]),
                component, graph));

            for (size_t lane{}; lane < lanes; ++lane)
                consider(outcomes[index + lane], sums[lane]);
        }
    }

    for (; index < count; ++index)
        consider(outcomes[index], fees<uint64_t>(outcomes[index], component,
            graph));
}

constexpr bool algorithm::covered(uint64_t best, uint64_t best_without,
    uint64_t candidate_bytes, uint64_t rate) NOEXCEPT
{
    // Compared in satoshis per virtual kilobyte.
    const auto added = floored_subtract(best, best_without);
    return ceilinged_multiply(added, 1'000_u64) >=
        ceilinged_multiply(rate, candidate_bytes);
}

// Transitions (frontier, table columns).
// ----------------------------------------------------------------------------

constexpr void algorithm::relabel(const columns_t& columns,
    const component_t& component) NOEXCEPT
{
    // Rows are sorted by collect, so the label is the first.
    const auto label = component.rows.front();
    for (size_t member{}; member < component.count; ++member)
        columns.cells[component.rows[member]] = label;
}

inline void algorithm::exclude(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    std::vector<link_t> pending{ row };
    columns.cells[row] = absent;

    while (!pending.empty())
    {
        const auto next = pending.back();
        pending.pop_back();
        each_child(graph, next, [&](link_t down) NOEXCEPT
        {
            if (is_present(columns.cells[down]))
            {
                columns.cells[down] = absent;
                pending.push_back(down);
            }
        });
    }
}

inline void algorithm::admit(const columns_t& columns, link_t row) NOEXCEPT
{
    const std_array<link_t, one> rows{ { row } };
    columns.cells[row] = unlabeled;
    reform(columns, rows);
}

inline void algorithm::confirm(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    component_t former{};
    const auto labeled = is_labeled(columns.cells[row]) &&
        collect(former, graph, row);

    columns.cells[row] = boundary;
    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            const auto rival_row = owner(graph, other);
            if (is_present(columns.cells[rival_row]))
                exclude(columns, rival_row);
        });
    }

    if (!labeled)
        return;

    std::vector<link_t> rest{};
    for (size_t member{}; member < former.count; ++member)
        if (is_present(columns.cells[former.rows[member]]))
            rest.push_back(former.rows[member]);

    reform(columns, rest);
}

inline void algorithm::unconfirm(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    std::vector<link_t> affected{ row };
    columns.cells[row] = unlabeled;

    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            restore(affected, columns, owner(graph, other));
        });
    }

    reform(columns, affected);
}

inline void algorithm::revive(const columns_t& columns, link_t row) NOEXCEPT
{
    std::vector<link_t> affected{};
    restore(affected, columns, row);
    reform(columns, affected);
}

// Columns (whole table).
// ----------------------------------------------------------------------------

template <typename Word>
constexpr void algorithm::gather(std::span<link_t> out,
    const std::span<const link_t>& values,
    const std::span<const link_t>& indexes) NOEXCEPT
{
    const auto count = indexes.size();
    size_t index{};

    if constexpr (is_extended<Word>)
    {
        constexpr auto lanes = link_lanes<Word>;
        const auto nones = f::broadcast<Word>(none);
        for (; index + lanes <= count; index += lanes)
        {
            const auto at = load<Word>(&indexes[index]);
            const auto missing = f::eq<bits<link_t>>(at, nones);
            const auto found = f::gather(values.data(), f::andnot(missing, at));
            store(&out[index], f::select(missing, nones, found));
        }
    }

    for (; index < count; ++index)
        out[index] = (indexes[index] == none) ? none : values[indexes[index]];
}

constexpr void algorithm::initialize(const columns_t& columns) NOEXCEPT
{
    const graph_t graph{ columns };
    const auto count = columns.cells.size();
    for (size_t row{}; row < count; ++row)
    {
        auto& value = columns.cells[row];
        if (is_present(value))
            value = contested(graph, possible_narrow_cast<link_t>(row)) ?
                possible_narrow_cast<link_t>(row) : unlabeled;
    }
}

template <typename Word>
constexpr bool algorithm::propagate(const columns_t& columns,
    std::span<link_t> scratch) NOEXCEPT
{
    const graph_t graph{ columns };
    gather<Word>(scratch, columns.cells, columns.rows);

    auto changed = false;
    const auto count = columns.cells.size();
    for (size_t at{}; at < count; ++at)
    {
        const auto row = possible_narrow_cast<link_t>(at);
        const auto own = columns.cells[row];
        if (!is_present(own))
            continue;

        auto down = unlabeled;
        const auto slice = columns.edges[row];
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            down = std::min(down, scratch[input]);
            each_rival(graph, input, [&](link_t other) NOEXCEPT
            {
                down = std::min(down, columns.cells[owner(graph, other)]);
            });
        }

        // An uncontested row without a relevant parent is irrelevant.
        if (!is_labeled(own) && !is_labeled(down))
            continue;

        auto label = std::min(own, down);
        each_child(graph, row, [&](link_t child) NOEXCEPT
        {
            label = std::min(label, columns.cells[child]);
        });

        if (label != own)
        {
            columns.cells[row] = label;
            changed = true;
        }
    }

    return changed;
}

template <typename Word>
constexpr bool algorithm::collect(component_t& out, link_t label,
    const std::span<const link_t>& cells) NOEXCEPT
{
    out = {};
    const auto rows = cells.size();
    const auto add = [&](size_t row) NOEXCEPT
    {
        if (out.count == maximum_members)
            return false;

        out.rows[out.count++] = possible_narrow_cast<link_t>(row);
        return true;
    };

    size_t row{};
    if constexpr (is_extended<Word>)
    {
        constexpr auto lanes = link_lanes<Word>;
        const auto wanted = f::broadcast<Word>(label);
        for (; row + lanes <= rows; row += lanes)
        {
            const auto found = f::eq<bits<link_t>>(load<Word>(&cells[row]),
                wanted);

            if (f::any(found))
                for (auto lane = row; lane < row + lanes; ++lane)
                    if (cells[lane] == label && !add(lane))
                        return false;
        }
    }

    for (; row < rows; ++row)
        if (cells[row] == label && !add(row))
            return false;

    return true;
}

template <typename Word>
inline void algorithm::find(std::vector<link_t>& out,
    const std::span<const link_t>& values, link_t value) NOEXCEPT
{
    out.clear();
    const auto count = values.size();
    size_t index{};

    if constexpr (is_extended<Word>)
    {
        constexpr auto lanes = link_lanes<Word>;
        const auto wanted = f::broadcast<Word>(value);
        for (; index + lanes <= count; index += lanes)
        {
            const auto found = f::eq<bits<link_t>>(load<Word>(&values[index]),
                wanted);

            if (f::any(found))
                for (auto lane = index; lane < index + lanes; ++lane)
                    if (values[lane] == value)
                        out.push_back(possible_narrow_cast<link_t>(lane));
        }
    }

    for (; index < count; ++index)
        if (values[index] == value)
            out.push_back(possible_narrow_cast<link_t>(index));
}

// Internals.
// ----------------------------------------------------------------------------

constexpr size_t algorithm::rows(const graph_t& graph) NOEXCEPT
{
    return graph.table.cells.size();
}

constexpr algorithm::link_t& algorithm::cell(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return row < count ? graph.table.cells[row] :
        graph.extra.cells[row - count];
}

constexpr algorithm::edges_t algorithm::edges(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return row < count ? graph.table.edges[row] :
        graph.extra.edges[row - count];
}

constexpr const algorithm::totals_t& algorithm::totals(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return row < count ? graph.table.totals[row] :
        graph.extra.totals[row - count];
}

constexpr algorithm::link_t algorithm::child(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return row < count ? graph.table.children[row] :
        graph.extra.children[row - count];
}

constexpr algorithm::link_t algorithm::parent(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return input < count ? graph.table.rows[input] :
        graph.extra.rows[input - count];
}

constexpr algorithm::link_t algorithm::owner(const graph_t& graph,
    link_t input) NOEXCEPT
{
    // The last row whose first input is not beyond the input.
    const auto count = graph.table.rows.size();
    const auto& edges = input < count ? graph.table.edges : graph.extra.edges;
    const auto after = std::upper_bound(edges.begin(), edges.end(), input,
        [](link_t at, const edges_t& edge) NOEXCEPT
        {
            return at < edge.first;
        });

    const auto base = input < count ? zero : rows(graph);
    return possible_narrow_cast<link_t>(base +
        std::distance(edges.begin(), std::prev(after)));
}

constexpr algorithm::link_t algorithm::sibling(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return input < count ? graph.table.siblings[input] :
        graph.extra.siblings[input - count];
}

constexpr algorithm::link_t algorithm::rival(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return input < count ? graph.table.rivals[input] :
        graph.extra.rivals[input - count];
}

template <typename Visitor>
constexpr void algorithm::each_child(const graph_t& graph, link_t row,
    const Visitor& visit) NOEXCEPT
{
    for (auto input = child(graph, row); input != none;
        input = sibling(graph, input))
        visit(owner(graph, input));

    if (row < rows(graph))
        for (const auto& bridge: graph.children)
            if (bridge.from == row)
                visit(owner(graph, bridge.to));
}

template <typename Visitor>
constexpr void algorithm::each_rival(const graph_t& graph, link_t input,
    const Visitor& visit) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    std::vector<link_t> seen{ input };
    const auto step = [&](link_t next) NOEXCEPT
    {
        if (next == none || std::ranges::find(seen, next) != seen.end())
            return false;

        seen.push_back(next);
        return true;
    };

    // A table input bridges to the candidate inputs spending its point.
    for (auto at = input;;)
    {
        if (at < count)
            for (const auto& bridge: graph.rivals)
                if (bridge.from == at && step(bridge.to))
                    visit(bridge.to);

        at = rival(graph, at);
        if (!step(at))
            return;

        visit(at);
    }
}

constexpr bool algorithm::contested(const graph_t& graph, link_t row) NOEXCEPT
{
    auto out = false;
    const auto slice = edges(graph, row);
    for (auto input = slice.first; !out && input < slice.first + slice.count;
        ++input)
    {
        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            if (is_present(cell(graph, owner(graph, other))))
                out = true;
        });
    }

    return out;
}

constexpr bool algorithm::descended(const graph_t& graph, link_t row) NOEXCEPT
{
    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        const auto up = parent(graph, input);
        if (up != none && is_labeled(cell(graph, up)))
            return true;
    }

    return false;
}

constexpr bool algorithm::confirmable(const graph_t& graph,
    link_t row) NOEXCEPT
{
    auto out = true;
    const auto slice = edges(graph, row);
    for (auto input = slice.first; out && input < slice.first + slice.count;
        ++input)
    {
        const auto up = parent(graph, input);
        if (up != none && cell(graph, up) == absent)
            return false;

        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            if (is_boundary(cell(graph, owner(graph, other))))
                out = false;
        });
    }

    return out;
}

constexpr size_t algorithm::index(const component_t& component,
    link_t row) NOEXCEPT
{
    for (size_t member{}; member < component.count; ++member)
        if (component.rows[member] == row)
            return member;

    return maximum_members;
}

constexpr algorithm::mask_t algorithm::losers(const component_t& component,
    size_t index) NOEXCEPT
{
    mask_t out{};
    for (size_t contest{}; contest < component.contests; ++contest)
    {
        const auto clique = component.cliques[contest];
        const auto size = ones_count(clique);
        const auto digit = index % size;
        index /= size;

        // The winner is the clique's digit-th set bit.
        auto winner = clique;
        for (size_t cleared{}; cleared < digit; ++cleared)
            winner &= sub1(winner);

        winner &= (mask_t{} - winner);
        out |= clique & ~winner;
    }

    return out;
}

inline void algorithm::restore(std::vector<link_t>& out,
    const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    if (columns.cells[row] != absent || !confirmable(graph, row))
        return;

    columns.cells[row] = unlabeled;
    out.push_back(row);
    each_child(graph, row, [&](link_t down) NOEXCEPT
    {
        restore(out, columns, down);
    });
}

inline void algorithm::reform(const columns_t& columns,
    const std::span<const link_t>& rows) NOEXCEPT
{
    const graph_t graph{ columns };
    for (const auto row: rows)
        if (is_present(columns.cells[row]))
            columns.cells[row] = unlabeled;

    // Relevance closes over the rows in at most count passes.
    for (auto changed = true; changed;)
    {
        changed = false;
        for (const auto row: rows)
        {
            if (!is_present(columns.cells[row]) ||
                is_labeled(columns.cells[row]))
                continue;

            if (contested(graph, row) || descended(graph, row))
            {
                columns.cells[row] = row;
                changed = true;
            }
        }
    }

    for (const auto row: rows)
    {
        // A row relabeled by a previous component carries a lower label.
        if (columns.cells[row] != row)
            continue;

        component_t component{};
        if (collect(component, graph, row))
            relabel(columns, component);
    }
}

template <typename Word>
INLINE constexpr Word algorithm::fees(Word outcomes,
    const component_t& component, const graph_t& graph) NOEXCEPT
{
    constexpr auto lane = bits<mask_t>;
    auto sum = f::broadcast<Word>(mask_t{});
    for (size_t member{}; member < component.count; ++member)
    {
        const auto bit = f::broadcast<Word>(set_right(mask_t{}, member));
        const auto fee = f::broadcast<Word>(totals(graph,
            component.rows[member]).fee);
        const auto selected = f::eq<lane>(f::and_(outcomes, bit), bit);
        sum = f::add<lane>(sum, f::and_(selected, fee));
    }

    return sum;
}

template <typename Word, typename Integral>
INLINE constexpr Word algorithm::load(const Integral* at) NOEXCEPT
{
    if constexpr (is_extended<Word>)
        return f::load<Word>(pointer_cast<const uint8_t>(at));
    else
        return *at;
}

template <typename Word, typename Integral>
INLINE constexpr void algorithm::store(Integral* at, Word word) NOEXCEPT
{
    if constexpr (is_extended<Word>)
        f::store(pointer_cast<uint8_t>(at), word);
    else
        *at = word;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace pool
} // namespace system
} // namespace libbitcoin

#endif
