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

constexpr uint64_t algorithm::chargeable(const totals_t& totals,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    const auto sigops = totals.sigops;
    const auto cost = ceilinged_multiply<uint64_t>(sigops, bytes_per_sigop);
    return greater(totals.size, cost);
}

constexpr uint64_t algorithm::fee_rate(const totals_t& totals,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    const auto size = greater(chargeable(totals, bytes_per_sigop), one);
    return ceilinged_multiply(totals.fee, 1'000_u64) / size;
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
        native_to_little_end(value.size),
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
        native_from_little_end(value.size),
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
        out.size = ceilinged_add(out.size, add.size);
        out.sigops = ceilinged_add(out.sigops, add.sigops);

        const auto slice = edges(graph, row);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up != none && is_present(cell(graph, up)))
                push(up);
        }

        return out.size <= limit.size && out.sigops <= limit.sigops;
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
    auto out = bit_and(unmask_right<mask_t>(component.count), bit_not(lost));
    for (size_t member{}; member < component.count; ++member)
        if (is_nonzero(bit_and(component.ancestors[member], lost)))
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
    each_fee<Word>(zero, outcomes, component, graph,
        [&](size_t index, uint64_t fee) NOEXCEPT
        {
            best = std::max(best, fee);
            if (is_zero(bit_and(outcomes[index], candidate)))
                best_without = std::max(best_without, fee);
        });
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
    const auto label = to_column(component.rows.front());
    for (size_t member{}; member < component.count; ++member)
        columns.cells[component.rows[member]] = label;
}

inline void algorithm::exclude(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    std::vector<link_t> pending{ row };
    set_cell(graph, row, absent);

    while (!pending.empty())
    {
        const auto next = pending.back();
        pending.pop_back();
        each_child(graph, next, [&](link_t down) NOEXCEPT
        {
            if (is_present(cell(graph, down)))
            {
                set_cell(graph, down, absent);
                pending.push_back(down);
            }
        });
    }
}

inline void algorithm::admit(const columns_t& columns, link_t row) NOEXCEPT
{
    const std_array<link_t, one> rows{ { row } };
    columns.cells[row] = to_column(unlabeled);
    reform(columns, rows);
}

inline void algorithm::confirm(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    component_t former{};
    const auto labeled = is_labeled(cell(graph, row)) &&
        collect(former, graph, row);

    set_cell(graph, row, boundary);
    const auto slice = edges(graph, row);
    for (auto input = slice.first; input < slice.first + slice.count; ++input)
    {
        each_rival(graph, input, [&](link_t other) NOEXCEPT
        {
            const auto rival_row = owner(graph, other);
            if (is_present(cell(graph, rival_row)))
                exclude(columns, rival_row);
        });
    }

    if (!labeled)
        return;

    std::vector<link_t> rest{};
    for (size_t member{}; member < former.count; ++member)
        if (is_present(cell(graph, former.rows[member])))
            rest.push_back(former.rows[member]);

    reform(columns, rest);
}

inline void algorithm::unconfirm(const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    std::vector<link_t> affected{ row };
    set_cell(graph, row, unlabeled);

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

// Template (frontier, table columns).
// ----------------------------------------------------------------------------

template <typename Word>
inline void algorithm::resolve(std::span<mark> marks,
    const columns_t& columns, size_t maximum_outcomes) NOEXCEPT
{
    const graph_t graph{ columns };
    const auto count = columns.cells.size();
    for (size_t row{}; row < count; ++row)
    {
        const auto present = is_present(from_column(columns.cells[row]));
        marks[row] = present ? mark::unmarked : mark::excluded;
    }

    std::vector<mask_t> masks{};
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
        const auto total = std::min(outcomes(component), limit);

        if (is_zero(total))
        {
            forfeit(marks, graph, row);
            continue;
        }

        masks.resize(total);
        for (size_t index{}; index < total; ++index)
            masks[index] = outcome(component, index);

        const auto chosen = masks[best<Word>(masks, component, graph)];
        for (size_t member{}; member < component.count; ++member)
            if (!get_right(chosen, member))
                discard(marks, graph, component.rows[member]);
    }
}

inline void algorithm::pack(std::vector<link_t>& out, std::span<mark> marks,
    const columns_t& columns, const totals_t& budget, size_t depth,
    uint32_t bytes_per_sigop) NOEXCEPT
{
    takes_t takes{};
    pack_(out, takes, marks, columns, budget, depth, bytes_per_sigop);
}

inline void algorithm::project(std::vector<link_t>& out,
    projection_t& projection, const columns_t& columns,
    const totals_t& budget, size_t depth, uint32_t bytes_per_sigop) NOEXCEPT
{
    takes_t takes{};
    auto& marks = projection.marks;
    pack_(out, takes, marks, columns, budget, depth, bytes_per_sigop);

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
            size = ceilinged_add(size, take.size);

    return size;
}

template <typename Arrivals>
inline uint64_t algorithm::estimate(const projection_t& projection,
    size_t count, uint64_t space, const Arrivals& arrivals) NOEXCEPT
{
    uint64_t low{};
    auto high = max_uint64;
    if (!reaches(projection, count, space, high, arrivals))
        return high;

    // A rate that is taken implies that each higher rate is taken.
    while (low < high)
    {
        const auto middle = low + to_half(high - low);
        if (reaches(projection, count, space, middle, arrivals))
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
    // The last row whose first input is not beyond the input.
    const auto count = graph.table.rows.size();
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
        if (up != none && is_absent(cell(graph, up)))
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

        winner &= twos_complement(winner);
        out |= bit_and(clique, bit_not(winner));
    }

    return out;
}

template <typename Word>
constexpr size_t algorithm::best(const std::span<const mask_t>& outcomes,
    const component_t& component, const graph_t& graph) NOEXCEPT
{
    size_t out{};
    uint64_t most{};
    each_fee<Word>(zero, outcomes, component, graph,
        [&](size_t index, uint64_t fee) NOEXCEPT
        {
            if (fee > most)
            {
                most = fee;
                out = index;
            }
        });

    return out;
}

inline void algorithm::discard(std::span<mark> marks, const graph_t& graph,
    link_t row) NOEXCEPT
{
    std::vector<link_t> pending{ row };
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

inline void algorithm::forfeit(std::span<mark> marks, const graph_t& graph,
    link_t row) NOEXCEPT
{
    std::vector<link_t> found{ row };
    const auto add = [&](link_t next) NOEXCEPT
    {
        if (std::ranges::find(found, next) == found.end())
            found.push_back(next);
    };

    for (size_t at{}; at < found.size(); ++at)
    {
        const auto member = found[at];
        const auto slice = edges(graph, member);
        for (auto input = slice.first; input < slice.first + slice.count;
            ++input)
        {
            const auto up = parent(graph, input);
            if (up != none && is_labeled(cell(graph, up)))
                add(up);

            each_rival(graph, input, [&](link_t other) NOEXCEPT
            {
                const auto rival_row = owner(graph, other);
                if (is_present(cell(graph, rival_row)))
                    add(rival_row);
            });
        }

        each_child(graph, member, [&](link_t down) NOEXCEPT
        {
            if (is_present(cell(graph, down)))
                add(down);
        });
    }

    for (const auto member: found)
        discard(marks, graph, member);
}

inline void algorithm::pack_(std::vector<link_t>& out, takes_t& takes,
    std::span<mark> marks, const columns_t& columns, const totals_t& budget,
    size_t depth, uint32_t bytes_per_sigop) NOEXCEPT
{
    struct score_t
    {
        link_t row{};
        uint64_t rate{};
        uint64_t fee{};
        size_t count{};
    };

    const graph_t graph{ columns };
    const auto count = columns.cells.size();
    std::vector<score_t> scores{};
    std::vector<link_t> rows{};
    totals_t used{};
    out.clear();
    takes.clear();

    const auto available = [&]() NOEXCEPT -> totals_t
    {
        return
        {
            {},
            floored_subtract(budget.size, used.size),
            floored_subtract(budget.sigops, used.sigops)
        };
    };

    for (auto taken = true; taken;)
    {
        taken = false;
        scores.clear();
        for (size_t at{}; at < count; ++at)
        {
            totals_t sum{};
            const auto row = possible_narrow_cast<link_t>(at);
            if (marks[row] == mark::unmarked &&
                chunk(rows, sum, marks, graph, row, depth, available()))
            {
                const auto rate = fee_rate(sum, bytes_per_sigop);
                scores.push_back({ row, rate, sum.fee, rows.size() });
            }
        }

        std::sort(scores.begin(), scores.end(),
            [](const score_t& left, const score_t& right) NOEXCEPT
            {
                return left.rate == right.rate ? left.row < right.row :
                    left.rate > right.rate;
            });

        for (const auto& score: scores)
        {
            totals_t sum{};
            const auto space = available();
            if (marks[score.row] != mark::unmarked ||
                !chunk(rows, sum, marks, graph, score.row, depth, space) ||
                rows.size() != score.count ||
                sum.fee != score.fee)
                continue;

            for (const auto row: rows)
                marks[row] = mark::taken;

            out.insert(out.end(), rows.begin(), rows.end());
            takes.push_back({ score.rate, chargeable(sum, bytes_per_sigop) });
            used.fee = ceilinged_add(used.fee, sum.fee);
            used.size = ceilinged_add(used.size, sum.size);
            used.sigops = ceilinged_add(used.sigops, sum.sigops);
            taken = true;
        }
    }

    std::sort(out.begin(), out.end());
}

template <typename Arrivals>
inline bool algorithm::reaches(const projection_t& projection, size_t count,
    uint64_t space, uint64_t rate, const Arrivals& arrivals) NOEXCEPT
{
    const auto& blocks = projection.blocks;
    const auto arriving = arrivals(rate);
    uint64_t carry{};

    // A block that leaves rows is full, so its takes are its capacity.
    for (size_t block{}; block < count; ++block)
    {
        std::span<const take_t> takes{};
        if (block < blocks.size())
            takes = blocks[block];

        const auto last = add1(block) >= blocks.size();
        const auto full = !takes.empty() && !(last && projection.exhausted);
        const auto capacity = full ? above(takes, zero) : space;
        const auto demand = ceilinged_add(carry, above(takes, rate));
        if (demand < capacity)
            return true;

        const auto excess = floored_subtract(demand, capacity);
        carry = ceilinged_add(excess, arriving);
    }

    return false;
}

inline bool algorithm::chunk(std::vector<link_t>& out, totals_t& sum,
    const std::span<const mark>& marks, const graph_t& graph, link_t row,
    size_t depth, const totals_t& available) NOEXCEPT
{
    sum = {};
    out.assign(one, row);

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

                if (std::ranges::find(out, up) != out.end())
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
        const auto size = ceilinged_add(sum.size, add.size);
        const auto sigops = ceilinged_add(sum.sigops, add.sigops);
        if (size > available.size || sigops > available.sigops)
            break;

        sum.fee = ceilinged_add(sum.fee, add.fee);
        sum.size = size;
        sum.sigops = sigops;
    }

    out.erase(cut, out.end());
    return !out.empty();
}

inline void algorithm::restore(std::vector<link_t>& out,
    const columns_t& columns, link_t row) NOEXCEPT
{
    const graph_t graph{ columns };
    if (!is_absent(cell(graph, row)) || !confirmable(graph, row))
        return;

    set_cell(graph, row, unlabeled);
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

    for (const auto row: rows)
    {
        // A row relabeled by a previous component carries a lower label.
        if (cell(graph, row) != row)
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
