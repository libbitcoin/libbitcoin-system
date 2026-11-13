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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(pool_algorithm_tests)

class accessor
  : public pool::algorithm
{
public:
    using algorithm::best;
    using algorithm::index;
    using algorithm::losers;
};

using link_t = accessor::link_t;
using mask_t = accessor::mask_t;
using edges_t = accessor::edges_t;
using bridge_t = accessor::bridge_t;
using totals_t = accessor::totals_t;
using columns_t = accessor::columns_t;
using graph_t = accessor::graph_t;
using component_t = accessor::component_t;
using mark = accessor::mark;
constexpr auto none = accessor::none;
constexpr auto absent = accessor::absent;
constexpr auto boundary = accessor::boundary;
constexpr auto unlabeled = accessor::unlabeled;

// Rows: Q=0, A=1, B=2, D=3, P=4. Confirmed X and Y are not rows.
// Inputs: 0 Q spends X:0, 1 A spends X:0, 2 A spends Y:0, 3 B spends Y:0,
// 4 D spends A:0, 5 P spends Q:0. Rings: X:0 (0, 1), Y:0 (2, 3).
constexpr size_t rows = 5;
constexpr size_t inputs = 6;

struct state
{
    std_array<link_t, rows> cells{ { 0, 0, 0, 0, 0 } };
    std_array<link_t, rows> children{ { 5, 4, none, none, none } };
    std_array<edges_t, rows> edges
    {
        { { 0, 1 }, { 1, 2 }, { 3, 1 }, { 4, 1 }, { 5, 1 } }
    };
    std_array<totals_t, rows> totals
    {
        {
            { 100, 100, 1 }, { 300, 100, 1 }, { 150, 100, 1 },
            { 200, 100, 1 }, { 400, 100, 1 }
        }
    };
    std_array<link_t, inputs> parents{ { none, none, none, none, 1, 0 } };
    std_array<link_t, inputs> siblings
    {
        { none, none, none, none, none, none }
    };
    std_array<link_t, inputs> rivals{ { 1, 0, 3, 2, none, none } };
    std_array<link_t, inputs> scratch{};

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: M=0 (parent none), N=1 spends M:0 and X:0, O=2 spends X:0.
struct chain_state
{
    std_array<link_t, 3> cells{ { 0, 0, 0 } };
    std_array<link_t, 3> children{ { 0, none, none } };
    std_array<edges_t, 3> edges{ { { 0, 0 }, { 0, 2 }, { 2, 1 } } };
    std_array<totals_t, 3> totals
    {
        { { 10, 100, 1 }, { 20, 100, 1 }, { 30, 100, 1 } }
    };
    std_array<link_t, 3> parents{ { 0, none, none } };
    std_array<link_t, 3> siblings{ { none, none, none } };
    std_array<link_t, 3> rivals{ { none, 2, 1 } };
    std_array<link_t, 3> scratch{};

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: R=0, S=1 spends R:0, T=2 spends S:0, each uncontested.
struct line_state
{
    std_array<link_t, 3> cells{ { unlabeled, unlabeled, unlabeled } };
    std_array<link_t, 3> children{ { 1, 2, none } };
    std_array<edges_t, 3> edges{ { { 0, 1 }, { 1, 1 }, { 2, 1 } } };
    std_array<totals_t, 3> totals
    {
        { { 10, 100, 1 }, { 10, 100, 1 }, { 400, 100, 1 } }
    };
    std_array<link_t, 3> parents{ { none, 0, 1 } };
    std_array<link_t, 3> siblings{ { none, none, none } };
    std_array<link_t, 3> rivals{ { none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

template <typename Word>
static state label() NOEXCEPT
{
    state out{};
    accessor::initialize(out.columns());
    while (accessor::propagate<Word>(out.columns(), out.scratch));
    return out;
}

static state label_without_a() NOEXCEPT
{
    state out{};
    out.cells[1] = absent;
    accessor::initialize(out.columns());
    while (accessor::propagate<link_t>(out.columns(), out.scratch));
    return out;
}

static chain_state label_chain() NOEXCEPT
{
    chain_state out{};
    accessor::initialize(out.columns());
    while (accessor::propagate<link_t>(out.columns(), out.scratch));
    return out;
}

static component_t component(state& labeled) NOEXCEPT
{
    component_t out{};
    const graph_t graph{ labeled.columns() };
    accessor::collect(out, graph, 4);
    accessor::relate(out, graph);
    return out;
}

static std_array<mask_t, 4> outcomes_of(const component_t& out) NOEXCEPT
{
    std_array<mask_t, 4> masks{};
    size_t index{};
    std::generate(masks.begin(), masks.end(), [&]() NOEXCEPT
    {
        return accessor::outcome(out, index++);
    });

    return masks;
}

// Lane comparisons return true where the word is unavailable.
template <typename Word>
static bool gathers_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
    {
        std_array<link_t, 37> values{};
        std_array<link_t, 37> indexes{};
        size_t at{};
        std::generate(values.begin(), values.end(), [&]() NOEXCEPT
        {
            return possible_narrow_cast<link_t>(at++ * 7);
        });

        at = zero;
        std::generate(indexes.begin(), indexes.end(), [&]() NOEXCEPT
        {
            const auto index = at++;
            return is_zero(index % 5) ? none :
                possible_narrow_cast<link_t>((index * 11) % values.size());
        });

        std_array<link_t, 37> expected{};
        std_array<link_t, 37> out{};
        accessor::gather<link_t>(expected, values, indexes);
        accessor::gather<Word>(out, values, indexes);
        return out == expected;
    }
    else
    {
        return true;
    }
}

template <typename Word>
static bool propagates_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
        return label<Word>().cells == label<link_t>().cells;
    else
        return true;
}

template <typename Word>
static bool collects_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
    {
        std_array<link_t, 69> cells{};
        size_t at{};
        std::generate(cells.begin(), cells.end(), [&]() NOEXCEPT
        {
            return possible_narrow_cast<link_t>(at++ % 3);
        });

        component_t expected{};
        component_t out{};
        return accessor::collect<link_t>(expected, 1, cells) &&
            accessor::collect<Word>(out, 1, cells) &&
            out.rows == expected.rows && out.count == expected.count;
    }
    else
    {
        return true;
    }
}

template <typename Word>
static bool finds_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
    {
        std_array<link_t, 37> values{};
        size_t at{};
        std::generate(values.begin(), values.end(), [&]() NOEXCEPT
        {
            return possible_narrow_cast<link_t>(at++ % 4);
        });

        std::vector<link_t> expected{};
        std::vector<link_t> out{};
        accessor::find<link_t>(expected, values, 3);
        accessor::find<Word>(out, values, 3);
        return out == expected;
    }
    else
    {
        return true;
    }
}

template <typename Word>
static bool evaluates_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
    {
        auto labeled = label<link_t>();
        const auto out = component(labeled);
        const graph_t graph{ labeled.columns() };
        std_array<mask_t, 19> masks{};
        size_t at{};
        std::generate(masks.begin(), masks.end(), [&]() NOEXCEPT
        {
            return accessor::outcome(out, at++ % 4);
        });

        const std_array<link_t, 1> candidates{ { 4 } };
        const auto candidate = accessor::members(out, candidates);
        uint64_t best{}, best_without{}, best_x{}, best_without_x{};
        accessor::evaluate<uint64_t>(best, best_without, masks, candidate,
            out, graph);
        accessor::evaluate<Word>(best_x, best_without_x, masks, candidate,
            out, graph);
        return best_x == best && best_without_x == best_without;
    }
    else
    {
        return true;
    }
}

template <typename Word>
static bool bests_as_integral() NOEXCEPT
{
    if constexpr (have_<Word>())
    {
        auto labeled = label<link_t>();
        const auto out = component(labeled);
        const graph_t graph{ labeled.columns() };
        std_array<mask_t, 19> masks{};
        size_t at{};
        std::generate(masks.begin(), masks.end(), [&]() NOEXCEPT
        {
            return accessor::outcome(out, at++ % 4);
        });

        return accessor::best<Word>(masks, out, graph) ==
            accessor::best<uint64_t>(masks, out, graph);
    }
    else
    {
        return true;
    }
}

// The template rows of the resolved scenario within the budget size.
static std::vector<link_t> template_of(uint32_t size) NOEXCEPT
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    std::vector<link_t> out{};
    accessor::resolve<link_t>(marks, labeled.columns(), 1'024);
    accessor::pack(out, marks, labeled.columns(), { 0, size, 100 }, 25, 20);
    return out;
}

// A table of the scenario less P, with P as the candidate (row 4, input 5
// spending Q:0), bridged from Q.
struct child_candidate
{
    state labeled{ label<link_t>() };
    std_array<link_t, 4> cells
    {
        {
            labeled.cells[0], labeled.cells[1], labeled.cells[2],
            labeled.cells[3]
        }
    };
    std_array<link_t, 4> children{ { none, 4, none, none } };
    std_array<edges_t, 4> edges{ { { 0, 1 }, { 1, 2 }, { 3, 1 }, { 4, 1 } } };
    std_array<link_t, 5> parents{ { none, none, none, none, 1 } };
    std_array<link_t, 5> siblings{ { none, none, none, none, none } };
    std_array<link_t, 5> rivals{ { 1, 0, 3, 2, none } };
    std_array<link_t, 1> extra_cells{ { absent } };
    std_array<link_t, 1> extra_children{ { none } };
    std_array<edges_t, 1> extra_edges{ { { 5, 1 } } };
    std_array<totals_t, 1> extra_totals{ { { 400, 100, 1 } } };
    std_array<link_t, 1> extra_parents{ { 0 } };
    std_array<link_t, 1> extra_siblings{ { none } };
    std_array<link_t, 1> extra_rivals{ { none } };
    std_array<bridge_t, 1> bridges{ { { 0, 5 } } };
    std_array<link_t, 1> candidates{ { 4 } };

    graph_t graph() NOEXCEPT
    {
        const columns_t table
        {
            cells, children, edges, labeled.totals, parents, siblings, rivals
        };
        const columns_t extra
        {
            extra_cells, extra_children, extra_edges, extra_totals,
            extra_parents, extra_siblings, extra_rivals
        };
        return { table, extra, bridges, {} };
    }
};

// The full scenario with a candidate C (row 5, input 6) spending X:0,
// bridged from input 0.
struct rival_candidate
{
    state labeled{ label<link_t>() };
    std_array<link_t, 1> extra_cells{ { absent } };
    std_array<link_t, 1> extra_children{ { none } };
    std_array<edges_t, 1> extra_edges{ { { 6, 1 } } };
    std_array<totals_t, 1> extra_totals{ { { 500, 100, 1 } } };
    std_array<link_t, 1> extra_parents{ { none } };
    std_array<link_t, 1> extra_siblings{ { none } };
    std_array<link_t, 1> extra_rivals{ { 0 } };
    std_array<bridge_t, 1> bridges{ { { 0, 6 } } };
    std_array<link_t, 1> candidates{ { 5 } };

    graph_t graph() NOEXCEPT
    {
        const columns_t extra
        {
            extra_cells, extra_children, extra_edges, extra_totals,
            extra_parents, extra_siblings, extra_rivals
        };
        return { labeled.columns(), extra, {}, bridges };
    }
};

// The full scenario with a candidate (row 5, input 6) spending confirmed Z:0.
struct lone_candidate
{
    state labeled{ label<link_t>() };
    std_array<link_t, 1> extra_cells{ { absent } };
    std_array<link_t, 1> extra_children{ { none } };
    std_array<edges_t, 1> extra_edges{ { { 6, 1 } } };
    std_array<totals_t, 1> extra_totals{ { { 500, 100, 1 } } };
    std_array<link_t, 1> extra_parents{ { none } };
    std_array<link_t, 1> extra_siblings{ { none } };
    std_array<link_t, 1> extra_rivals{ { none } };
    std_array<link_t, 1> candidates{ { 5 } };

    graph_t graph() NOEXCEPT
    {
        const columns_t extra
        {
            extra_cells, extra_children, extra_edges, extra_totals,
            extra_parents, extra_siblings, extra_rivals
        };
        return { labeled.columns(), extra, {}, {} };
    }
};

// cells
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__cells__states__expected)
{
    BC_REQUIRE(accessor::is_present(0));
    BC_REQUIRE(accessor::is_present(unlabeled));
    BC_REQUIRE(!accessor::is_present(absent));
    BC_REQUIRE(!accessor::is_present(boundary));
    BC_REQUIRE(!accessor::is_present(none));
    BC_REQUIRE(accessor::is_labeled(0));
    BC_REQUIRE(!accessor::is_labeled(unlabeled));
    BC_REQUIRE(!accessor::is_labeled(absent));
    BC_REQUIRE(accessor::is_boundary(boundary));
    BC_REQUIRE(!accessor::is_boundary(absent));
}

BC_AUTO_TEST_CASE(pool_algorithm__chargeable__size_or_sigops__maximum)
{
    BC_REQUIRE_EQUAL(accessor::chargeable({ 0, 100, 1 }, 20), 100u);
    BC_REQUIRE_EQUAL(accessor::chargeable({ 0, 100, 6 }, 20), 120u);
}

// gather
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__gather__integral__expected)
{
    const std_array<link_t, 6> values{ { 10, 11, 12, 13, 14, 15 } };
    const std_array<link_t, 5> indexes{ { 5, 3, none, 0, 4 } };
    std_array<link_t, 5> out{};
    accessor::gather<link_t>(out, values, indexes);
    BC_REQUIRE_EQUAL(out[0], 15u);
    BC_REQUIRE_EQUAL(out[1], 13u);
    BC_REQUIRE_EQUAL(out[2], none);
    BC_REQUIRE_EQUAL(out[3], 10u);
    BC_REQUIRE_EQUAL(out[4], 14u);
}

BC_AUTO_TEST_CASE(pool_algorithm__gather__extended__same_as_integral)
{
    BC_REQUIRE(gathers_as_integral<xint128_t>());
    BC_REQUIRE(gathers_as_integral<xint256_t>());
    BC_REQUIRE(gathers_as_integral<xint512_t>());
}

// initialize/propagate (columns)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__initialize__columns__contested_labeled)
{
    state out{};
    accessor::initialize(out.columns());
    BC_REQUIRE_EQUAL(out.cells[0], 0u);
    BC_REQUIRE_EQUAL(out.cells[1], 1u);
    BC_REQUIRE_EQUAL(out.cells[2], 2u);
    BC_REQUIRE_EQUAL(out.cells[3], unlabeled);
    BC_REQUIRE_EQUAL(out.cells[4], unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__connected__one_label)
{
    const auto labeled = label<link_t>();
    BC_REQUIRE_EQUAL(labeled.cells[0], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[1], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[2], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[3], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[4], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__absent_row__nothing_contested)
{
    const auto out = label_without_a();
    BC_REQUIRE_EQUAL(out.cells[0], unlabeled);
    BC_REQUIRE_EQUAL(out.cells[1], absent);
    BC_REQUIRE_EQUAL(out.cells[2], unlabeled);
    BC_REQUIRE_EQUAL(out.cells[3], unlabeled);
    BC_REQUIRE_EQUAL(out.cells[4], unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__uncontested_ancestor__not_labeled)
{
    const auto out = label_chain();
    BC_REQUIRE_EQUAL(out.cells[0], unlabeled);
    BC_REQUIRE_EQUAL(out.cells[1], 1u);
    BC_REQUIRE_EQUAL(out.cells[2], 1u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__extended__same_as_integral)
{
    BC_REQUIRE(propagates_as_integral<xint128_t>());
    BC_REQUIRE(propagates_as_integral<xint256_t>());
    BC_REQUIRE(propagates_as_integral<xint512_t>());
}

// collect (frontier and columns)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__collect__frontier__all_rows_sorted)
{
    auto labeled = label<link_t>();
    const graph_t graph{ labeled.columns() };
    component_t out{};
    BC_REQUIRE(accessor::collect(out, graph, 3));
    BC_REQUIRE_EQUAL(out.count, rows);
    BC_REQUIRE_EQUAL(out.rows[0], 0u);
    BC_REQUIRE_EQUAL(out.rows[1], 1u);
    BC_REQUIRE_EQUAL(out.rows[2], 2u);
    BC_REQUIRE_EQUAL(out.rows[3], 3u);
    BC_REQUIRE_EQUAL(out.rows[4], 4u);
}

BC_AUTO_TEST_CASE(pool_algorithm__collect__frontier_absent_rival__excluded)
{
    auto labeled = label<link_t>();
    labeled.cells[2] = absent;
    const graph_t graph{ labeled.columns() };
    component_t out{};
    BC_REQUIRE(accessor::collect(out, graph, 4));
    BC_REQUIRE_EQUAL(out.count, 4u);
    BC_REQUIRE_EQUAL(accessor::index(out, 2), accessor::maximum_members);
}

BC_AUTO_TEST_CASE(pool_algorithm__collect__columns_label__all_rows)
{
    const auto labeled = label<link_t>();
    component_t out{};
    BC_REQUIRE(accessor::collect<link_t>(out, 0, labeled.cells));
    BC_REQUIRE_EQUAL(out.count, rows);
    BC_REQUIRE_EQUAL(out.rows[4], 4u);
    BC_REQUIRE(accessor::collect<link_t>(out, 1, labeled.cells));
    BC_REQUIRE_EQUAL(out.count, 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__collect__columns_extended__same_as_integral)
{
    BC_REQUIRE(collects_as_integral<xint128_t>());
    BC_REQUIRE(collects_as_integral<xint256_t>());
    BC_REQUIRE(collects_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__collect__columns_too_many__false)
{
    std_array<link_t, add1(accessor::maximum_members)> cells{};
    component_t out{};
    BC_REQUIRE(!accessor::collect<link_t>(out, 0, cells));
}

// find
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__find__repeated_value__all_indexes)
{
    const std_array<link_t, 9> values{ { 3, 0, 3, 1, 3, 2, 3, 3, 0 } };
    std::vector<link_t> out{};
    accessor::find<link_t>(out, values, 3);
    BC_REQUIRE_EQUAL(out.size(), 5u);
    BC_REQUIRE_EQUAL(out.front(), 0u);
    BC_REQUIRE_EQUAL(out.back(), 7u);
}

BC_AUTO_TEST_CASE(pool_algorithm__find__extended__same_as_integral)
{
    BC_REQUIRE(finds_as_integral<xint128_t>());
    BC_REQUIRE(finds_as_integral<xint256_t>());
    BC_REQUIRE(finds_as_integral<xint512_t>());
}

// relate/members/ancestry
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__relate__component__ancestors_and_cliques)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    BC_REQUIRE_EQUAL(out.ancestors[0], 0u);
    BC_REQUIRE_EQUAL(out.ancestors[1], 0u);
    BC_REQUIRE_EQUAL(out.ancestors[2], 0u);
    BC_REQUIRE_EQUAL(out.ancestors[3], 0b00010u);
    BC_REQUIRE_EQUAL(out.ancestors[4], 0b00001u);
    BC_REQUIRE_EQUAL(out.contests, 2u);
    BC_REQUIRE_EQUAL(out.cliques[0], 0b00011u);
    BC_REQUIRE_EQUAL(out.cliques[1], 0b00110u);
}

BC_AUTO_TEST_CASE(pool_algorithm__members__rows__mask_of_members)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const std_array<link_t, 3> candidates{ { 4, 2, 9 } };
    BC_REQUIRE_EQUAL(accessor::members(out, candidates), 0b10100u);
}

BC_AUTO_TEST_CASE(pool_algorithm__ancestry__component__totals_within_limit)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    totals_t sum{};
    totals_t over_size{};
    totals_t over_sigops{};
    const auto fits = accessor::ancestry(sum, graph, out, { 0, 500, 5 });
    const auto exceeds_size = accessor::ancestry(over_size, graph, out, { 0, 499, 5 });
    const auto exceeds_sigops = accessor::ancestry(over_sigops, graph, out, { 0, 500, 4 });
    BC_REQUIRE(fits);
    BC_REQUIRE_EQUAL(sum.fee, 1'150u);
    BC_REQUIRE_EQUAL(sum.size, 500u);
    BC_REQUIRE_EQUAL(sum.sigops, 5u);
    BC_REQUIRE(!exceeds_size);
    BC_REQUIRE(!exceeds_sigops);
}

BC_AUTO_TEST_CASE(pool_algorithm__ancestry__uncontested_ancestor__included)
{
    auto chain = label_chain();
    const graph_t graph{ chain.columns() };
    component_t out{};
    totals_t sum{};
    const auto collected = accessor::collect(out, graph, 2);
    const auto fits = accessor::ancestry(sum, graph, out, { 0, 1'000, 10 });
    BC_REQUIRE(collected);
    BC_REQUIRE_EQUAL(out.count, 2u);
    BC_REQUIRE(fits);
    BC_REQUIRE_EQUAL(sum.fee, 60u);
    BC_REQUIRE_EQUAL(sum.size, 300u);
    BC_REQUIRE_EQUAL(sum.sigops, 3u);
}

// candidates (graph)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__initialize__candidate_child__collected)
{
    child_candidate scenario{};
    const auto graph = scenario.graph();
    component_t from_candidate{};
    component_t from_table{};
    accessor::initialize(graph, scenario.candidates);
    const auto collected_candidate = accessor::collect(from_candidate, graph, 4);
    const auto collected_table = accessor::collect(from_table, graph, 2);
    const auto related = accessor::relate(from_table, graph);
    BC_REQUIRE_EQUAL(scenario.extra_cells[0], 4u);
    BC_REQUIRE(collected_candidate);
    BC_REQUIRE_EQUAL(from_candidate.count, rows);
    BC_REQUIRE_EQUAL(from_candidate.rows[4], 4u);
    BC_REQUIRE(collected_table);
    BC_REQUIRE_EQUAL(from_table.count, rows);
    BC_REQUIRE(related);
    BC_REQUIRE_EQUAL(from_table.ancestors[4], 0b00001u);
}

BC_AUTO_TEST_CASE(pool_algorithm__initialize__candidate_rival__clique_of_three)
{
    rival_candidate scenario{};
    const auto graph = scenario.graph();
    component_t out{};
    component_t from_table{};
    accessor::initialize(graph, scenario.candidates);
    const auto collected = accessor::collect(out, graph, 5);
    const auto related = accessor::relate(out, graph);
    const auto collected_table = accessor::collect(from_table, graph, 1);
    BC_REQUIRE_EQUAL(scenario.extra_cells[0], 5u);
    BC_REQUIRE(collected);
    BC_REQUIRE_EQUAL(out.count, 6u);
    BC_REQUIRE(related);
    BC_REQUIRE_EQUAL(out.contests, 2u);
    BC_REQUIRE_EQUAL(out.cliques[0], 0b100011u);
    BC_REQUIRE_EQUAL(out.cliques[1], 0b000110u);
    BC_REQUIRE_EQUAL(accessor::outcomes(out), 6u);
    BC_REQUIRE(collected_table);
    BC_REQUIRE_EQUAL(from_table.count, 6u);
}

BC_AUTO_TEST_CASE(pool_algorithm__initialize__uncontested_candidate__unlabeled)
{
    lone_candidate scenario{};
    const auto graph = scenario.graph();
    accessor::initialize(graph, scenario.candidates);
    BC_REQUIRE_EQUAL(scenario.extra_cells[0], unlabeled);
}

// outcomes
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__outcomes__two_cliques__product)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    BC_REQUIRE_EQUAL(accessor::outcomes(out), 4u);
}

BC_AUTO_TEST_CASE(pool_algorithm__outcome__each_index__closed_and_conflict_free)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    BC_REQUIRE_EQUAL(accessor::losers(out, 0), 0b00110u);
    BC_REQUIRE_EQUAL(accessor::outcome(out, 0), 0b10001u);
    BC_REQUIRE_EQUAL(accessor::outcome(out, 1), 0b01010u);
    BC_REQUIRE_EQUAL(accessor::outcome(out, 2), 0b10101u);
    BC_REQUIRE_EQUAL(accessor::outcome(out, 3), 0b00100u);
}

BC_AUTO_TEST_CASE(pool_algorithm__evaluate__candidate_p__best_and_best_without)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    const auto masks = outcomes_of(out);
    const std_array<link_t, 1> candidates{ { 4 } };
    const auto candidate = accessor::members(out, candidates);
    uint64_t best{}, best_without{};
    accessor::evaluate<uint64_t>(best, best_without, masks, candidate, out, graph);
    BC_REQUIRE_EQUAL(best, 650u);
    BC_REQUIRE_EQUAL(best_without, 500u);
}

BC_AUTO_TEST_CASE(pool_algorithm__evaluate__extended__same_as_integral)
{
    BC_REQUIRE(evaluates_as_integral<xint128_t>());
    BC_REQUIRE(evaluates_as_integral<xint256_t>());
    BC_REQUIRE(evaluates_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__covered__added_fee_at_rate__expected)
{
    BC_REQUIRE(accessor::covered(650, 500, 100, 1'500));
    BC_REQUIRE(!accessor::covered(650, 500, 100, 1'501));
    BC_REQUIRE(accessor::covered(500, 650, 0, 1));
    BC_REQUIRE(!accessor::covered(500, 650, 1, 1));
}

// template
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__best__scenario__highest_fee_index)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    const auto masks = outcomes_of(out);
    BC_REQUIRE_EQUAL(accessor::best<uint64_t>(masks, out, graph), 2u);
}

BC_AUTO_TEST_CASE(pool_algorithm__best__extended__same_as_integral)
{
    BC_REQUIRE(bests_as_integral<xint128_t>());
    BC_REQUIRE(bests_as_integral<xint256_t>());
    BC_REQUIRE(bests_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__scenario__highest_fee_outcome)
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 1'024);
    BC_REQUIRE(marks[0] == mark::unmarked);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::unmarked);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__outcome_limit__best_of_first)
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 2);
    BC_REQUIRE(marks[0] == mark::unmarked);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::excluded);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__no_outcomes__component_excluded)
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 0);
    BC_REQUIRE(marks[0] == mark::excluded);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::excluded);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::excluded);
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__boundary_row__excluded)
{
    line_state line{};
    line.cells[0] = boundary;
    std_array<mark, 3> marks{};
    accessor::resolve<link_t>(marks, line.columns(), 1'024);
    BC_REQUIRE(marks[0] == mark::excluded);
    BC_REQUIRE(marks[1] == mark::unmarked);
    BC_REQUIRE(marks[2] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__resolved_scenario__winners_ascending)
{
    const auto out = template_of(1'000);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 2u);
    BC_REQUIRE_EQUAL(out[2], 4u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__two_rows__highest_rate_chunk)
{
    const auto out = template_of(200);
    BC_REQUIRE_EQUAL(out.size(), 2u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 4u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__one_row__split_chunk_outrated)
{
    const auto out = template_of(100);
    BC_REQUIRE_EQUAL(out.size(), 1u);
    BC_REQUIRE_EQUAL(out[0], 2u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__chain__whole_chunk)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), { 0, 1'000, 100 }, 2, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE_EQUAL(out[2], 2u);
    BC_REQUIRE(marks[0] == mark::taken);
    BC_REQUIRE(marks[1] == mark::taken);
    BC_REQUIRE(marks[2] == mark::taken);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__chain_deeper_than_depth__taken_in_rounds)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), { 0, 1'000, 100 }, 0, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE_EQUAL(out[2], 2u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__chain_over_size__leading_rows)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), { 0, 250, 100 }, 2, 20);
    BC_REQUIRE_EQUAL(out.size(), 2u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE(marks[2] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__chain_over_sigops__leading_rows)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), { 0, 1'000, 2 }, 2, 20);
    BC_REQUIRE_EQUAL(out.size(), 2u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__excluded_parent__descendant_not_taken)
{
    line_state line{};
    std_array<mark, 3> marks{ { mark::unmarked, mark::excluded, mark::unmarked } };
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), { 0, 1'000, 100 }, 2, 20);
    BC_REQUIRE_EQUAL(out.size(), 1u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE(marks[2] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__empty_budget__empty)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), {}, 2, 20);
    BC_REQUIRE(out.empty());
}

// transitions
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__exclude__row__descendants_absent)
{
    auto labeled = label<link_t>();
    accessor::exclude(labeled.columns(), 1);
    BC_REQUIRE_EQUAL(labeled.cells[0], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[1], absent);
    BC_REQUIRE_EQUAL(labeled.cells[2], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[3], absent);
    BC_REQUIRE_EQUAL(labeled.cells[4], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__admit__contested_row__labeled_into_component)
{
    auto labeled = label<link_t>();
    labeled.cells = { { 0, 0, 0, 0, absent } };
    accessor::admit(labeled.columns(), 4);
    BC_REQUIRE_EQUAL(labeled.cells[4], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__q__rivals_excluded_rest_unlabeled)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), 0);
    BC_REQUIRE_EQUAL(labeled.cells[0], boundary);
    BC_REQUIRE_EQUAL(labeled.cells[1], absent);
    BC_REQUIRE_EQUAL(labeled.cells[2], unlabeled);
    BC_REQUIRE_EQUAL(labeled.cells[3], absent);
    BC_REQUIRE_EQUAL(labeled.cells[4], unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__b__q_loses_rival_unlabeled)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), 2);
    BC_REQUIRE_EQUAL(labeled.cells[0], unlabeled);
    BC_REQUIRE_EQUAL(labeled.cells[1], absent);
    BC_REQUIRE_EQUAL(labeled.cells[2], boundary);
    BC_REQUIRE_EQUAL(labeled.cells[3], absent);
    BC_REQUIRE_EQUAL(labeled.cells[4], unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__unconfirm__q__component_restored)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), 0);
    accessor::unconfirm(labeled.columns(), 0);
    BC_REQUIRE_EQUAL(labeled.cells[0], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[1], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[2], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[3], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[4], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__unconfirm__other_boundary__rival_stays_absent)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), 2);
    accessor::confirm(labeled.columns(), 0);
    accessor::unconfirm(labeled.columns(), 0);
    BC_REQUIRE_EQUAL(labeled.cells[0], unlabeled);
    BC_REQUIRE_EQUAL(labeled.cells[1], absent);
    BC_REQUIRE_EQUAL(labeled.cells[2], boundary);
    BC_REQUIRE_EQUAL(labeled.cells[3], absent);
    BC_REQUIRE_EQUAL(labeled.cells[4], unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__revive__absent_chain__relabeled)
{
    auto labeled = label<link_t>();
    accessor::exclude(labeled.columns(), 1);
    accessor::revive(labeled.columns(), 1);
    BC_REQUIRE_EQUAL(labeled.cells[0], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[1], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[2], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[3], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[4], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__revive__present__unchanged)
{
    auto labeled = label<link_t>();
    accessor::revive(labeled.columns(), 1);
    BC_REQUIRE_EQUAL(labeled.cells[1], 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__relabel__component__lowest_row)
{
    auto labeled = label<link_t>();
    labeled.cells = { { 0, 1, 2, 3, 4 } };
    component_t out{};
    out.count = 3;
    out.rows = { { 2, 3, 4 } };
    accessor::relabel(labeled.columns(), out);
    BC_REQUIRE_EQUAL(labeled.cells[0], 0u);
    BC_REQUIRE_EQUAL(labeled.cells[1], 1u);
    BC_REQUIRE_EQUAL(labeled.cells[2], 2u);
    BC_REQUIRE_EQUAL(labeled.cells[3], 2u);
    BC_REQUIRE_EQUAL(labeled.cells[4], 2u);
}

BC_AUTO_TEST_SUITE_END()
