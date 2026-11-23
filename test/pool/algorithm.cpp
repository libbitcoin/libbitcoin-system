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

#include <random>

BC_AUTO_TEST_SUITE(pool_algorithm_tests)

class accessor
  : public pool::algorithm
{
public:
    using algorithm::best;
    using algorithm::family;
    using algorithm::greedy;
    using algorithm::index;
    using algorithm::losers;
    using algorithm::totals;
    using algorithm::worth;
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
using take_t = accessor::take_t;
using projection_t = accessor::projection_t;
constexpr auto none = accessor::none;
constexpr auto absent = accessor::absent;
constexpr auto boundary = accessor::boundary;
constexpr auto unlabeled = accessor::unlabeled;

// The column as stored (little-endian).
template <typename Column>
static Column stored(Column column) NOEXCEPT
{
    for (auto& value: column)
        value = accessor::to_column(value);

    return column;
}

// The value of a stored cell.
static link_t cell(const std::span<const link_t>& cells, size_t row) NOEXCEPT
{
    return accessor::from_column(cells[row]);
}

// Rows: Q=0, A=1, B=2, D=3, P=4. Confirmed X and Y are not rows.
// Inputs: 0 Q spends X:0, 1 A spends X:0, 2 A spends Y:0, 3 B spends Y:0,
// 4 D spends A:0, 5 P spends Q:0. Rings: X:0 (0, 1), Y:0 (2, 3).
constexpr size_t rows = 5;
constexpr size_t inputs = 6;

struct state
{
    state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

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

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: M=0 (parent none), N=1 spends M:0 and X:0, O=2 spends X:0.
struct chain_state
{
    chain_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

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

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: R=0, S=1 spends R:0, T=2 spends S:0, each uncontested.
struct line_state
{
    line_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

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

// Rows: P=0 (no fee), C=1 and D=2 (spend P), Q=3. Inputs: 0 P spends X:0,
// 1 C spends P:0, 2 D spends P:1, 3 Q spends Y:0.
struct fork_state
{
    fork_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 4> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 4> children{ { 1, none, none, none } };
    std_array<edges_t, 4> edges{ { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } } };
    std_array<totals_t, 4> totals
    {
        { { 0, 100, 0 }, { 60, 100, 0 }, { 60, 100, 0 }, { 35, 100, 0 } }
    };
    std_array<link_t, 4> parents{ { none, 0, 0, none } };
    std_array<link_t, 4> siblings{ { none, 2, none, none } };
    std_array<link_t, 4> rivals{ { none, none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: P=0 (no fee), A=1, B=2 and C=3 (spend P), Z=4. Inputs: 0 P spends X:0,
// 1 A spends P:0, 2 B spends P:1, 3 C spends P:2, 4 Z spends Y:0.
struct tail_state
{
    tail_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 5> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 5> children{ { 1, none, none, none, none } };
    std_array<edges_t, 5> edges
    {
        { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 } }
    };
    std_array<totals_t, 5> totals
    {
        {
            { 0, 100, 0 }, { 300, 50, 0 }, { 300, 50, 0 }, { 300, 50, 0 },
            { 200, 80, 0 }
        }
    };
    std_array<link_t, 5> parents{ { none, 0, 0, 0, none } };
    std_array<link_t, 5> siblings{ { none, 2, 3, none, none } };
    std_array<link_t, 5> rivals{ { none, none, none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: A=0, B=1 spends A:0, Q=2. Inputs: 0 A spends X:0, 1 B spends A:0,
// 2 Q spends Y:0.
struct deferred_state
{
    deferred_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 3> cells{ { unlabeled, unlabeled, unlabeled } };
    std_array<link_t, 3> children{ { 1, none, none } };
    std_array<edges_t, 3> edges{ { { 0, 1 }, { 1, 1 }, { 2, 1 } } };
    std_array<totals_t, 3> totals
    {
        { { 70, 100, 0 }, { 60, 100, 0 }, { 50, 100, 0 } }
    };
    std_array<link_t, 3> parents{ { none, 0, none } };
    std_array<link_t, 3> siblings{ { none, none, none } };
    std_array<link_t, 3> rivals{ { none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: E=0, L=1, J=2 and K=3 (spend L), M=4 (spends E and L), Q=5.
// Inputs: 0 E spends Z:0, 1 L spends X:0, 2 J spends L:0, 3 K spends L:1,
// 4 M spends E:0, 5 M spends L:2, 6 Q spends Y:0.
struct partial_state
{
    partial_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 6> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 6> children{ { 4, 2, none, none, none, none } };
    std_array<edges_t, 6> edges
    {
        { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 2 }, { 6, 1 } }
    };
    std_array<totals_t, 6> totals
    {
        {
            { 0, 100, 0 }, { 0, 100, 0 }, { 100, 100, 0 }, { 70, 100, 0 },
            { 10, 100, 0 }, { 40, 100, 0 }
        }
    };
    std_array<link_t, 7> parents{ { none, none, 1, 1, 0, 1, none } };
    std_array<link_t, 7> siblings{ { none, none, 3, 5, none, none, none } };
    std_array<link_t, 7> rivals{ { none, none, none, none, none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: X=0, P=1, C=2 (spends P), D=3 (spends P and X), E=4.
// Inputs: 0 X spends W:0, 1 P spends W:1, 2 C spends P:0, 3 D spends P:1,
// 4 D spends X:0, 5 E spends W:2. C is charged for its sigops.
struct charged_state
{
    charged_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 5> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 5> children{ { 4, 2, none, none, none } };
    std_array<edges_t, 5> edges
    {
        { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 2 }, { 5, 1 } }
    };
    std_array<totals_t, 5> totals
    {
        {
            { 0, 100, 0 }, { 1'000, 1'000, 0 }, { 500, 10, 200 },
            { 1, 100, 0 }, { 1'200, 1'000, 0 }
        }
    };
    std_array<link_t, 6> parents{ { none, none, 1, 1, 0, none } };
    std_array<link_t, 6> siblings{ { none, none, 3, none, none, none } };
    std_array<link_t, 6> rivals{ { none, none, none, none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: S=0, L=1, M=2 (spends L), N=3 (spends M).
// Inputs: 0 S spends X:0, 1 L spends Y:0, 2 M spends L:0, 3 N spends M:0.
struct apart_state
{
    apart_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 4> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 4> children{ { none, 2, 3, none } };
    std_array<edges_t, 4> edges{ { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } } };
    std_array<totals_t, 4> totals
    {
        { { 500, 100, 0 }, { 10, 100, 0 }, { 10, 100, 0 }, { 400, 100, 0 } }
    };
    std_array<link_t, 4> parents{ { none, none, 1, 2 } };
    std_array<link_t, 4> siblings{ { none, none, none, none } };
    std_array<link_t, 4> rivals{ { none, none, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: A=0, B=1, C=2 (spends B), D=3. Inputs: 0 A spends X:0,
// 1 B spends X:0, 2 C spends B:0, 3 C spends Y:0, 4 D spends Y:0.
// Rings: X:0 (0, 1), Y:0 (3, 4).
struct descent_state
{
    descent_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 4> cells
    {
        { unlabeled, unlabeled, unlabeled, unlabeled }
    };
    std_array<link_t, 4> children{ { none, 2, none, none } };
    std_array<edges_t, 4> edges{ { { 0, 1 }, { 1, 1 }, { 2, 2 }, { 4, 1 } } };
    std_array<totals_t, 4> totals
    {
        { { 300, 100, 1 }, { 100, 100, 1 }, { 500, 100, 1 }, { 200, 100, 1 } }
    };
    std_array<link_t, 5> parents{ { none, none, 1, none, none } };
    std_array<link_t, 5> siblings{ { none, none, none, none, none } };
    std_array<link_t, 5> rivals{ { 1, 0, none, 4, 3 } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: U=0, V=1, W=2, Z=3, K=4. Confirmed X and Y are not rows.
// Inputs: 0 U spends X:0, 1 V spends X:0, 2 W spends Y:0, 3 Z spends Y:0,
// 4 K spends U:0, 5 K spends W:0. Rings: X:0 (0, 1), Y:0 (2, 3).
struct merge_state
{
    merge_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 5> cells{ { 0, 0, 0, 0, 0 } };
    std_array<link_t, 5> children{ { 4, none, 5, none, none } };
    std_array<edges_t, 5> edges
    {
        { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 2 } }
    };
    std_array<totals_t, 5> totals
    {
        {
            { 10, 100, 1 }, { 10, 100, 1 }, { 10, 100, 1 }, { 10, 100, 1 },
            { 10, 100, 1 }
        }
    };
    std_array<link_t, 6> parents{ { none, none, none, none, 0, 2 } };
    std_array<link_t, 6> siblings{ { none, none, none, none, none, none } };
    std_array<link_t, 6> rivals{ { 1, 0, 3, 2, none, none } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows: E=0, F=1 (absent), G=2, each spending X:0. Ring: X:0 (0, 1, 2).
struct ring_state
{
    ring_state() NOEXCEPT
    {
        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    std_array<link_t, 3> cells{ { 0, absent, 0 } };
    std_array<link_t, 3> children{ { none, none, none } };
    std_array<edges_t, 3> edges{ { { 0, 1 }, { 1, 1 }, { 2, 1 } } };
    std_array<totals_t, 3> totals
    {
        { { 10, 100, 1 }, { 10, 100, 1 }, { 10, 100, 1 } }
    };
    std_array<link_t, 3> parents{ { none, none, none } };
    std_array<link_t, 3> siblings{ { none, none, none } };
    std_array<link_t, 3> rivals{ { 1, 2, 0 } };

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

// Rows 0..39, each with one input, and a second where row % 7 is 3. Row spends
// row three before it unless row % 3 is 0. Rings: first inputs of rows (0, 1)
// and (9, 12), second inputs of rows (3, 10, 17) and (31, 38).
constexpr size_t wide_rows = 40;
constexpr size_t wide_inputs = 46;

struct wide_state
{
    wide_state() NOEXCEPT
    {
        children.fill(none);
        parents.fill(none);
        siblings.fill(none);
        rivals.fill(none);

        link_t input{};
        for (size_t row{}; row < wide_rows; ++row)
        {
            const link_t count = (row % 7) == 3 ? 2 : 1;
            totals[row] = { 10, 100, 1 };
            edges[row] = { input, count };
            firsts[row] = input;
            if (row >= 3 && is_nonzero(row % 3))
                spend(input, possible_narrow_cast<link_t>(row - 3));

            input += count;
        }

        ring(firsts[0], firsts[1], firsts[0]);
        ring(firsts[9], firsts[12], firsts[9]);
        ring(add1(firsts[3]), add1(firsts[10]), add1(firsts[17]));
        ring(add1(firsts[31]), add1(firsts[38]), add1(firsts[31]));

        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
    }

    void spend(link_t input, link_t parent) NOEXCEPT
    {
        parents[input] = parent;
        siblings[input] = children[parent];
        children[parent] = input;
    }

    // A ring of two inputs repeats the first as the third.
    void ring(link_t first, link_t second, link_t third) NOEXCEPT
    {
        rivals[first] = second;
        rivals[second] = third;
        rivals[third] = (third == first) ? second : first;
    }

    std_array<link_t, wide_rows> firsts{};
    std_array<link_t, wide_rows> cells{};
    std_array<link_t, wide_rows> children{};
    std_array<edges_t, wide_rows> edges{};
    std_array<totals_t, wide_rows> totals{};
    std_array<link_t, wide_inputs> parents{};
    std_array<link_t, wide_inputs> siblings{};
    std_array<link_t, wide_inputs> rivals{};

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }
};

template <typename Word, typename State>
static State label_of() NOEXCEPT
{
    State out{};
    accessor::initialize<Word>(out.columns());
    while (accessor::propagate<Word>(out.columns()));
    return out;
}

template <typename Word>
static state label() NOEXCEPT
{
    state out{};
    accessor::initialize<Word>(out.columns());
    while (accessor::propagate<Word>(out.columns()));
    return out;
}

static state label_without_a() NOEXCEPT
{
    state out{};
    out.cells[1] = accessor::to_column(absent);
    accessor::initialize(out.columns());
    while (accessor::propagate<link_t>(out.columns()));
    return out;
}

static chain_state label_chain() NOEXCEPT
{
    chain_state out{};
    accessor::initialize(out.columns());
    while (accessor::propagate<link_t>(out.columns()));
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

template <typename Word>
static bool propagates_as_integral() NOEXCEPT
{
    return label<Word>().cells == label<link_t>().cells;
}

static wide_state label_wide() NOEXCEPT
{
    wide_state out{};
    pool::label(out.columns());
    return out;
}

template <typename Word>
static bool propagates_wide_as_integral() NOEXCEPT
{
    const auto expected = label_of<link_t, wide_state>();
    return label_of<Word, wide_state>().cells == expected.cells;
}

// The wide table after one pass.
template <typename Word>
static wide_state propagate_wide() NOEXCEPT
{
    wide_state out{};
    accessor::initialize<Word>(out.columns());
    accessor::propagate<Word>(out.columns());
    return out;
}

template <typename Word>
static bool propagates_wide_once_as_integral() NOEXCEPT
{
    return propagate_wide<Word>().cells == propagate_wide<link_t>().cells;
}

template <typename Word>
static bool collects_as_integral() NOEXCEPT
{
    std_array<link_t, 79> cells{};
    size_t at{};
    std::generate(cells.begin(), cells.end(), [&]() NOEXCEPT
    {
        return possible_narrow_cast<link_t>(at++ % 3);
    });

    cells = stored(cells);
    component_t expected{};
    component_t out{};
    return accessor::collect<link_t>(expected, 1, cells) &&
        accessor::collect<Word>(out, 1, cells) &&
        out.rows == expected.rows && out.count == expected.count;
}

template <typename Word>
static bool finds_as_integral() NOEXCEPT
{
    std_array<link_t, 47> values{};
    size_t at{};
    std::generate(values.begin(), values.end(), [&]() NOEXCEPT
    {
        return possible_narrow_cast<link_t>(at++ % 4);
    });

    values = stored(values);
    std::vector<link_t> expected{};
    std::vector<link_t> out{};
    accessor::find<link_t>(expected, values, 3);
    accessor::find<Word>(out, values, 3);
    return out == expected;
}

template <typename Word>
static bool evaluates_as_integral() NOEXCEPT
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    std_array<mask_t, 23> masks{};
    size_t at{};
    std::generate(masks.begin(), masks.end(), [&]() NOEXCEPT
    {
        return accessor::outcome(out, at++ % 4);
    });

    return accessor::evaluate<Word>(masks, out, graph) ==
        accessor::evaluate<uint64_t>(masks, out, graph);
}

template <typename Word>
static bool bests_as_integral() NOEXCEPT
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    std_array<mask_t, 23> masks{};
    size_t at{};
    std::generate(masks.begin(), masks.end(), [&]() NOEXCEPT
    {
        return accessor::outcome(out, at++ % 4);
    });

    return accessor::best<Word>(masks, out, graph, 0, 20) ==
        accessor::best<uint64_t>(masks, out, graph, 0, 20) &&
        accessor::best<Word>(masks, out, graph, 2'000, 20) ==
        accessor::best<uint64_t>(masks, out, graph, 2'000, 20);
}

// The template rows of the resolved scenario within the budget size.
static std::vector<link_t> template_of(uint32_t size) NOEXCEPT
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    std::vector<link_t> out{};
    accessor::resolve<link_t>(marks, labeled.columns(), 1'024, 0, 20);
    accessor::pack(out, marks, labeled.columns(), { 0, size, 100 }, 25, 20);
    return out;
}

// The line scenario projected by the count of blocks, of two rows each.
static projection_t projection_of(size_t count) NOEXCEPT
{
    line_state line{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(3);
    for (size_t block{}; block < count; ++block)
        accessor::project(rows, out, line.columns(), { 0, 200, 100 }, 2, 20);

    return out;
}

// No arrivals, and 200 bytes per block at each rate up to 50, or up to 500.
static uint64_t no_arrivals(uint64_t) NOEXCEPT
{
    return 0;
}

static uint64_t low_arrivals(uint64_t rate) NOEXCEPT
{
    return rate > 50 ? 0 : 200;
}

static uint64_t high_arrivals(uint64_t rate) NOEXCEPT
{
    return rate > 500 ? 0 : 200;
}

// Table of scenario less P, with P as candidate
// (row 4, input 5 spending Q:0), bridged from Q.
struct child_candidate
{
    child_candidate() NOEXCEPT
    {
        children = stored(children);
        edges = stored(edges);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
        extra_cells = stored(extra_cells);
        extra_children = stored(extra_children);
        extra_edges = stored(extra_edges);
        extra_totals = stored(extra_totals);
        extra_parents = stored(extra_parents);
        extra_siblings = stored(extra_siblings);
        extra_rivals = stored(extra_rivals);
    }

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

// Full scenario with candidate C (row 5, input 6) spending X:0,
// bridged from input 0.
struct rival_candidate
{
    rival_candidate() NOEXCEPT
    {
        extra_cells = stored(extra_cells);
        extra_children = stored(extra_children);
        extra_edges = stored(extra_edges);
        extra_totals = stored(extra_totals);
        extra_parents = stored(extra_parents);
        extra_siblings = stored(extra_siblings);
        extra_rivals = stored(extra_rivals);
    }

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
    lone_candidate() NOEXCEPT
    {
        extra_cells = stored(extra_cells);
        extra_children = stored(extra_children);
        extra_edges = stored(extra_edges);
        extra_totals = stored(extra_totals);
        extra_parents = stored(extra_parents);
        extra_siblings = stored(extra_siblings);
        extra_rivals = stored(extra_rivals);
    }

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
    BC_REQUIRE_EQUAL(accessor::chargeable({ 0, 100, 24 }, 20), 120u);
    BC_REQUIRE_EQUAL(accessor::chargeable({ 0, 100, 21 }, 20), 105u);
}

BC_AUTO_TEST_CASE(pool_algorithm__fee_rate__size_sigops_or_empty__per_chargeable_kilobyte)
{
    BC_REQUIRE_EQUAL(accessor::fee_rate({ 1'000, 100, 1 }, 20), 10'000u);
    BC_REQUIRE_EQUAL(accessor::fee_rate({ 1'000, 100, 40 }, 20), 5'000u);
    BC_REQUIRE_EQUAL(accessor::fee_rate({ 1'000, 0, 0 }, 20), 1'000'000u);
}

// initialize/propagate (columns)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__initialize__columns__contested_labeled)
{
    state out{};
    accessor::initialize(out.columns());
    BC_REQUIRE_EQUAL(cell(out.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), 1u);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), 2u);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__connected__one_label)
{
    const auto labeled = label<link_t>();
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__absent_row__nothing_contested)
{
    const auto out = label_without_a();
    BC_REQUIRE_EQUAL(cell(out.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), absent);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__uncontested_ancestor__not_labeled)
{
    const auto out = label_chain();
    BC_REQUIRE_EQUAL(cell(out.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), 1u);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), 1u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__extended__same_as_integral)
{
    BC_REQUIRE(propagates_as_integral<xint128_t>());
    BC_REQUIRE(propagates_as_integral<xint256_t>());
    BC_REQUIRE(propagates_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__child_of_two_cliques__one_label)
{
    const auto out = label_of<link_t, merge_state>();
    BC_REQUIRE_EQUAL(cell(out.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__absent_ring_member__passed_over)
{
    const auto out = label_of<link_t, ring_state>();
    BC_REQUIRE_EQUAL(cell(out.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), absent);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__wide__expected)
{
    const auto out = label_of<link_t, wide_state>();
    BC_REQUIRE_EQUAL(cell(out.cells, 1), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 17), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 37), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 38), 0u);
    BC_REQUIRE_EQUAL(cell(out.cells, 9), 9u);
    BC_REQUIRE_EQUAL(cell(out.cells, 12), 9u);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 14), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 24), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 39), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__wide_extended__same_as_integral)
{
    BC_REQUIRE(propagates_wide_as_integral<xint128_t>());
    BC_REQUIRE(propagates_wide_as_integral<xint256_t>());
    BC_REQUIRE(propagates_wide_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__propagate__wide_extended_one_pass__same_as_integral)
{
    BC_REQUIRE(propagates_wide_once_as_integral<xint128_t>());
    BC_REQUIRE(propagates_wide_once_as_integral<xint256_t>());
    BC_REQUIRE(propagates_wide_once_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__label__wide__same_as_propagate)
{
    const auto expected = label_of<link_t, wide_state>();
    const auto out = label_wide();
    BC_REQUIRE(out.cells == expected.cells);
}

// reset/descend (columns)
// ----------------------------------------------------------------------------

constexpr auto rivaled = bit_or(absent, accessor::rivaled);
constexpr auto inherited = bit_or(absent, accessor::ancestor);
constexpr auto conflicted = bit_or(absent, accessor::conflicted);
constexpr auto insufficient = bit_or(absent, accessor::insufficient);

// Absence over the columns at the lane width of the word (or integral).
template <typename Word, typename State>
static void settle(State& out) NOEXCEPT
{
    accessor::reset<Word>(out.columns());
    while (accessor::descend<Word>(out.columns()));
}

// The wide table with row 0 confirmed and row 9 conflicted.
template <typename Word>
static wide_state settle_wide() NOEXCEPT
{
    wide_state out{};
    out.cells[0] = accessor::to_column(boundary);
    out.cells[9] = accessor::to_column(conflicted);
    settle<Word, wide_state>(out);
    return out;
}

template <typename Word>
static bool settles_wide_as_integral() NOEXCEPT
{
    return settle_wide<Word>().cells == settle_wide<link_t>().cells;
}

// The wide table with row 0 confirmed and row 9 conflicted, after one pass.
template <typename Word>
static wide_state descend_wide() NOEXCEPT
{
    wide_state out{};
    out.cells[0] = accessor::to_column(boundary);
    out.cells[9] = accessor::to_column(conflicted);
    accessor::reset<Word>(out.columns());
    accessor::descend<Word>(out.columns());
    return out;
}

template <typename Word>
static bool descends_wide_as_integral() NOEXCEPT
{
    return descend_wide<Word>().cells == descend_wide<link_t>().cells;
}

BC_AUTO_TEST_CASE(pool_algorithm__is_absent__cells__expected)
{
    BC_REQUIRE(!accessor::is_absent(0u));
    BC_REQUIRE(!accessor::is_absent(unlabeled));
    BC_REQUIRE(accessor::is_absent(absent));
    BC_REQUIRE(accessor::is_absent(conflicted));
    BC_REQUIRE(accessor::is_absent(bit_or(absent, accessor::derived)));
    BC_REQUIRE(!accessor::is_absent(boundary));
    BC_REQUIRE(!accessor::is_present(insufficient));
}

BC_AUTO_TEST_CASE(pool_algorithm__raised__cells__expected)
{
    BC_REQUIRE_EQUAL(accessor::raised(0u, accessor::conflicted), conflicted);
    BC_REQUIRE_EQUAL(accessor::raised(unlabeled, accessor::rivaled), rivaled);
    BC_REQUIRE_EQUAL(accessor::raised(rivaled, accessor::conflicted), bit_or(rivaled, accessor::conflicted));
    BC_REQUIRE_EQUAL(accessor::raised(conflicted, accessor::conflicted), conflicted);
}

BC_AUTO_TEST_CASE(pool_algorithm__lowered__cells__expected)
{
    BC_REQUIRE_EQUAL(accessor::lowered(0u, accessor::conflicted), 0u);
    BC_REQUIRE_EQUAL(accessor::lowered(boundary, accessor::conflicted), boundary);
    BC_REQUIRE_EQUAL(accessor::lowered(conflicted, accessor::conflicted), unlabeled);
    BC_REQUIRE_EQUAL(accessor::lowered(conflicted, accessor::rivaled), conflicted);
    BC_REQUIRE_EQUAL(accessor::lowered(bit_or(rivaled, accessor::conflicted), accessor::conflicted), rivaled);
    BC_REQUIRE_EQUAL(accessor::lowered(absent, accessor::conflicted), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__is_clear__present_or_derived__true)
{
    state out{};
    BC_REQUIRE(accessor::is_clear(out.cells));
    out.cells[1] = accessor::to_column(rivaled);
    out.cells[3] = accessor::to_column(inherited);
    BC_REQUIRE(accessor::is_clear(out.cells));
}

BC_AUTO_TEST_CASE(pool_algorithm__is_clear__stored_cause__false)
{
    state out{};
    out.cells[1] = accessor::to_column(bit_or(conflicted, accessor::ancestor));
    BC_REQUIRE(!accessor::is_clear(out.cells));
}

BC_AUTO_TEST_CASE(pool_algorithm__is_clear__confirmed__false)
{
    state out{};
    out.cells[0] = accessor::to_column(boundary);
    BC_REQUIRE(!accessor::is_clear(out.cells));
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__confirmed_rival__rival_and_its_child_absent)
{
    state out{};
    out.cells[0] = accessor::to_column(boundary);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), boundary);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), rivaled);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 2)));
    BC_REQUIRE_EQUAL(cell(out.cells, 3), inherited);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 4)));
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__rival_unconfirmed__all_present)
{
    state out{};
    out.cells[0] = accessor::to_column(boundary);
    settle<link_t, state>(out);
    out.cells[0] = accessor::to_column(unlabeled);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), unlabeled);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 2)));
    BC_REQUIRE_EQUAL(cell(out.cells, 3), unlabeled);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 4)));
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__conflicted__kept_and_its_child_absent)
{
    state out{};
    out.cells[1] = accessor::to_column(conflicted);
    settle<link_t, state>(out);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 0)));
    BC_REQUIRE_EQUAL(cell(out.cells, 1), conflicted);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 2)));
    BC_REQUIRE_EQUAL(cell(out.cells, 3), inherited);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 4)));
}

BC_AUTO_TEST_CASE(pool_algorithm__reset__conflict_cleared__present)
{
    state out{};
    out.cells[1] = accessor::to_column(conflicted);
    settle<link_t, state>(out);
    out.cells[1] = accessor::to_column(absent);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__insufficient_with_confirmed_rival__both_causes)
{
    state out{};
    out.cells[0] = accessor::to_column(boundary);
    out.cells[1] = accessor::to_column(insufficient);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), bit_or(insufficient, accessor::rivaled));
    BC_REQUIRE_EQUAL(cell(out.cells, 3), inherited);
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__purged__kept_descendant_inherits)
{
    state out{};
    const auto purged = bit_or(absent, accessor::purged);
    out.cells[1] = accessor::to_column(purged);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), purged);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), inherited);
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__absent_parent_of_conflicted__both_causes)
{
    state out{};
    out.cells[1] = accessor::to_column(conflicted);
    out.cells[3] = accessor::to_column(conflicted);
    settle<link_t, state>(out);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), conflicted);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), bit_or(conflicted, accessor::ancestor));
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__wide__expected)
{
    const auto out = settle_wide<link_t>();
    BC_REQUIRE_EQUAL(cell(out.cells, 0), boundary);
    BC_REQUIRE_EQUAL(cell(out.cells, 1), rivaled);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), inherited);
    BC_REQUIRE_EQUAL(cell(out.cells, 7), inherited);
    BC_REQUIRE_EQUAL(cell(out.cells, 9), conflicted);
    BC_REQUIRE(accessor::is_present(cell(out.cells, 2)));
    BC_REQUIRE(accessor::is_present(cell(out.cells, 12)));
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__wide_extended__same_as_integral)
{
    BC_REQUIRE(settles_wide_as_integral<xint128_t>());
    BC_REQUIRE(settles_wide_as_integral<xint256_t>());
    BC_REQUIRE(settles_wide_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__descend__wide_extended_one_pass__same_as_integral)
{
    BC_REQUIRE(descends_wide_as_integral<xint128_t>());
    BC_REQUIRE(descends_wide_as_integral<xint256_t>());
    BC_REQUIRE(descends_wide_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__settle__wide__same_as_descend)
{
    const auto expected = settle_wide<link_t>();
    wide_state out{};
    out.cells[0] = accessor::to_column(boundary);
    out.cells[9] = accessor::to_column(conflicted);
    const auto report = pool::settle(out.columns());
    BC_REQUIRE(out.cells == expected.cells);
    BC_REQUIRE_EQUAL(report.rows, wide_rows);
    BC_REQUIRE(is_nonzero(report.passes));
}

BC_AUTO_TEST_CASE(pool_algorithm__settle__clear__lanes_one_pass)
{
    state out{};
    out.cells[3] = accessor::to_column(inherited);
    const auto report = pool::settle(out.columns());
    BC_REQUIRE(!report.device);
    BC_REQUIRE_EQUAL(report.passes, 1u);
    BC_REQUIRE_EQUAL(cell(out.cells, 3), unlabeled);
}

// suffice (columns)
// ----------------------------------------------------------------------------

// Twenty rows validated under flags 7 at height 100 and median time past
// 1000, but for row 2 (flags 5), row 4 (median time past 2000), row 6 (height
// 500) and row 17 (height 101).
constexpr size_t context_rows = 20;
constexpr accessor::context_t validated{ 7, 100, 1000 };
constexpr accessor::context_t regained{ 7, 500, 2000 };
constexpr accessor::context_t widened{ 5, 100, 1000 };
constexpr uint32_t required = 4;
constexpr uint32_t missing = 8;

struct context_state
{
    context_state() NOEXCEPT
    {
        cells.fill(unlabeled);
        flags.fill(validated.flags);
        heights.fill(validated.height);
        medians.fill(validated.median);
        flags[2] = 5;
        medians[4] = 2000;
        heights[6] = 500;
        heights[17] = 101;

        cells = stored(cells);
        flags = stored(flags);
        heights = stored(heights);
        medians = stored(medians);
    }

    columns_t columns() NOEXCEPT
    {
        columns_t out{};
        out.cells = cells;
        out.flags = flags;
        out.heights = heights;
        out.medians = medians;
        return out;
    }

    std_array<link_t, context_rows> cells{};
    std_array<uint32_t, context_rows> flags{};
    std_array<uint32_t, context_rows> heights{};
    std_array<uint32_t, context_rows> medians{};
};

// The table swept under the context at the lane width of the word.
template <typename Word>
static context_state suffice_as(const accessor::context_t& context) NOEXCEPT
{
    context_state out{};
    accessor::suffice<Word>(out.columns(), context, required);
    return out;
}

// The table swept under the validated and then the regained context.
template <typename Word>
static context_state suffice_twice() NOEXCEPT
{
    auto out = suffice_as<Word>(validated);
    accessor::suffice<Word>(out.columns(), regained, required);
    return out;
}

template <typename Word>
static bool suffices_as_integral() NOEXCEPT
{
    const auto once = suffice_as<link_t>(validated);
    const auto twice = suffice_twice<link_t>();
    const auto wide = suffice_as<link_t>(widened);
    return suffice_as<Word>(validated).cells == once.cells &&
        suffice_twice<Word>().cells == twice.cells &&
        suffice_as<Word>(widened).cells == wide.cells;
}

// The count of rows left insufficient under the validated context.
template <typename Word>
static size_t insufficient_rows() NOEXCEPT
{
    context_state out{};
    return accessor::suffice<Word>(out.columns(), validated, required);
}

// The count of rows left insufficient when a required flag is not held.
template <typename Word>
static size_t unable_rows() NOEXCEPT
{
    context_state out{};
    return accessor::suffice<Word>(out.columns(), validated, missing);
}

BC_AUTO_TEST_CASE(pool_algorithm__is_sufficient__contexts__expected)
{
    BC_REQUIRE(accessor::is_sufficient({ 7, 100, 1000 }, { 7, 100, 1000 }, required));
    BC_REQUIRE(accessor::is_sufficient({ 7, 99, 999 }, { 7, 100, 1000 }, required));
    BC_REQUIRE(accessor::is_sufficient({ 7, 100, 1000 }, { 7, 100, 1000 }, 0));
    BC_REQUIRE(!accessor::is_sufficient({ 7, 100, 1000 }, { 7, 100, 1000 }, missing));
    BC_REQUIRE(!accessor::is_sufficient({ 5, 100, 1000 }, { 7, 100, 1000 }, required));
    BC_REQUIRE(accessor::is_sufficient({ 7, 100, 1000 }, { 5, 100, 1000 }, required));
    BC_REQUIRE(!accessor::is_sufficient({ 6, 100, 1000 }, { 5, 100, 1000 }, required));
    BC_REQUIRE(!accessor::is_sufficient({ 7, 101, 1000 }, { 7, 100, 1000 }, required));
    BC_REQUIRE(!accessor::is_sufficient({ 7, 100, 1001 }, { 7, 100, 1000 }, required));
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__required_not_held__all_insufficient)
{
    BC_REQUIRE_EQUAL(unable_rows<link_t>(), context_rows);
    BC_REQUIRE_EQUAL(unable_rows<xint128_t>(), context_rows);
    BC_REQUIRE_EQUAL(unable_rows<xint256_t>(), context_rows);
    BC_REQUIRE_EQUAL(unable_rows<xint512_t>(), context_rows);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__validated__four_insufficient)
{
    const auto out = suffice_as<link_t>(validated);
    BC_REQUIRE_EQUAL(insufficient_rows<link_t>(), 4u);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), insufficient);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), insufficient);
    BC_REQUIRE_EQUAL(cell(out.cells, 6), insufficient);
    BC_REQUIRE_EQUAL(cell(out.cells, 17), insufficient);
    BC_REQUIRE_EQUAL(cell(out.cells, 19), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__regained__only_flags_insufficient)
{
    const auto out = suffice_twice<link_t>();
    BC_REQUIRE_EQUAL(cell(out.cells, 2), insufficient);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 6), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 17), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__widened__flags_sufficient)
{
    context_state out{};
    BC_REQUIRE_EQUAL(accessor::suffice<link_t>(out.columns(), widened, required), 3u);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), unlabeled);
    BC_REQUIRE_EQUAL(cell(out.cells, 4), insufficient);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__confirmed__unchanged_and_uncounted)
{
    context_state out{};
    out.cells[2] = accessor::to_column(boundary);
    const auto count = accessor::suffice<link_t>(out.columns(), validated, required);
    BC_REQUIRE_EQUAL(count, 3u);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), boundary);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__conflicted__cause_kept)
{
    context_state out{};
    out.cells[0] = accessor::to_column(conflicted);
    out.cells[2] = accessor::to_column(conflicted);
    accessor::suffice<link_t>(out.columns(), validated, required);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), conflicted);
    BC_REQUIRE_EQUAL(cell(out.cells, 2), bit_or(conflicted, accessor::insufficient));
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__labeled__label_kept)
{
    context_state out{};
    out.cells[0] = accessor::to_column(8u);
    accessor::suffice<link_t>(out.columns(), validated, required);
    BC_REQUIRE_EQUAL(cell(out.cells, 0), 8u);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__extended__same_as_integral)
{
    BC_REQUIRE(suffices_as_integral<xint128_t>());
    BC_REQUIRE(suffices_as_integral<xint256_t>());
    BC_REQUIRE(suffices_as_integral<xint512_t>());
    BC_REQUIRE_EQUAL(insufficient_rows<xint128_t>(), 4u);
    BC_REQUIRE_EQUAL(insufficient_rows<xint256_t>(), 4u);
    BC_REQUIRE_EQUAL(insufficient_rows<xint512_t>(), 4u);
}

BC_AUTO_TEST_CASE(pool_algorithm__suffice__entry__same_as_integral)
{
    const auto expected = suffice_as<link_t>(validated);
    context_state out{};
    size_t count{};
    const auto report = pool::suffice(count, out.columns(), validated, required);
    BC_REQUIRE(out.cells == expected.cells);
    BC_REQUIRE_EQUAL(count, 4u);
    BC_REQUIRE_EQUAL(report.rows, context_rows);
    BC_REQUIRE_EQUAL(report.passes, 1u);
    BC_REQUIRE(!report.device);
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
    labeled.cells[2] = accessor::to_column(absent);
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
    const auto values = stored(std_array<link_t, 9>{ { 3, 0, 3, 1, 3, 2, 3, 3, 0 } });
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
    BC_REQUIRE_EQUAL(cell(scenario.extra_cells, 0), 4u);
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
    BC_REQUIRE_EQUAL(cell(scenario.extra_cells, 0), 5u);
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
    BC_REQUIRE_EQUAL(cell(scenario.extra_cells, 0), unlabeled);
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

BC_AUTO_TEST_CASE(pool_algorithm__evaluate__outcomes__highest_fee)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    BC_REQUIRE_EQUAL(accessor::evaluate<uint64_t>(outcomes_of(out), out, graph), 650u);
}

BC_AUTO_TEST_CASE(pool_algorithm__evaluate__extended__same_as_integral)
{
    BC_REQUIRE(evaluates_as_integral<xint128_t>());
    BC_REQUIRE(evaluates_as_integral<xint256_t>());
    BC_REQUIRE(evaluates_as_integral<xint512_t>());
}

BC_AUTO_TEST_CASE(pool_algorithm__covered__fee_at_rate__expected)
{
    BC_REQUIRE(accessor::covered(150, 100, 1'500));
    BC_REQUIRE(!accessor::covered(150, 100, 1'501));
    BC_REQUIRE(accessor::covered(0, 0, 1));
    BC_REQUIRE(!accessor::covered(0, 1, 1));
}

BC_AUTO_TEST_CASE(pool_algorithm__chargeable__component__sum_of_members)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    BC_REQUIRE_EQUAL(accessor::chargeable(out, graph, 20), 500u);
    BC_REQUIRE_EQUAL(accessor::chargeable(out, graph, 800), 1'000u);
}

// template
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__family__connected__best_outcome_all_bytes)
{
    auto labeled = label<link_t>();
    const graph_t graph{ labeled.columns() };
    const std_array<link_t, 1> rows{ { 4 } };
    uint64_t fee{}, size{};
    BC_REQUIRE(accessor::family<uint64_t>(fee, size, graph, rows, 500, 0, 20, 1'024));
    BC_REQUIRE_EQUAL(fee, 650u);
    BC_REQUIRE_EQUAL(size, 500u);
}

BC_AUTO_TEST_CASE(pool_algorithm__family__over_limit__false)
{
    auto labeled = label<link_t>();
    const graph_t graph{ labeled.columns() };
    const std_array<link_t, 1> rows{ { 4 } };
    uint64_t fee{}, size{};
    BC_REQUIRE(!accessor::family<uint64_t>(fee, size, graph, rows, 499, 0, 20, 1'024));
}

BC_AUTO_TEST_CASE(pool_algorithm__family__outcomes_over_bound__first_and_greedy)
{
    auto labeled = label<link_t>();
    const graph_t graph{ labeled.columns() };
    const std_array<link_t, 1> rows{ { 4 } };
    uint64_t fee{}, size{};
    BC_REQUIRE(accessor::family<uint64_t>(fee, size, graph, rows, max_uint64, 0, 20, 1));
    BC_REQUIRE_EQUAL(fee, 650u);
    BC_REQUIRE_EQUAL(size, 500u);
}

BC_AUTO_TEST_CASE(pool_algorithm__family__no_outcomes__false)
{
    auto labeled = label<link_t>();
    const graph_t graph{ labeled.columns() };
    const std_array<link_t, 1> rows{ { 4 } };
    uint64_t fee{}, size{};
    BC_REQUIRE(!accessor::family<uint64_t>(fee, size, graph, rows, max_uint64, 0, 20, 0));
}


BC_AUTO_TEST_CASE(pool_algorithm__best__scenario__highest_fee_index)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    const auto masks = outcomes_of(out);
    BC_REQUIRE_EQUAL(accessor::best<uint64_t>(masks, out, graph, 0, 20), 2u);
}

// At two satoshis per vbyte the third row's 150 no longer pays for its 100.
BC_AUTO_TEST_CASE(pool_algorithm__best__scenario_at_rate__highest_net_index)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    const auto masks = outcomes_of(out);
    BC_REQUIRE_EQUAL(accessor::best<uint64_t>(masks, out, graph, 2'000, 20), 0u);
}

// P (400) takes Q, so A loses and B is taken alone: rows 0, 2, 4 (650).
BC_AUTO_TEST_CASE(pool_algorithm__greedy__scenario__winners_by_value)
{
    auto labeled = label<link_t>();
    const auto out = component(labeled);
    const graph_t graph{ labeled.columns() };
    BC_REQUIRE_EQUAL(accessor::greedy(out, graph, 0, 20), 0b10101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__leaves__within_limit__each_outcome)
{
    auto labeled = label<link_t>();
    const auto scenario = component(labeled);
    const graph_t graph{ labeled.columns() };
    std::vector<mask_t> out{};
    accessor::leaves(out, scenario, graph, 1'024, 0, 20);
    BC_REQUIRE_EQUAL(out.size(), 4u);
    BC_REQUIRE_EQUAL(out[3], accessor::outcome(scenario, 3));
}

BC_AUTO_TEST_CASE(pool_algorithm__leaves__past_limit__first_and_greedy)
{
    auto labeled = label<link_t>();
    const auto scenario = component(labeled);
    const graph_t graph{ labeled.columns() };
    std::vector<mask_t> out{};
    accessor::leaves(out, scenario, graph, 2, 0, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], accessor::outcome(scenario, 0));
    BC_REQUIRE_EQUAL(out[1], accessor::outcome(scenario, 1));
    BC_REQUIRE_EQUAL(out[2], 0b10101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__leaves__zero_limit__empty)
{
    auto labeled = label<link_t>();
    const auto scenario = component(labeled);
    const graph_t graph{ labeled.columns() };
    std::vector<mask_t> out{ 42 };
    accessor::leaves(out, scenario, graph, 0, 0, 20);
    BC_REQUIRE(out.empty());
}

// Row 2 (500) is taken with its ancestor row 1, so rows 0 and 3 lose.
BC_AUTO_TEST_CASE(pool_algorithm__greedy__winner_with_ancestor__ancestor_rival_lost)
{
    auto labeled = label_of<link_t, descent_state>();
    const graph_t graph{ labeled.columns() };
    component_t out{};
    BC_REQUIRE(accessor::collect(out, graph, 0));
    BC_REQUIRE(accessor::relate(out, graph));
    BC_REQUIRE_EQUAL(accessor::greedy(out, graph, 0, 20), 0b0110u);
}

BC_AUTO_TEST_CASE(pool_algorithm__worth__fee_less_rate_on_size__expected)
{
    BC_REQUIRE(accessor::worth(500, 200, 650, 300, 2'000));
    BC_REQUIRE(!accessor::worth(650, 300, 500, 200, 2'000));
    BC_REQUIRE(accessor::worth(650, 300, 500, 200, 0));
    BC_REQUIRE(!accessor::worth(500, 200, 500, 200, 2'000));
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
    accessor::resolve<link_t>(marks, labeled.columns(), 1'024, 0, 20);
    BC_REQUIRE(marks[0] == mark::unmarked);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::unmarked);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

// Greedy leaf rows 0, 2, 4 (650) outbid first two outcomes (500).
BC_AUTO_TEST_CASE(pool_algorithm__resolve__outcome_limit__greedy_leaf_best)
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 2, 0, 20);
    BC_REQUIRE(marks[0] == mark::unmarked);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::unmarked);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

// Row 3 at 300, greedy rows 0, 2, 4 (650) outbid rows 1 and 3 (600).
BC_AUTO_TEST_CASE(pool_algorithm__resolve__outcome_limit_greedy_outbids__greedy_leaf)
{
    auto labeled = label<link_t>();
    labeled.totals[3] = accessor::to_column(totals_t{ 300, 100, 1 });
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 1, 0, 20);
    BC_REQUIRE(marks[0] == mark::unmarked);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::unmarked);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__no_outcomes__component_excluded)
{
    auto labeled = label<link_t>();
    std_array<mark, rows> marks{};
    accessor::resolve<link_t>(marks, labeled.columns(), 0, 0, 20);
    BC_REQUIRE(marks[0] == mark::excluded);
    BC_REQUIRE(marks[1] == mark::excluded);
    BC_REQUIRE(marks[2] == mark::excluded);
    BC_REQUIRE(marks[3] == mark::excluded);
    BC_REQUIRE(marks[4] == mark::excluded);
}

BC_AUTO_TEST_CASE(pool_algorithm__resolve__boundary_row__excluded)
{
    line_state line{};
    line.cells[0] = accessor::to_column(boundary);
    std_array<mark, 3> marks{};
    accessor::resolve<link_t>(marks, line.columns(), 1'024, 0, 20);
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

BC_AUTO_TEST_CASE(pool_algorithm__pack__chain_deeper_than_depth__whole_closure)
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

// Parent with both children (400) outrates Q (350),
// which outrates parent with one child (300).
BC_AUTO_TEST_CASE(pool_algorithm__pack__shared_parent__closure_outrates_row)
{
    fork_state fork{};
    std_array<mark, 4> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, fork.columns(), { 0, 300, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE_EQUAL(out[2], 2u);
    BC_REQUIRE(marks[3] == mark::unmarked);
}

// Cluster (250) over space (230) offers P, A, B (3,000), over Z (2,500).
BC_AUTO_TEST_CASE(pool_algorithm__pack__cluster_over_space__closure_that_fits)
{
    tail_state tail{};
    std_array<mark, 5> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, tail.columns(), { 0, 230, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE_EQUAL(out[2], 2u);
    BC_REQUIRE(marks[3] == mark::unmarked);
    BC_REQUIRE(marks[4] == mark::unmarked);
}

// The closure of A (700) leaves B (600), which outrates Q (500).
BC_AUTO_TEST_CASE(pool_algorithm__pack__closure_remainder__outrates_next_cluster)
{
    deferred_state deferred{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, deferred.columns(), { 0, 200, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(out.size(), 2u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 1u);
    BC_REQUIRE(marks[2] == mark::unmarked);
}

// The chunk of J with L (500) leaves K alone (700), which outrates Q (400).
BC_AUTO_TEST_CASE(pool_algorithm__pack__chunk_descendant__outrates_next_cluster)
{
    partial_state partial{};
    std_array<mark, 6> marks{ { mark::excluded, mark::unmarked, mark::unmarked, mark::unmarked, mark::unmarked, mark::unmarked } };
    std::vector<link_t> out{};
    accessor::pack(out, marks, partial.columns(), { 0, 300, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(out.size(), 3u);
    BC_REQUIRE_EQUAL(out[0], 1u);
    BC_REQUIRE_EQUAL(out[1], 2u);
    BC_REQUIRE_EQUAL(out[2], 3u);
    BC_REQUIRE(marks[4] == mark::unmarked);
    BC_REQUIRE(marks[5] == mark::unmarked);
}

// Chunk of C with P (750, each row charged alone) is outrated by E (1200) and
// by P alone (1000), which fill block.
BC_AUTO_TEST_CASE(pool_algorithm__pack__sigop_charged_child__chunk_rated_by_rows)
{
    charged_state charged{};
    std_array<mark, 5> marks{ { mark::excluded, mark::unmarked, mark::unmarked, mark::unmarked, mark::unmarked } };
    std::vector<link_t> out{};
    accessor::pack(out, marks, charged.columns(), { 0, 2'000, 1'000 }, 25, 20);
    BC_REQUIRE_EQUAL(out.size(), 2u);
    BC_REQUIRE_EQUAL(out[0], 1u);
    BC_REQUIRE_EQUAL(out[1], 4u);
    BC_REQUIRE(marks[2] == mark::unmarked);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__sigop_charged_child__takes_sized_by_rows)
{
    charged_state charged{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks = { mark::excluded, mark::unmarked, mark::unmarked, mark::unmarked, mark::unmarked };
    accessor::project(rows, out, charged.columns(), { 0, 3'000, 1'000 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 3u);
    BC_REQUIRE_EQUAL(out.blocks.size(), 1u);
    BC_REQUIRE_EQUAL(out.blocks[0].size(), 3u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].rate, 1'200u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].size, 1'000u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].rate, 1'000u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].size, 1'000u);
    BC_REQUIRE_EQUAL(out.blocks[0][2].rate, 500u);
    BC_REQUIRE_EQUAL(out.blocks[0][2].size, 1'000u);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__shared_parent__closure_then_row)
{
    fork_state fork{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(4);
    accessor::project(rows, out, fork.columns(), { 0, 1'000, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 4u);
    BC_REQUIRE_EQUAL(out.blocks.size(), 1u);
    BC_REQUIRE_EQUAL(out.blocks[0].size(), 2u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].rate, 400u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].size, 300u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].rate, 350u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].size, 100u);
}

BC_AUTO_TEST_CASE(pool_algorithm__pack__empty_budget__empty)
{
    line_state line{};
    std_array<mark, 3> marks{};
    std::vector<link_t> out{};
    accessor::pack(out, marks, line.columns(), {}, 2, 20);
    BC_REQUIRE(out.empty());
}

// project
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__project__first_block__full_not_exhausted)
{
    const auto out = projection_of(1);
    BC_REQUIRE_EQUAL(out.blocks.size(), 1u);
    BC_REQUIRE_EQUAL(out.blocks[0].size(), 2u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].rate, 100u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].size, 100u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].rate, 100u);
    BC_REQUIRE_EQUAL(out.blocks[0][1].size, 100u);
    BC_REQUIRE(!out.exhausted);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__second_block__remainder_exhausted)
{
    const auto out = projection_of(2);
    BC_REQUIRE_EQUAL(out.blocks.size(), 2u);
    BC_REQUIRE_EQUAL(out.blocks[1].size(), 1u);
    BC_REQUIRE_EQUAL(out.blocks[1][0].rate, 4'000u);
    BC_REQUIRE_EQUAL(out.blocks[1][0].size, 100u);
    BC_REQUIRE(out.exhausted);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__exhausted__no_block)
{
    const auto out = projection_of(3);
    BC_REQUIRE_EQUAL(out.blocks.size(), 2u);
    BC_REQUIRE(out.exhausted);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__second_block__rows_of_block)
{
    line_state line{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(3);
    accessor::project(rows, out, line.columns(), { 0, 200, 100 }, 2, 20);
    accessor::project(rows, out, line.columns(), { 0, 200, 100 }, 2, 20);
    BC_REQUIRE_EQUAL(rows.size(), 1u);
    BC_REQUIRE_EQUAL(rows[0], 2u);
}

// S, then the chain L, M, N a row a block, its cluster kept and formed again.
BC_AUTO_TEST_CASE(pool_algorithm__project__blocks_of_one_row__cluster_row_by_row)
{
    apart_state apart{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(4);
    accessor::project(rows, out, apart.columns(), { 0, 100, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 1u);
    BC_REQUIRE_EQUAL(rows[0], 0u);
    accessor::project(rows, out, apart.columns(), { 0, 100, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 1u);
    BC_REQUIRE_EQUAL(rows[0], 1u);
    accessor::project(rows, out, apart.columns(), { 0, 100, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 1u);
    BC_REQUIRE_EQUAL(rows[0], 2u);
    accessor::project(rows, out, apart.columns(), { 0, 100, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 1u);
    BC_REQUIRE_EQUAL(rows[0], 3u);
    BC_REQUIRE_EQUAL(out.blocks.size(), 4u);
    BC_REQUIRE_EQUAL(out.blocks[0][0].rate, 5'000u);
    BC_REQUIRE_EQUAL(out.blocks[1][0].rate, 100u);
    BC_REQUIRE_EQUAL(out.blocks[2][0].rate, 100u);
    BC_REQUIRE_EQUAL(out.blocks[3][0].rate, 4'000u);
    BC_REQUIRE(out.exhausted);
}

// The chain does not fit the first budget, and is one take of the second.
BC_AUTO_TEST_CASE(pool_algorithm__project__budget_raised_between_blocks__whole_cluster_one_take)
{
    apart_state apart{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(4);
    accessor::project(rows, out, apart.columns(), { 0, 100, 100 }, 25, 20);
    accessor::project(rows, out, apart.columns(), { 0, 1'000, 100 }, 25, 20);
    BC_REQUIRE_EQUAL(rows.size(), 3u);
    BC_REQUIRE_EQUAL(out.blocks.size(), 2u);
    BC_REQUIRE_EQUAL(out.blocks[1].size(), 1u);
    BC_REQUIRE_EQUAL(out.blocks[1][0].rate, 1'400u);
    BC_REQUIRE_EQUAL(out.blocks[1][0].size, 300u);
}

BC_AUTO_TEST_CASE(pool_algorithm__project__unfit_row__exhausted)
{
    line_state line{};
    projection_t out{};
    std::vector<link_t> rows{};
    out.marks.resize(3);
    accessor::project(rows, out, line.columns(), { 0, 50, 100 }, 2, 20);
    BC_REQUIRE(rows.empty());
    BC_REQUIRE(out.blocks.empty());
    BC_REQUIRE(out.exhausted);
}

// estimate
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_algorithm__above__rates__sizes_at_or_above)
{
    const std_array<take_t, 3> takes{ { { 300, 10 }, { 200, 20 }, { 100, 30 } } };
    BC_REQUIRE_EQUAL(accessor::above(takes, 0), 60u);
    BC_REQUIRE_EQUAL(accessor::above(takes, 200), 30u);
    BC_REQUIRE_EQUAL(accessor::above(takes, 301), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__empty_pool__zero)
{
    projection_t projection{};
    projection.exhausted = true;
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 200, 1, no_arrivals), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__full_block__above_lowest_take)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 200, 1, no_arrivals), 101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__full_last_block__above_lowest_take)
{
    const auto projection = projection_of(1);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 200, 1, no_arrivals), 101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__takes_out_of_rate_order__above_lowest_take)
{
    projection_t projection{};
    projection.blocks = { { { 100, 50 }, { 300, 100 }, { 200, 50 } }, { { 50, 10 } } };
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 200, 1, no_arrivals), 101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__open_block__zero)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 2, 200, 1, no_arrivals), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__beyond_blocks__zero)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 3, 200, 1, no_arrivals), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__low_arrivals_fill_open_block__above_arrivals)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 2, 200, 1, low_arrivals), 51u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__high_arrivals_fill_open_block__above_first_block)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 2, 200, 1, high_arrivals), 101u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__no_space__maximum)
{
    projection_t projection{};
    projection.exhausted = true;
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 0, 1, no_arrivals), max_uint64);
}

// The tail take of 10 bytes leaves room for one byte at its rate, not for 100.
BC_AUTO_TEST_CASE(pool_algorithm__estimate__room_within_low_tail_take__above_tail_take)
{
    projection_t projection{};
    projection.blocks = { { { 500, 990 }, { 10, 10 } }, { { 5, 1'000 } } };
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 1'000, 1, no_arrivals), 11u);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 1'000, 10, no_arrivals), 11u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__room_over_low_tail_take__above_take_before_it)
{
    projection_t projection{};
    projection.blocks = { { { 500, 990 }, { 10, 10 } }, { { 5, 1'000 } } };
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 1'000, 11, no_arrivals), 501u);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 1'000, 100, no_arrivals), 501u);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__room_over_block__maximum)
{
    projection_t projection{};
    projection.blocks = { { { 500, 990 }, { 10, 10 } }, { { 5, 1'000 } } };
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 1, 1'000, 1'001, no_arrivals), max_uint64);
}

BC_AUTO_TEST_CASE(pool_algorithm__estimate__no_blocks__maximum)
{
    const auto projection = projection_of(2);
    BC_REQUIRE_EQUAL(accessor::estimate(projection, 0, 200, 1, no_arrivals), max_uint64);
}

// transitions
// ----------------------------------------------------------------------------

// Present rows labeled by the columns.
template <typename State>
static void relabel(State& out) NOEXCEPT
{
    accessor::initialize<link_t>(out.columns());
    while (accessor::propagate<link_t>(out.columns()));
}

// The rows made boundary and the conflicts conflicted, by the columns.
template <typename State>
static void confirm_by_columns(State& out,
    const std::span<const link_t>& block,
    const std::span<const link_t>& conflicts) NOEXCEPT
{
    for (const auto row: block)
        out.cells[row] = accessor::to_column(boundary);

    for (const auto row: conflicts)
    {
        const auto own = cell(out.cells, row);
        if (own != boundary)
            out.cells[row] = accessor::to_column(accessor::raised(own,
                accessor::conflicted));
    }

    settle<link_t, State>(out);
    relabel(out);
}

// Boundary rows made unconfirmed and conflicts not conflicted, by columns.
template <typename State>
static void unconfirm_by_columns(State& out,
    const std::span<const link_t>& block,
    const std::span<const link_t>& conflicts) NOEXCEPT
{
    for (const auto row: block)
        if (cell(out.cells, row) == boundary)
            out.cells[row] = accessor::to_column(unlabeled);

    for (const auto row: conflicts)
        out.cells[row] = accessor::to_column(accessor::lowered(
            cell(out.cells, row), accessor::conflicted));

    settle<link_t, State>(out);
    relabel(out);
}

// Wide table after rows confirm with conflicts, and then after they unconfirm,
// by walk and by columns.
static bool walks_wide_as_columns(const std::vector<link_t>& block,
    const std::vector<link_t>& conflicts) NOEXCEPT
{
    auto walked = label_of<link_t, wide_state>();
    auto scanned = label_of<link_t, wide_state>();
    const auto complete = accessor::confirm(walked.columns(), block, conflicts);
    confirm_by_columns(scanned, block, conflicts);
    const auto same = walked.cells == scanned.cells;
    const auto restored = accessor::unconfirm(walked.columns(), block,
        conflicts);

    unconfirm_by_columns(scanned, block, conflicts);
    return complete && same && restored && walked.cells == scanned.cells;
}

// Random table. Row has one to three inputs, each spending earlier row or none
// with equal odds. One in four of first inputs that spend no row starts ring of
// two or three of them.
struct random_state
{
    random_state(size_t count, uint32_t seed) NOEXCEPT
    {
        std::mt19937 generator(seed);
        std::vector<link_t> free{};
        cells.assign(count, unlabeled);
        children.assign(count, none);
        totals.assign(count, totals_t{ 10, 100, 1 });
        edges.resize(count);

        for (size_t row{}; row < count; ++row)
        {
            const auto inputs = possible_narrow_cast<link_t>(
                add1(generator() % 3u));

            const auto first = possible_narrow_cast<link_t>(parents.size());
            edges[row] = { first, inputs };
            for (link_t at{}; at < inputs; ++at)
            {
                const auto input = first + at;
                const auto spend = is_nonzero(row) &&
                    is_zero(generator() % 2u);

                const auto parent = spend ? possible_narrow_cast<link_t>(
                    generator() % row) : none;

                parents.push_back(parent);
                siblings.push_back(spend ? children[parent] : none);
                rivals.push_back(none);
                if (spend)
                    children[parent] = input;
            }

            if (parents[first] == none)
                free.push_back(first);
        }

        for (size_t at{}; at + 3u <= free.size();)
        {
            const auto size = 2u + (generator() % 2u);
            if (is_nonzero(generator() % 4u))
            {
                ++at;
                continue;
            }

            for (size_t member{}; member < size; ++member)
                rivals[free[at + member]] = free[at + (add1(member) % size)];

            at += size;
        }

        cells = stored(cells);
        children = stored(children);
        edges = stored(edges);
        totals = stored(totals);
        parents = stored(parents);
        siblings = stored(siblings);
        rivals = stored(rivals);
        relabel(*this);
    }

    columns_t columns() NOEXCEPT
    {
        return { cells, children, edges, totals, parents, siblings, rivals };
    }

    std::vector<link_t> cells{};
    std::vector<link_t> children{};
    std::vector<edges_t> edges{};
    std::vector<totals_t> totals{};
    std::vector<link_t> parents{};
    std::vector<link_t> siblings{};
    std::vector<link_t> rivals{};
};

// Random rows of the table.
static std::vector<link_t> random_rows(std::mt19937& generator, size_t count,
    size_t rows) NOEXCEPT
{
    std::vector<link_t> out(count);
    for (auto& row: out)
        row = possible_narrow_cast<link_t>(generator() % rows);

    return out;
}

// Random blocks, confirmed and half unconfirmed, by walk and by columns.
// Walk that meets oversized component is relabeled by columns,
// and at least one walk must not.
static bool walks_random_as_columns(uint32_t seed) NOEXCEPT
{
    constexpr size_t rows = 300;
    constexpr size_t steps = 40;
    std::mt19937 generator(add1(seed));
    random_state walked{ rows, seed };
    auto scanned = walked;
    size_t complete{};

    for (size_t step{}; step < steps; ++step)
    {
        const auto block = random_rows(generator, 8, rows);
        const auto conflicts = random_rows(generator, 4, rows);
        if (accessor::confirm(walked.columns(), block, conflicts))
            ++complete;
        else
            relabel(walked);

        confirm_by_columns(scanned, block, conflicts);
        if (walked.cells != scanned.cells)
            return false;

        if (is_zero(generator() % 2u))
            continue;

        if (accessor::unconfirm(walked.columns(), block, conflicts))
            ++complete;
        else
            relabel(walked);

        unconfirm_by_columns(scanned, block, conflicts);
        if (walked.cells != scanned.cells)
            return false;
    }

    return is_nonzero(complete);
}

BC_AUTO_TEST_CASE(pool_algorithm__admit__contested_row__labeled_into_component)
{
    auto labeled = label<link_t>();
    labeled.cells = stored(std_array<link_t, rows>{ { 0, 0, 0, 0, absent } });
    const auto complete = accessor::admit(labeled.columns(), 4);
    BC_REQUIRE(complete);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__admit__absent_parent__absent_for_ancestor)
{
    auto labeled = label<link_t>();
    labeled.cells = stored(std_array<link_t, rows>{ { conflicted, unlabeled, unlabeled, unlabeled, absent } });
    accessor::admit(labeled.columns(), 4);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), inherited);
}

BC_AUTO_TEST_CASE(pool_algorithm__admit__conflict__conflicted_and_child_absent)
{
    auto labeled = label<link_t>();
    labeled.cells = stored(std_array<link_t, rows>{ { unlabeled, absent, unlabeled, unlabeled, unlabeled } });
    accessor::admit(labeled.columns(), 1, true);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), conflicted);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), inherited);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__q__rival_and_its_child_absent_rest_unlabeled)
{
    auto labeled = label<link_t>();
    const auto complete = accessor::confirm(labeled.columns(), std::vector<link_t>{ 0 }, {});
    BC_REQUIRE(complete);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), boundary);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), rivaled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), unlabeled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), inherited);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__b__q_loses_rival_unlabeled)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), std::vector<link_t>{ 2 }, {});
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), rivaled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), boundary);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), inherited);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__conflict__conflicted_and_child_absent)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), {}, std::vector<link_t>{ 1 });
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), conflicted);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), unlabeled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), inherited);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__unconfirm__q__component_restored)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), std::vector<link_t>{ 0 }, {});
    accessor::unconfirm(labeled.columns(), std::vector<link_t>{ 0 }, {});
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__unconfirm__other_boundary__rival_stays_absent)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), std::vector<link_t>{ 2 }, {});
    accessor::confirm(labeled.columns(), std::vector<link_t>{ 0 }, {});
    accessor::unconfirm(labeled.columns(), std::vector<link_t>{ 0 }, {});
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), unlabeled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), rivaled);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), boundary);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), inherited);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), unlabeled);
}

BC_AUTO_TEST_CASE(pool_algorithm__unconfirm__conflict__component_restored)
{
    auto labeled = label<link_t>();
    accessor::confirm(labeled.columns(), {}, std::vector<link_t>{ 1 });
    accessor::unconfirm(labeled.columns(), {}, std::vector<link_t>{ 1 });
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), 0u);
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__wide__same_as_columns)
{
    BC_REQUIRE(walks_wide_as_columns({ 0 }, {}));
    BC_REQUIRE(walks_wide_as_columns({ 1, 12 }, {}));
    BC_REQUIRE(walks_wide_as_columns({ 3 }, { 9 }));
    BC_REQUIRE(walks_wide_as_columns({}, { 0, 31 }));
    BC_REQUIRE(walks_wide_as_columns({ 10, 17, 38 }, { 4 }));
}

BC_AUTO_TEST_CASE(pool_algorithm__confirm__random__same_as_columns)
{
    BC_REQUIRE(walks_random_as_columns(1));
    BC_REQUIRE(walks_random_as_columns(2));
    BC_REQUIRE(walks_random_as_columns(3));
    BC_REQUIRE(walks_random_as_columns(4));
}

BC_AUTO_TEST_CASE(pool_algorithm__relabel__component__lowest_row)
{
    auto labeled = label<link_t>();
    labeled.cells = stored(std_array<link_t, rows>{ { 0, 1, 2, 3, 4 } });
    component_t out{};
    out.count = 3;
    out.rows = { { 2, 3, 4 } };
    accessor::relabel(labeled.columns(), out);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 0), 0u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 1), 1u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 2), 2u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 3), 2u);
    BC_REQUIRE_EQUAL(cell(labeled.cells, 4), 2u);
}

BC_AUTO_TEST_SUITE_END()
