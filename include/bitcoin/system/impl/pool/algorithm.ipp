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

#include <ranges>
#include <span>
#include <unordered_set>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
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

constexpr bool algorithm::is_absent(link_t cell) NOEXCEPT
{
    return cell >= absent && cell < boundary;
}

constexpr algorithm::link_t algorithm::raised(link_t cell,
    link_t cause) NOEXCEPT
{
    return bit_or(is_absent(cell) ? cell : absent, cause);
}

constexpr algorithm::link_t algorithm::lowered(link_t cell,
    link_t cause) NOEXCEPT
{
    if (!is_absent(cell))
        return cell;

    const auto left = bit_and(cell, bit_not(cause));
    return left == absent ? unlabeled : left;
}

// Sigop cost at bytes per sigop is weight.
constexpr uint64_t algorithm::chargeable(const totals_t& totals,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    const auto sigop = ceilinged_multiply<uint64_t>(totals.sigops, bytes_per_sigop);
    return greater(totals.weight, sigop);
}

constexpr uint64_t algorithm::chargeable(mask_t outcome,
    const component_t& component, const graph_t& graph,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    uint64_t bytes{};
    for (size_t member{}; member < component.count; ++member)
    {
        if (!get_right(outcome, member))
            continue;

        const auto row = totals(graph, component.rows[member]);
        bytes = ceilinged_add(bytes, chargeable(row, bytes_per_sigop));
    }

    return bytes;
}

constexpr uint64_t algorithm::fee_rate(const totals_t& totals,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    return fee_rate(totals.fee, chargeable(totals, bytes_per_sigop));
}

constexpr uint64_t algorithm::fee_rate(uint64_t fee, uint64_t weight) NOEXCEPT
{
    return ceilinged_multiply(fee, per_weight) / greater(weight, 1_u64);
}

// Column values.
// ----------------------------------------------------------------------------

constexpr algorithm::link_t algorithm::to_column(link_t value) NOEXCEPT
{
    return native_to_little_end(value);
}

constexpr algorithm::edges_t algorithm::to_column(
    const edges_t& value) NOEXCEPT
{
    return { to_column(value.first), to_column(value.count) };
}

constexpr algorithm::totals_t algorithm::to_column(
    const totals_t& value) NOEXCEPT
{
    return
    {
        native_to_little_end(value.fee),
        native_to_little_end(value.weight),
        native_to_little_end(value.sigops)
    };
}

constexpr algorithm::link_t algorithm::from_column(link_t value) NOEXCEPT
{
    return native_from_little_end(value);
}

constexpr algorithm::edges_t algorithm::from_column(
    const edges_t& value) NOEXCEPT
{
    return { from_column(value.first), from_column(value.count) };
}

constexpr algorithm::totals_t algorithm::from_column(
    const totals_t& value) NOEXCEPT
{
    return
    {
        native_from_little_end(value.fee),
        native_from_little_end(value.weight),
        native_from_little_end(value.sigops)
    };
}

// Candidates.
// ----------------------------------------------------------------------------

constexpr void algorithm::initialize(const graph_t& graph,
    const std::span<const link_t>& candidates) NOEXCEPT
{
    for (const auto row: candidates)
        set_cell(graph, row, unlabeled);

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
                set_cell(graph, row, row);
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
        if (!each_member(graph, out.rows[at], add))
            return false;

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

inline bool algorithm::ancestry(totals_t& out, const graph_t& graph,
    const component_t& component, const totals_t& limit) NOEXCEPT
{
    out = {};
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
        const auto& add = totals(graph, row);
        out.fee = ceilinged_add(out.fee, add.fee);
        out.weight = ceilinged_add(out.weight, add.weight);
        out.sigops = ceilinged_add(out.sigops, add.sigops);

        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up != none && is_present(cell(graph, up)))
                push(up);
        }

        return out.weight <= limit.weight && out.sigops <= limit.sigops;
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

template <typename Word>
inline bool algorithm::family(uint64_t& fee, uint64_t& bytes,
    const graph_t& graph, const std::span<const link_t>& rows,
    uint64_t limit, uint64_t rate, uint32_t bytes_per_sigop,
    size_t maximum_outcomes) NOEXCEPT
{
    fee = 0_u64;
    bytes = 0_u64;
    std::vector<link_t> found{};
    std::unordered_set<link_t> visited{};
    const auto push = [&](link_t row) NOEXCEPT
    {
        if (row == none || !is_present(cell(graph, row)) ||
            !visited.insert(row).second)
            return true;

        found.push_back(row);
        const auto size = chargeable(totals(graph, row), bytes_per_sigop);
        bytes = ceilinged_add(bytes, size);
        return bytes <= limit;
    };

    for (const auto row: rows)
        if (!push(row))
            return false;

    for (size_t at{}; at < found.size(); ++at)
    {
        auto within = true;
        const auto row = found[at];
        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            within = within && push(parent(graph, input));
            each_rival(graph, input, [&](link_t other) NOEXCEPT
            {
                within = within && push(owner(graph, other));
            });
        }

        each_child(graph, row, [&](link_t down) NOEXCEPT
        {
            within = within && push(down);
        });

        if (!within)
            return false;
    }

    // Each component pays its best outcome, then each other row its fee.
    std::vector<mask_t> masks{};
    std::unordered_set<link_t> resolved{};
    for (const auto row: found)
    {
        if (!is_labeled(cell(graph, row)) || resolved.contains(row))
            continue;

        component_t component{};
        if (!collect(component, graph, row) || !relate(component, graph))
            return false;

        leaves(masks, component, graph, maximum_outcomes, rate,
            bytes_per_sigop);
        if (masks.empty())
            return false;

        fee = ceilinged_add(fee, evaluate<Word>(masks, component, graph));
        for (size_t member{}; member < component.count; ++member)
            resolved.insert(component.rows[member]);
    }

    for (const auto row: found)
        if (!resolved.contains(row))
            fee = ceilinged_add(fee, totals(graph, row).fee);

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
    auto out = bit_and(unmask_right<mask_t>(component.count), bit_not(lost));
    for (size_t member{}; member < component.count; ++member)
        if (is_nonzero(bit_and(component.ancestors[member], lost)))
            out = set_right(out, member, false);

    return out;
}

template <typename Word>
constexpr uint64_t algorithm::evaluate(const std::span<const mask_t>& outcomes,
    const component_t& component, const graph_t& graph) NOEXCEPT
{
    uint64_t best{};
    each_fee<Word>(zero, outcomes, component, graph,
        [&](size_t, uint64_t fee) NOEXCEPT
        {
            best = std::max(best, fee);
        });

    return best;
}

inline void algorithm::leaves(std::vector<mask_t>& out,
    const component_t& component, const graph_t& graph, size_t limit,
    uint64_t rate, uint32_t bytes_per_sigop) NOEXCEPT
{
    out.clear();
    if (is_zero(limit))
        return;

    const auto all = unmask_right<mask_t>(component.count);
    const auto contests = component.contests;
    if (is_zero(contests))
    {
        out.push_back(all);
        return;
    }

    std_array<mask_t, maximum_members> below{};
    for (size_t member{}; member < component.count; ++member)
        for (size_t up{}; up < component.count; ++up)
            if (get_right(component.ancestors[member], up))
                set_right_into(below[up], member);

    // Losers and their descendants join the dead.
    const auto kill = [&](mask_t dead, mask_t losers) NOEXCEPT
    {
        dead |= losers;
        for (; is_nonzero(losers); losers &= sub1(losers))
            dead |= below[right_zeros(losers)];

        return dead;
    };

    // Dead winners of a clique yield one outcome, so only the first is tried.
    std_array<mask_t, add1(maximum_members)> deads{};
    std_array<mask_t, add1(maximum_members)> rests{};
    std_array<bool, add1(maximum_members)> spent{};
    size_t level{};
    rests[level] = component.cliques[level];
    while (true)
    {
        if (level == contests)
        {
            if (out.size() == limit)
            {
                out.push_back(greedy(component, graph, rate, bytes_per_sigop));
                return;
            }

            out.push_back(bit_and(all, bit_not(deads[level])));
            --level;
            continue;
        }

        const auto clique = component.cliques[level];
        auto& rest = rests[level];
        auto descend = false;
        while (!descend && is_nonzero(rest))
        {
            const auto winner = bit_and(rest, twos_complement(rest));
            rest &= sub1(rest);
            if (is_nonzero(bit_and(winner, deads[level])))
            {
                if (spent[level])
                    continue;

                spent[level] = true;
            }

            deads[add1(level)] = kill(deads[level],
                bit_and(clique, bit_not(winner)));
            descend = true;
        }

        if (descend)
        {
            ++level;
            if (level < contests)
            {
                rests[level] = component.cliques[level];
                spent[level] = false;
            }

            continue;
        }

        if (is_zero(level))
            return;

        --level;
    }
}

constexpr uint64_t algorithm::chargeable(const component_t& component,
    const graph_t& graph, uint32_t bytes_per_sigop) NOEXCEPT
{
    uint64_t bytes{};
    for (size_t member{}; member < component.count; ++member)
    {
        const auto row = totals(graph, component.rows[member]);
        bytes = ceilinged_add(bytes, chargeable(row, bytes_per_sigop));
    }

    return bytes;
}

constexpr bool algorithm::covered(uint64_t fee, uint64_t weight,
    uint64_t rate) NOEXCEPT
{
    return ceilinged_multiply(fee, per_weight) >=
        ceilinged_multiply(rate, weight);
}

// Transitions (frontier, table columns).
// ----------------------------------------------------------------------------

constexpr void algorithm::relabel(const columns_t& columns,
    const component_t& component) NOEXCEPT
{
    // Rows are sorted by collect, so the label is the first.
    const auto label = to_column(component.rows.front());
    for (size_t member{}; member < component.count; ++member)
        columns.cells[component.rows[member]] = label;
}

inline bool algorithm::admit(const columns_t& columns, link_t row,
    bool conflict) NOEXCEPT
{
    walk_t walk{};
    const graph_t graph{ columns };
    set_cell(graph, row, conflict ? bit_or(absent, conflicted) : unlabeled);

    // Its children are the rows adopted by it.
    walk.affected.push_back(row);
    walk.pending.push_back(row);
    each_child(graph, row, [&](link_t down) NOEXCEPT
    {
        walk.pending.push_back(down);
    });

    return spread(walk, columns);
}

inline bool algorithm::confirm(const columns_t& columns,
    const std::span<const link_t>& rows,
    const std::span<const link_t>& conflicts) NOEXCEPT
{
    walk_t walk{};
    const graph_t graph{ columns };
    for (const auto row: rows)
        change(walk, columns, row, boundary);

    for (const auto row: conflicts)
        if (!is_boundary(cell(graph, row)))
            change(walk, columns, row, raised(cell(graph, row), conflicted));

    return spread(walk, columns);
}

inline bool algorithm::unconfirm(const columns_t& columns,
    const std::span<const link_t>& rows,
    const std::span<const link_t>& conflicts) NOEXCEPT
{
    walk_t walk{};
    const graph_t graph{ columns };
    for (const auto row: rows)
    {
        if (!is_boundary(cell(graph, row)))
            continue;

        change(walk, columns, row, unlabeled);
        walk.pending.push_back(row);
    }

    for (const auto row: conflicts)
        change(walk, columns, row, lowered(cell(graph, row), conflicted));

    return spread(walk, columns);
}

// Template (frontier, table columns).
// ----------------------------------------------------------------------------

inline void algorithm::owners(std::vector<link_t>& out,
    const columns_t& columns) NOEXCEPT
{
    out.resize(columns.rows.size());
    for (size_t row{}; row < columns.edges.size(); ++row)
    {
        const auto slice = from_column(columns.edges[row]);
        const auto first = std::min<size_t>(slice.first, out.size());
        const auto last = std::min(first + slice.count, out.size());
        std::fill(std::next(out.begin(), first), std::next(out.begin(), last),
            possible_narrow_cast<link_t>(row));
    }
}

template <typename Word>
inline void algorithm::resolve(std::span<mark> marks,
    const columns_t& columns, size_t maximum_outcomes, uint64_t rate,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    const graph_t graph{ columns };
    const auto count = columns.cells.size();
    for (size_t row{}; row < count; ++row)
    {
        const auto present = is_present(from_column(columns.cells[row]));
        marks[row] = present ? mark::unmarked : mark::excluded;
    }

    std::vector<mask_t> masks{};
    std::vector<link_t> pending{};
    for (size_t at{}; at < count; ++at)
    {
        // A component is resolved at its label, its lowest member row.
        const auto row = possible_narrow_cast<link_t>(at);
        const auto label = cell(graph, row);
        if (!is_labeled(label) || label != row)
            continue;

        component_t component{};
        const auto collected = collect(component, graph, row);
        const auto related = collected && relate(component, graph);
        const auto limit = related ? maximum_outcomes : zero;
        leaves(masks, component, graph, limit, rate, bytes_per_sigop);
        if (masks.empty())
        {
            forfeit(pending, marks, graph, row);
            continue;
        }

        const auto chosen = masks[best<Word>(masks, component, graph, rate,
            bytes_per_sigop)];

        for (size_t member{}; member < component.count; ++member)
            if (!get_right(chosen, member))
                discard(pending, marks, graph, component.rows[member]);
    }
}

inline void algorithm::pack(std::vector<link_t>& out, std::span<mark> marks,
    const columns_t& columns, const totals_t& budget, size_t depth,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    takes_t takes{};
    packing_t kept{};
    pack_(out, takes, kept, marks, columns, budget, depth, bytes_per_sigop);
}

inline void algorithm::project(std::vector<link_t>& out,
    projection_t& projection, const columns_t& columns,
    const totals_t& budget, size_t depth, uint32_t bytes_per_sigop) NOEXCEPT
{
    takes_t takes{};
    auto& marks = projection.marks;
    pack_(out, takes, projection.packing, marks, columns, budget, depth,
        bytes_per_sigop);

    const auto left = std::ranges::find(marks, mark::unmarked) != marks.end();
    projection.exhausted = takes.empty() || !left;
    if (!takes.empty())
        projection.blocks.push_back(std::move(takes));
}

// Estimate (projected blocks).
// ----------------------------------------------------------------------------

constexpr uint64_t algorithm::above(const std::span<const take_t>& takes,
    uint64_t rate) NOEXCEPT
{
    uint64_t size{};
    for (const auto& take: takes)
        if (take.rate >= rate)
            size = ceilinged_add(size, take.weight);

    return size;
}

template <typename Arrivals>
inline uint64_t algorithm::estimate(const projection_t& projection,
    size_t count, uint64_t space, uint64_t room,
    const Arrivals& arrivals) NOEXCEPT
{
    uint64_t low{};
    auto high = max_uint64;
    const auto sized = levels(projection, count);
    if (!reaches(projection, sized, count, space, room, high, arrivals))
        return high;

    // A rate that has room implies that each higher rate has.
    while (low < high)
    {
        const auto middle = low + to_half(high - low);
        if (reaches(projection, sized, count, space, room, middle, arrivals))
            high = middle;
        else
            low = add1(middle);
    }

    return low;
}

// Columns (whole table).
// ----------------------------------------------------------------------------

template <typename Word>
constexpr bool algorithm::collect(component_t& out, link_t label,
    const std::span<const link_t>& cells) NOEXCEPT
{
    out = {};
    return collect_<Word>(zero, out, label, cells);
}

template <typename Word>
inline void algorithm::find(std::vector<link_t>& out,
    const std::span<const link_t>& values, link_t value) NOEXCEPT
{
    out.clear();
    find_<Word>(zero, out, values, value);
}

// Internals.
// ----------------------------------------------------------------------------

constexpr size_t algorithm::rows(const graph_t& graph) NOEXCEPT
{
    return graph.table.cells.size();
}

constexpr algorithm::link_t algorithm::cell(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return from_column(row < count ? graph.table.cells[row] :
        graph.extra.cells[row - count]);
}

constexpr void algorithm::set_cell(const graph_t& graph, link_t row,
    link_t value) NOEXCEPT
{
    const auto count = rows(graph);
    auto& stored = row < count ? graph.table.cells[row] :
        graph.extra.cells[row - count];

    stored = to_column(value);
}

constexpr algorithm::edges_t algorithm::edges(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return from_column(row < count ? graph.table.edges[row] :
        graph.extra.edges[row - count]);
}

constexpr algorithm::totals_t algorithm::totals(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return from_column(row < count ? graph.table.totals[row] :
        graph.extra.totals[row - count]);
}

constexpr algorithm::link_t algorithm::child(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto count = rows(graph);
    return from_column(row < count ? graph.table.children[row] :
        graph.extra.children[row - count]);
}

constexpr algorithm::link_t algorithm::parent(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return from_column(input < count ? graph.table.rows[input] :
        graph.extra.rows[input - count]);
}

constexpr algorithm::link_t algorithm::owner(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    if (input < count && !graph.table.owners.empty())
        return graph.table.owners[input];

    // The last row whose first input is not beyond the input.
    const auto& edges = input < count ? graph.table.edges : graph.extra.edges;
    const auto after = std::upper_bound(edges.begin(), edges.end(), input,
        [](link_t at, const edges_t& edge) NOEXCEPT
        {
            return at < from_column(edge.first);
        });

    const auto base = input < count ? zero : rows(graph);
    const auto offset = std::distance(edges.begin(), std::prev(after));
    return possible_narrow_cast<link_t>(base + offset);
}

constexpr algorithm::link_t algorithm::sibling(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return from_column(input < count ? graph.table.siblings[input] :
        graph.extra.siblings[input - count]);
}

constexpr algorithm::link_t algorithm::rival(const graph_t& graph,
    link_t input) NOEXCEPT
{
    const auto count = graph.table.rows.size();
    return from_column(input < count ? graph.table.rivals[input] :
        graph.extra.rivals[input - count]);
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
    // A candidate input enters the ring of its table rival, which excludes it.
    const auto count = graph.table.rows.size();
    const auto next = rival(graph, input);
    const auto start = (input >= count && next < count) ? next : input;
    auto at = start;

    do
    {
        if (at != input)
            visit(at);

        // A table input bridges to the candidate inputs spending its point.
        if (at < count)
            for (const auto& bridge: graph.rivals)
                if (bridge.from == at && bridge.to != input)
                    visit(bridge.to);

        at = rival(graph, at);
    }
    while (at != none && at != start);
}

template <typename Visitor>
constexpr bool algorithm::each_member(const graph_t& graph, link_t row,
    const Visitor& visit) NOEXCEPT
{
    auto all = true;
    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        const auto up = parent(graph, input);
        if (up != none && is_labeled(cell(graph, up)))
            all = all && visit(up);

        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            const auto rival_row = owner(graph, other);
            if (is_present(cell(graph, rival_row)))
                all = all && visit(rival_row);
        });
    }

    each_child(graph, row, [&](link_t down) NOEXCEPT
    {
        if (is_present(cell(graph, down)))
            all = all && visit(down);
    });

    return all;
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

        winner &= twos_complement(winner);
        out |= bit_and(clique, bit_not(winner));
    }

    return out;
}

template <typename Word>
constexpr size_t algorithm::best(const std::span<const mask_t>& outcomes,
    const component_t& component, const graph_t& graph, uint64_t rate,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    size_t out{};
    uint64_t most{};
    uint64_t least{};
    auto first = true;
    each_fee<Word>(zero, outcomes, component, graph,
        [&](size_t index, uint64_t fee) NOEXCEPT
        {
            const auto size = is_zero(rate) ? 0_u64 : chargeable(
                outcomes[index], component, graph, bytes_per_sigop);

            if (first || worth(fee, size, most, least, rate))
            {
                first = false;
                most = fee;
                least = size;
                out = index;
            }
        });

    return out;
}

constexpr algorithm::mask_t algorithm::greedy(const component_t& component,
    const graph_t& graph, uint64_t rate, uint32_t bytes_per_sigop) NOEXCEPT
{
    struct value_t
    {
        uint64_t fee;
        uint64_t bytes;
        size_t member;
    };

    std_array<value_t, maximum_members> values{};
    for (size_t member{}; member < component.count; ++member)
    {
        const auto row = totals(graph, component.rows[member]);
        values[member] = { row.fee, chargeable(row, bytes_per_sigop), member };
    }

    const auto end = std::next(values.begin(), component.count);
    std::sort(values.begin(), end, [rate](const value_t& left,
        const value_t& right) NOEXCEPT
    {
        if (worth(left.fee, left.bytes, right.fee, right.bytes, rate))
            return true;

        if (worth(right.fee, right.bytes, left.fee, left.bytes, rate))
            return false;

        return left.member < right.member;
    });

    mask_t won{};
    mask_t lost{};
    mask_t contested{};
    for (size_t contest{}; contest < component.contests; ++contest)
        contested |= component.cliques[contest];

    for (auto value = values.begin(); value != end; ++value)
    {
        const auto need = set_right(component.ancestors[value->member], value->member);
        if (is_nonzero(bit_and(need, lost)))
            continue;

        auto taken = true;
        mask_t losers{};
        for (size_t contest{}; taken && contest < component.contests;
            ++contest)
        {
            const auto clique = component.cliques[contest];
            const auto mine = bit_and(clique, need);
            if (is_zero(mine))
                continue;

            const auto others = bit_and(clique, bit_not(need));
            taken = is_one(ones_count(mine)) && is_zero(bit_and(others, won));
            losers |= others;
        }

        if (!taken)
            continue;

        won |= need;
        lost |= losers;
    }

    const auto all = unmask_right<mask_t>(component.count);
    auto out = bit_or(won, bit_and(all, bit_not(bit_or(lost, contested))));
    for (size_t member{}; member < component.count; ++member)
        if (is_nonzero(bit_and(component.ancestors[member], lost)))
            out = set_right(out, member, false);

    return out;
}

constexpr bool algorithm::worth(uint64_t fee, uint64_t weight,
    uint64_t other_fee, uint64_t other_weight, uint64_t rate) NOEXCEPT
{
    // Fee less the rate on weight.
    const auto value = ceilinged_add(ceilinged_multiply(fee, per_weight),
        ceilinged_multiply(rate, other_weight));
    const auto other = ceilinged_add(ceilinged_multiply(other_fee, per_weight),
        ceilinged_multiply(rate, weight));
    return value > other;
}

inline void algorithm::discard(std::vector<link_t>& pending,
    std::span<mark> marks, const graph_t& graph, link_t row) NOEXCEPT
{
    pending.assign(one, row);
    marks[row] = mark::excluded;

    while (!pending.empty())
    {
        const auto next = pending.back();
        pending.pop_back();
        each_child(graph, next, [&](link_t down) NOEXCEPT
        {
            if (marks[down] != mark::excluded)
            {
                marks[down] = mark::excluded;
                pending.push_back(down);
            }
        });
    }
}

inline void algorithm::forfeit(std::vector<link_t>& pending,
    std::span<mark> marks, const graph_t& graph, link_t row) NOEXCEPT
{
    std::vector<link_t> found{ row };
    const auto add = [&](link_t next) NOEXCEPT
    {
        if (std::ranges::find(found, next) == found.end())
            found.push_back(next);

        return true;
    };

    for (size_t at{}; at < found.size(); ++at)
        each_member(graph, found[at], add);

    for (const auto member: found)
        discard(pending, marks, graph, member);
}

inline void algorithm::pack_(std::vector<link_t>& out, takes_t& takes,
    packing_t& kept, std::span<mark> marks, const columns_t& columns,
    const totals_t& budget, size_t depth, uint32_t bytes_per_sigop) NOEXCEPT
{
    const auto same = [](const totals_t& left, const totals_t& right) NOEXCEPT
    {
        return left.fee == right.fee && left.weight == right.weight &&
            left.sigops == right.sigops;
    };

    // What is kept is of the rows and the limits it was made for, with no
    // row before the first unmarked row met by a walk.
    const graph_t graph{ columns };
    const auto count = columns.cells.size();
    const auto current = kept.rows == count && same(kept.budget, budget) &&
        kept.depth == depth && kept.bytes_per_sigop == bytes_per_sigop;

    if (!current)
    {
        const auto end = std::next(marks.begin(), count);
        const auto start = std::find(marks.begin(), end, mark::unmarked);
        const auto offset = to_unsigned(std::distance(marks.begin(), start));
        kept = {};
        kept.rows = count;
        kept.budget = budget;
        kept.depth = depth;
        kept.bytes_per_sigop = bytes_per_sigop;
        kept.base = possible_narrow_cast<link_t>(offset);
        kept.places.resize(count - offset);
    }

    const auto base = kept.base;
    const auto live = kept.places.size();
    auto& places = kept.places;
    auto& open = kept.open;
    auto& joined = kept.joined;
    auto& ring = kept.ring;
    auto& partial = kept.partial;
    auto& firsts = kept.offers;
    std::vector<offer_t> others{};
    std::vector<link_t> closed{};
    std::vector<link_t> rows{};
    std::vector<link_t> group{};
    std::vector<link_t> fresh{};
    std::vector<link_t> lost{};
    std::vector<link_t> descent{};
    std::vector<link_t> seeds{};
    std::vector<link_t> work{};
    std::vector<link_t> drop{};
    visits_t seen{ std::vector<link_t>(live), base };
    visits_t below{ std::vector<link_t>(live), base };
    visits_t walk{ std::vector<link_t>(live), base };
    flow_t flow{};
    size_t offers{};
    auto heaped = false;
    totals_t used{};
    out.clear();
    takes.clear();

    const auto fits = [](const totals_t& sum, const totals_t& space) NOEXCEPT
    {
        return sum.weight <= space.weight && sum.sigops <= space.sigops;
    };

    const auto unmarked = [&](link_t row) NOEXCEPT
    {
        return marks[row] == mark::unmarked;
    };

    const auto available = [&]() NOEXCEPT -> totals_t
    {
        return
        {
            {},
            floored_subtract(budget.weight, used.weight),
            floored_subtract(budget.sigops, used.sigops)
        };
    };

    const auto alone = [&](link_t row) NOEXCEPT
    {
        return fits(totals(graph, row), available());
    };

    // Chargeable size of the rows, each charged alone.
    const auto charged = [&](const std::vector<link_t>& members) NOEXCEPT
    {
        uint64_t bytes{};
        for (const auto member: members)
            bytes = ceilinged_add(bytes, chargeable(totals(graph, member),
                bytes_per_sigop));

        return bytes;
    };

    // An unmarked row of no more than the value is left (least on top).
    const auto remains = [&](std::vector<least_t>& least,
        uint32_t value) NOEXCEPT
    {
        while (!least.empty() && !unmarked(least.front().second))
        {
            std::pop_heap(least.begin(), least.end(), std::greater<>{});
            least.pop_back();
        }

        return !least.empty() && least.front().first <= value;
    };

    // The highest rate is on top, a bound over offers of equal rate, the
    // lowest row of equal rates, and the latest offer of the same row.
    const auto lower = [](const offer_t& left, const offer_t& right) NOEXCEPT
    {
        if (left.rate != right.rate)
            return left.rate < right.rate;

        const auto left_bound = left.count == none;
        const auto right_bound = right.count == none;
        if (left_bound != right_bound)
            return right_bound;

        return left.row == right.row ? left.order < right.order :
            left.row > right.row;
    };

    // The first offers are kept and heaped at once, the others as offered.
    const auto push = [&](offer_t&& score) NOEXCEPT
    {
        score.order = offers++;
        if (!heaped)
        {
            firsts.push_back(score);
            return;
        }

        others.push_back(score);
        std::push_heap(others.begin(), others.end(), lower);
    };

    const auto score_row = [&](link_t row) NOEXCEPT
    {
        totals_t sum{};
        if (chunk(rows, sum, walk, marks, graph, row, depth, available()))
            push({ row, {}, {}, {}, fee_rate(sum.fee, charged(rows)), sum });
    };

    // Each row of group offers its chunk (unless every, only rows below take),
    // none of which fits unless one of the rows does.
    const auto score_rows = [&](bool every) NOEXCEPT
    {
        if (!std::ranges::any_of(group, alone))
            return;

        for (const auto member: group)
            if (every || visited(below, member))
                score_row(member);
    };

    // The closure offered with its sum, held for the take.
    const auto hold = [&](const totals_t& sum) NOEXCEPT
    {
        auto& held = heaped ? closed : kept.closed;
        const auto first = held.size();
        const auto size = possible_narrow_cast<link_t>(rows.size());
        const auto rate = fee_rate(sum.fee, charged(rows));
        held.insert(held.end(), rows.begin(), rows.end());
        push({ rows.front(), size, first, {}, rate, sum });
    };

    // Least rate leaf of the closure and its descendants leave the work.
    const auto shed = [&]() NOEXCEPT
    {
        auto leaf = none;
        totals_t least{};
        for (const auto row: rows)
        {
            auto parent_of_row = false;
            each_child(graph, row, [&](link_t down) NOEXCEPT
            {
                parent_of_row = parent_of_row ||
                    std::ranges::find(rows, down) != rows.end();
            });

            if (parent_of_row)
                continue;

            const auto own = totals(graph, row);
            const auto bytes = chargeable(own, bytes_per_sigop);
            const auto least_bytes = chargeable(least, bytes_per_sigop);
            if (leaf == none || ceilinged_multiply(own.fee, least_bytes) <=
                ceilinged_multiply(least.fee, bytes))
            {
                leaf = row;
                least = own;
            }
        }

        drop.assign(one, leaf);
        for (size_t at{}; at < drop.size(); ++at)
            each_child(graph, drop[at], [&](link_t down) NOEXCEPT
            {
                if (std::ranges::find(drop, down) == drop.end())
                    drop.push_back(down);
            });

        std::erase_if(work, [&](link_t row) NOEXCEPT
        {
            return std::ranges::find(drop, row) != drop.end();
        });
    };

    // Whole cluster over the space offers its best closure that fits.
    const auto fit = [&]() NOEXCEPT
    {
        totals_t sum{};
        work.assign(group.begin(), group.end());
        while (!work.empty())
        {
            closure(rows, sum, flow, graph, work, bytes_per_sigop);
            if (rows.empty())
                return;

            if (fits(sum, available()))
            {
                hold(sum);
                return;
            }

            shed();
        }
    };

    // Whole cluster offers its best closure (itself if one row), else each of
    // its rows its chunk, of partial cluster of take only those below take.
    const auto propose = [&](bool whole, bool every) NOEXCEPT
    {
        totals_t sum{};
        if (whole && is_one(group.size()))
        {
            rows.assign(one, group.front());
            sum = totals(graph, group.front());
        }
        else if (whole)
        {
            closure(rows, sum, flow, graph, group, bytes_per_sigop);
        }

        if (whole && fits(sum, available()))
        {
            hold(sum);
            return;
        }

        if (whole && !is_one(group.size()))
            fit();

        score_rows(every || whole);
    };

    // Cluster of row below take,
    // false if over maximum_members with nothing offered.
    const auto offer = [&](link_t row) NOEXCEPT
    {
        const auto whole = cluster(group, walk, marks, graph, row);
        if (group.size() > maximum_members)
            return false;

        for (const auto member: group)
            visit(seen, member);

        propose(whole, false);
        return true;
    };

    // The first place of the cluster of the place.
    const auto lowest = [&](link_t place) NOEXCEPT
    {
        while (joined[place] != place)
        {
            joined[place] = joined[joined[place]];
            place = joined[place];
        }

        return place;
    };

    // Rows of the ring of the first place of a cluster.
    const auto gather = [&](link_t place) NOEXCEPT
    {
        group.clear();
        auto member = place;
        do
        {
            group.push_back(open[member]);
            member = ring[member];
        }
        while (member != place);
    };

    // Each place is joined to an earlier place of its cluster (the first to
    // itself) and is in a ring of the places of its cluster, which is partial
    // (at its first place) if a row of it has an excluded parent. Each
    // cluster of the places then makes its first offer.
    const auto form = [&](const auto& set) NOEXCEPT
    {
        for (const auto place: set)
        {
            joined[place] = ring[place] = place;
            partial[place] = false;
        }

        for (const auto place: set)
        {
            const auto slice = edges(graph, open[place]);
            for (auto input = slice.first; input < slice.first + slice.count;
                ++input)
            {
                const auto up = parent(graph, input);
                if (up == none || is_boundary(cell(graph, up)) ||
                    marks[up] == mark::taken)
                    continue;

                auto low = lowest(place);
                if (marks[up] == mark::excluded)
                {
                    partial[low] = true;
                    continue;
                }

                auto high = lowest(places[up - base]);
                if (high == low)
                    continue;

                if (high < low)
                    std::swap(low, high);

                joined[high] = low;
                std::swap(ring[low], ring[high]);
                partial[low] = partial[low] || partial[high];
            }
        }

        for (const auto place: set)
        {
            if (joined[place] != place)
                continue;

            gather(place);
            if (is_one(group.size()))
            {
                propose(!partial[place], true);
                continue;
            }

            // No offer of cluster exceeds the rate of its best row.
            uint64_t most{};
            for (const auto member: group)
            {
                const auto own = totals(graph, member);
                most = std::max(most, fee_rate(own.fee,
                    chargeable(own, bytes_per_sigop)));
            }

            push({ group.front(), none, {}, {}, most, {} });
        }
    };

    // Cluster of bound proposes as when formed, with the whole budget.
    const auto unfold = [&](link_t row) NOEXCEPT
    {
        const auto place = lowest(places[row - base]);
        gather(place);
        const auto spent = used;
        used = {};
        propose(!partial[place] && group.size() <= maximum_members, true);
        used = spent;
    };

    if (!current)
    {
        // The unmarked rows in order, each with its place among them.
        const auto start = std::next(marks.begin(), count - live);
        const auto end = std::next(marks.begin(), count);
        const auto total = std::count(start, end, mark::unmarked);
        const auto all = possible_narrow_cast<link_t>(to_unsigned(total));
        open.resize(all);
        joined.resize(all);
        ring.resize(all);
        partial.resize(all);
        kept.sizes.resize(all);
        kept.sigops.resize(all);

        link_t place{};
        for (auto at = count - live; at < count; ++at)
        {
            const auto row = possible_narrow_cast<link_t>(at);
            if (!unmarked(row))
                continue;

            const auto own = totals(graph, row);
            places[row - base] = place;
            open[place] = row;
            kept.sizes[place] = { own.weight, row };
            kept.sigops[place] = { own.sigops, row };
            ++place;
        }

        std::make_heap(kept.sizes.begin(), kept.sizes.end(),
            std::greater<>{});
        std::make_heap(kept.sigops.begin(), kept.sigops.end(),
            std::greater<>{});
        form(std::views::iota(link_t{}, all));
    }
    else
    {
        // A cluster that lost rows to the last block is formed again from
        // those left, and the first offers of the others are kept.
        restart(seen);
        for (const auto row: kept.taken)
        {
            const auto first = lowest(places[row - base]);
            if (visit(seen, open[first]))
                lost.push_back(first);
        }

        std::erase_if(firsts, [&](const offer_t& score) NOEXCEPT
        {
            return visited(seen, open[lowest(places[score.row - base])]);
        });

        for (auto& score: firsts)
            score.order = offers++;

        for (const auto first: lost)
        {
            auto member = first;
            do
            {
                if (unmarked(open[member]))
                    fresh.push_back(member);

                member = ring[member];
            }
            while (member != first);
        }

        form(fresh);
    }

    std::make_heap(firsts.begin(), firsts.end(), lower);
    auto left = firsts.size();
    heaped = true;

    // A first offer leaves the heap and is kept.
    while (is_nonzero(left) || !others.empty())
    {
        // A take holds an unmarked row, so none is left when none fits.
        const auto space = available();
        if (!remains(kept.sizes, space.weight) ||
            !remains(kept.sigops, space.sigops))
            break;

        const auto first = is_nonzero(left) && (others.empty() ||
            lower(others.front(), firsts.front()));

        offer_t score{};
        if (first)
        {
            std::pop_heap(firsts.begin(), std::next(firsts.begin(), left),
                lower);
            score = firsts[--left];
        }
        else
        {
            std::pop_heap(others.begin(), others.end(), lower);
            score = others.back();
            others.pop_back();
        }

        if (score.count == none)
        {
            unfold(score.row);
            continue;
        }

        totals_t sum{};
        if (is_nonzero(score.count))
        {
            const auto& from = first ? kept.closed : closed;
            const auto held = std::span<const link_t>{ from }.subspan(
                score.first, score.count);

            if (!std::ranges::all_of(held, unmarked))
                continue;

            // Closure that no longer fits falls back to chunks of its cluster.
            if (!fits(score.sum, space))
            {
                if (cluster(group, walk, marks, graph, score.row) &&
                    !is_one(group.size()) && group.size() <= maximum_members)
                    fit();

                score_rows(true);
                continue;
            }

            rows.assign(held.begin(), held.end());
            sum = score.sum;
        }
        else
        {
            if (!unmarked(score.row) || !chunk(rows, sum, walk, marks, graph,
                score.row, depth, space))
                continue;

            // A changed chunk competes again at its current rate.
            if (!same(sum, score.sum))
            {
                const auto rate = fee_rate(sum.fee, charged(rows));
                push({ score.row, {}, {}, {}, rate, sum });
                continue;
            }
        }

        for (const auto row: rows)
            marks[row] = mark::taken;

        out.insert(out.end(), rows.begin(), rows.end());
        takes.push_back({ score.rate, charged(rows) });
        used.weight = ceilinged_add(used.weight, sum.weight);
        used.sigops = ceilinged_add(used.sigops, sum.sigops);

        // Untaken descendants of take have changed chunks, and untaken
        // remainder of its cluster competes again from its children.
        restart(seen);
        restart(below);
        seeds.clear();
        descent.assign(rows.begin(), rows.end());
        const auto taken = descent.size();
        for (size_t at{}; at < descent.size(); ++at)
        {
            each_child(graph, descent[at], [&](link_t down) NOEXCEPT
            {
                if (!unmarked(down) || !visit(below, down))
                    return;

                descent.push_back(down);
                if (marks[descent[at]] == mark::taken)
                    seeds.push_back(down);
            });
        }

        auto walked = true;
        for (const auto seed: seeds)
            if (!visited(seen, seed) && !offer(seed))
                walked = false;

        // Each descendant that no walk met is of a cluster not walked.
        if (!walked)
            for (auto at = taken; at < descent.size(); ++at)
                if (!visited(seen, descent[at]))
                    score_row(descent[at]);
    }

    kept.taken = out;
    std::sort(out.begin(), out.end());
}

// Takes are in about descending order of rate as taken.
inline std::vector<algorithm::takes_t> algorithm::levels(
    const projection_t& projection, size_t count) NOEXCEPT
{
    const auto& blocks = projection.blocks;
    std::vector<takes_t> out(std::min(count, blocks.size()));
    for (size_t block{}; block < out.size(); ++block)
    {
        auto& level = out[block];
        level.assign(blocks[block].rbegin(), blocks[block].rend());
        if (!std::ranges::is_sorted(level, {}, &take_t::rate))
            std::ranges::sort(level, {}, &take_t::rate);

        uint64_t size{};
        for (auto take = level.rbegin(); take != level.rend(); ++take)
            take->weight = size = ceilinged_add(size, take->weight);
    }

    return out;
}

template <typename Arrivals>
inline bool algorithm::reaches(const projection_t& projection,
    const std::vector<takes_t>& levels, size_t count, uint64_t space,
    uint64_t room, uint64_t rate, const Arrivals& arrivals) NOEXCEPT
{
    const auto blocks = projection.blocks.size();
    uint64_t arriving{};
    uint64_t carry{};

    // A block that leaves rows is full, so its takes are its capacity.
    for (size_t block{}; block < count; ++block)
    {
        std::span<const take_t> level{};
        if (block < levels.size())
            level = levels[block];

        const auto from = std::ranges::lower_bound(level, rate, {},
            &take_t::rate);

        const auto last = add1(block) >= blocks;
        const auto full = !level.empty() && !(last && projection.exhausted);
        const auto capacity = full ? level.front().weight : space;
        const auto over = from == level.end() ? 0_u64 : from->weight;
        const auto demand = ceilinged_add(carry, over);
        if (floored_subtract(capacity, demand) >= room)
            return true;

        // Arrivals join what the first block leaves, and each block after.
        if (add1(block) == count)
            break;

        if (is_zero(block))
            arriving = arrivals(rate);

        const auto excess = floored_subtract(demand, capacity);
        carry = ceilinged_add(excess, arriving);
    }

    return false;
}

inline void algorithm::restart(visits_t& visits) NOEXCEPT
{
    if (is_nonzero(++visits.walk))
        return;

    std::ranges::fill(visits.walks, link_t{});
    visits.walk = 1_u32;
}

inline bool algorithm::visited(const visits_t& visits, link_t row) NOEXCEPT
{
    return visits.walks[row - visits.first] == visits.walk;
}

inline bool algorithm::visit(visits_t& visits, link_t row) NOEXCEPT
{
    auto& walk = visits.walks[row - visits.first];
    if (walk == visits.walk)
        return false;

    walk = visits.walk;
    return true;
}

inline bool algorithm::chunk(std::vector<link_t>& out, totals_t& sum,
    visits_t& visits, const std::span<const mark>& marks,
    const graph_t& graph, link_t row, size_t depth,
    const totals_t& available) NOEXCEPT
{
    sum = {};
    out.assign(one, row);
    restart(visits);
    visit(visits, row);

    // Each level holds the untaken parents of the previous level.
    for (size_t level{}, begin{}; begin < out.size(); ++level)
    {
        const auto end = out.size();
        for (auto at = begin; at < end; ++at)
        {
            const auto slice = edges(graph, out[at]);
            for (auto input = slice.first;
                input < slice.first + slice.count; ++input)
            {
                const auto up = parent(graph, input);
                if (up == none || marks[up] == mark::taken ||
                    is_boundary(cell(graph, up)))
                    continue;

                if (marks[up] == mark::excluded)
                    return false;

                if (!visit(visits, up))
                    continue;

                if (level == depth)
                    return false;

                out.push_back(up);
            }
        }

        begin = end;
    }

    std::sort(out.begin(), out.end());
    auto cut = out.begin();
    for (; cut != out.end(); ++cut)
    {
        const auto& add = totals(graph, *cut);
        const auto size = ceilinged_add(sum.weight, add.weight);
        const auto sigops = ceilinged_add(sum.sigops, add.sigops);
        if (size > available.weight || sigops > available.sigops)
            break;

        sum.fee = ceilinged_add(sum.fee, add.fee);
        sum.weight = size;
        sum.sigops = sigops;
    }

    out.erase(cut, out.end());
    return !out.empty();
}

inline bool algorithm::cluster(std::vector<link_t>& out, visits_t& visits,
    const std::span<const mark>& marks, const graph_t& graph,
    link_t row) NOEXCEPT
{
    out.assign(one, row);
    restart(visits);
    visit(visits, row);
    auto whole = true;
    const auto push = [&](link_t found) NOEXCEPT
    {
        if (marks[found] == mark::unmarked && visit(visits, found))
            out.push_back(found);
    };

    for (size_t at{}; at < out.size(); ++at)
    {
        if (out.size() > maximum_members)
            return false;

        const auto member = out[at];
        const auto slice = edges(graph, member);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up == none || is_boundary(cell(graph, up)))
                continue;

            if (marks[up] == mark::excluded)
                whole = false;
            else
                push(up);
        }

        each_child(graph, member, [&](link_t down) NOEXCEPT
        {
            push(down);
        });
    }

    return whole && out.size() <= maximum_members;
}

inline void algorithm::closure(std::vector<link_t>& out, totals_t& sum,
    flow_t& flow, const graph_t& graph, const std::vector<link_t>& cluster,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    // A node is a row in ascending order, a link a node to a parent node.
    const auto count = cluster.size();
    const auto end = std::next(flow.rows.begin(), count);
    std::copy(cluster.begin(), cluster.end(), flow.rows.begin());
    std::sort(flow.rows.begin(), end);

    size_t links{};
    uint64_t fee{};
    uint64_t size{};
    for (size_t node{}; node < count; ++node)
    {
        const auto row = flow.rows[node];
        const auto own = totals(graph, row);
        flow.fees[node] = own.fee;
        flow.sizes[node] = chargeable(own, bytes_per_sigop);
        fee = ceilinged_add(fee, flow.fees[node]);
        size = ceilinged_add(size, flow.sizes[node]);

        mask_t parents{};
        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            const auto it = std::lower_bound(flow.rows.begin(), end, up);
            if (it != end && *it == up)
                set_right_into(parents, to_unsigned(std::distance(
                    flow.rows.begin(), it)));
        }

        flow.parents[node] = parents;
        flow.links[node] = links;
        links += ones_count(parents);
    }

    // No capacity exceeds the fee of the cluster times its size.
    auto best = is_multiply_overflow(fee, size) ?
        closure_(flow, flow.wide, count, links, fee, size) :
        closure_(flow, flow.narrow, count, links, fee, size);

    out.clear();
    sum = {};
    for (; is_nonzero(best); best &= sub1(best))
    {
        const auto row = flow.rows[right_zeros(best)];
        const auto own = totals(graph, row);
        out.push_back(row);
        sum.fee = ceilinged_add(sum.fee, own.fee);
        sum.weight = ceilinged_add(sum.weight, own.weight);
        sum.sigops = ceilinged_add(sum.sigops, own.sigops);
    }
}

// Dinkelbach: maximum weight closure at rate of best closure
// found (fee times its size less size times its fee, a
// minimum cut) is positive until no closure has higher rate.
template <typename Capacity>
inline algorithm::mask_t algorithm::closure_(flow_t& flow,
    residual_t<Capacity>& residual, size_t count, size_t links, uint64_t fee,
    uint64_t size) NOEXCEPT
{
    const auto link = [&](size_t node, size_t up) NOEXCEPT
    {
        const auto before = unmask_right<mask_t>(up);
        return flow.links[node] + ones_count(bit_and(flow.parents[node],
            before));
    };

    const auto weigh = [&](mask_t nodes, uint64_t& fees,
        uint64_t& sizes) NOEXCEPT
    {
        fees = 0_u64;
        sizes = 0_u64;
        for (; is_nonzero(nodes); nodes &= sub1(nodes))
        {
            const auto node = right_zeros(nodes);
            fees = ceilinged_add(fees, flow.fees[node]);
            sizes = ceilinged_add(sizes, flow.sizes[node]);
        }
    };

    // Source feeds each node its fee at the size, each node feeds sink its size
    // at the fee, and a node feeds its parents without limit. Nodes reached
    // from source when no more can flow are the closure, and a node passes at
    // once what its own two limits allow.
    const auto cut = [&]() NOEXCEPT
    {
        residual.flows.assign(links, Capacity{});
        for (size_t node{}; node < count; ++node)
        {
            const auto gain = Capacity{ flow.fees[node] } * size;
            const auto cost = Capacity{ flow.sizes[node] } * fee;
            const auto both = std::min(gain, cost);
            residual.source[node] = gain - both;
            residual.sink[node] = cost - both;
            flow.fed[node] = {};
        }

        while (true)
        {
            // Path to each node reached is by node before it, source if count.
            mask_t reached{};
            size_t head{};
            size_t tail{};
            for (size_t node{}; node < count; ++node)
            {
                if (residual.source[node] != Capacity{})
                {
                    set_right_into(reached, node);
                    flow.before[node] = count;
                    flow.queue[tail++] = node;
                }
            }

            auto last = count;
            while (head < tail && last == count)
            {
                const auto node = flow.queue[head++];
                if (residual.sink[node] != Capacity{})
                {
                    last = node;
                    break;
                }

                // Flow moves to a parent, or back to a child that fed it.
                const auto next = bit_or(flow.parents[node], flow.fed[node]);
                for (auto more = bit_and(next, bit_not(reached));
                    is_nonzero(more); more &= sub1(more))
                {
                    const auto other = right_zeros(more);
                    set_right_into(reached, other);
                    flow.before[other] = node;
                    flow.queue[tail++] = other;
                }
            }

            if (last == count)
                return reached;

            auto amount = residual.sink[last];
            auto node = last;
            for (; flow.before[node] != count; node = flow.before[node])
            {
                const auto from = flow.before[node];
                if (!get_right(flow.parents[from], node))
                    amount = std::min(amount,
                        residual.flows[link(node, from)]);
            }

            amount = std::min(amount, residual.source[node]);
            residual.source[node] -= amount;
            residual.sink[last] -= amount;
            for (node = last; flow.before[node] != count;
                node = flow.before[node])
            {
                const auto from = flow.before[node];
                if (get_right(flow.parents[from], node))
                {
                    residual.flows[link(from, node)] += amount;
                    set_right_into(flow.fed[node], from);
                    continue;
                }

                auto& back = residual.flows[link(node, from)];
                back -= amount;
                if (back == Capacity{})
                    flow.fed[from] = set_right(flow.fed[from], node, false);
            }
        }
    };

    auto best = unmask_right<mask_t>(count);
    while (count > one && is_nonzero(size))
    {
        uint64_t found_fee{};
        uint64_t found_size{};
        const auto found = cut();
        weigh(found, found_fee, found_size);
        if (is_zero(found_size) || Capacity{ found_fee } * size <=
            Capacity{ found_size } * fee)
            break;

        best = found;
        fee = found_fee;
        size = found_size;
    }

    return best;
}

inline algorithm::link_t algorithm::resolved(const graph_t& graph,
    link_t row) NOEXCEPT
{
    const auto own = cell(graph, row);
    if (is_boundary(own))
        return own;

    auto next = reduced(own);
    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        const auto up = parent(graph, input);
        if (up != none && is_absent(cell(graph, up)))
            next = raised(next, ancestor);

        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            if (is_boundary(cell(graph, owner(graph, other))))
                next = raised(next, rivaled);
        });
    }

    return next;
}

// Children of row depend on its absence, and its rivals on its confirmation.
// Labeled row that leaves its component leaves rest of it to relabel, and row
// made present joins one.
inline void algorithm::change(walk_t& walk, const columns_t& columns,
    link_t row, link_t next) NOEXCEPT
{
    const graph_t graph{ columns };
    const auto own = cell(graph, row);
    if (next == own)
        return;

    if (is_labeled(own) && !is_present(next))
    {
        component_t former{};
        if (collect(former, graph, row))
            walk.affected.insert(walk.affected.end(), former.rows.cbegin(),
                std::next(former.rows.cbegin(), former.count));
        else
            walk.complete = false;
    }

    set_cell(graph, row, next);
    if (is_present(next) && !is_present(own))
        walk.affected.push_back(row);

    if (is_absent(next) != is_absent(own))
    {
        each_child(graph, row, [&](link_t down) NOEXCEPT
        {
            walk.pending.push_back(down);
        });
    }

    if (is_boundary(next) == is_boundary(own))
        return;

    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            walk.pending.push_back(owner(graph, other));
        });
    }
}

inline bool algorithm::spread(walk_t& walk, const columns_t& columns) NOEXCEPT
{
    const graph_t graph{ columns };
    while (!walk.pending.empty())
    {
        const auto row = walk.pending.back();
        walk.pending.pop_back();
        change(walk, columns, row, resolved(graph, row));
    }

    return reform(columns, walk.affected) && walk.complete;
}

inline bool algorithm::reform(const columns_t& columns,
    const std::span<const link_t>& rows) NOEXCEPT
{
    const graph_t graph{ columns };
    for (const auto row: rows)
        if (is_present(cell(graph, row)))
            set_cell(graph, row, unlabeled);

    // Relevance closes over the rows in at most count passes.
    for (auto changed = true; changed;)
    {
        changed = false;
        for (const auto row: rows)
        {
            if (!is_present(cell(graph, row)) || is_labeled(cell(graph, row)))
                continue;

            if (contested(graph, row) || descended(graph, row))
            {
                set_cell(graph, row, row);
                changed = true;
            }
        }
    }

    auto complete = true;
    for (const auto row: rows)
    {
        // A row relabeled by a previous component carries a lower label.
        if (cell(graph, row) != row)
            continue;

        component_t component{};
        if (collect(component, graph, row))
            relabel(columns, component);
        else
            complete = false;
    }

    return complete;
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
        const auto& total = totals(graph, component.rows[member]);
        const auto fee = f::broadcast<Word>(total.fee);
        const auto selected = f::eq<lane>(f::and_(outcomes, bit), bit);
        sum = f::add<lane>(sum, f::and_(selected, fee));
    }

    return sum;
}

template <typename Word, typename Visitor>
constexpr void algorithm::each_fee(size_t index,
    const std::span<const mask_t>& outcomes, const component_t& component,
    const graph_t& graph, const Visitor& visit) NOEXCEPT
{
    const auto count = outcomes.size();
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lanes = mask_lanes<Word>;
            aligned_array<uint64_t, lanes> sums{};
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = load<Word>(&outcomes[index]);
                store(sums.data(), fees<Word>(word, component, graph));

                for (size_t lane{}; lane < lanes; ++lane)
                    visit(index + lane, sums[lane]);
            }
        }

        using next = narrower<Word, mask_t>;
        each_fee<next>(index, outcomes, component, graph, visit);
    }
    else
    {
        for (; index < count; ++index)
            visit(index, fees<mask_t>(outcomes[index], component, graph));
    }
}

template <typename Word>
INLINE constexpr Word algorithm::native(Word word) NOEXCEPT
{
    if constexpr (is_little_endian)
        return word;
    else
        return f::byteswap<link_t>(word);
}

template <typename Word>
constexpr bool algorithm::collect_(size_t index, component_t& out,
    link_t label, const std::span<const link_t>& cells) NOEXCEPT
{
    const auto count = cells.size();
    const auto stored = to_column(label);
    const auto add = [&](size_t row) NOEXCEPT
    {
        if (out.count == maximum_members)
            return false;

        out.rows[out.count++] = possible_narrow_cast<link_t>(row);
        return true;
    };

    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lanes = link_lanes<Word>;
            const auto wanted = f::broadcast<Word>(stored);
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = load<Word>(&cells[index]);
                const auto found = f::eq<bits<link_t>>(word, wanted);

                if (f::any(found))
                    for (auto row = index; row < index + lanes; ++row)
                        if (cells[row] == stored && !add(row))
                            return false;
            }
        }

        return collect_<narrower<Word, link_t>>(index, out, label, cells);
    }
    else
    {
        for (; index < count; ++index)
            if (cells[index] == stored && !add(index))
                return false;

        return true;
    }
}

template <typename Word>
inline void algorithm::find_(size_t index, std::vector<link_t>& out,
    const std::span<const link_t>& values, link_t value) NOEXCEPT
{
    const auto count = values.size();
    const auto stored = to_column(value);
    if constexpr (is_extended<Word>)
    {
        if constexpr (have<Word>)
        {
            constexpr auto lanes = link_lanes<Word>;
            const auto wanted = f::broadcast<Word>(stored);
            for (; index + lanes <= count; index += lanes)
            {
                const auto word = load<Word>(&values[index]);
                const auto found = f::eq<bits<link_t>>(word, wanted);

                if (f::any(found))
                    for (auto row = index; row < index + lanes; ++row)
                        if (values[row] == stored)
                            out.push_back(possible_narrow_cast<link_t>(row));
            }
        }

        find_<narrower<Word, link_t>>(index, out, values, value);
    }
    else
    {
        for (; index < count; ++index)
            if (values[index] == stored)
                out.push_back(possible_narrow_cast<link_t>(index));
    }
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
