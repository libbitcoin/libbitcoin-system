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

#if defined(HAVE_PERFORMANCE_TESTS)

#include <chrono>
#include <random>

BC_AUTO_TEST_SUITE(pool_performance_tests)

// Each scan is timed alone over the columns of a random pool, on the integral
// path, on each lane width built, and through its entry (the gpu if there is
// one, otherwise the widest lanes).

using link_t = pool::algorithm::link_t;
using edges_t = pool::algorithm::edges_t;
using totals_t = pool::algorithm::totals_t;
using columns_t = pool::algorithm::columns_t;
using context_t = pool::algorithm::context_t;
using links_t = std_vector<link_t>;
using clock_t = std::chrono::steady_clock;
using milliseconds = std::chrono::duration<double, std::milli>;
constexpr auto none = pool::algorithm::none;
constexpr auto boundary = pool::algorithm::boundary;
constexpr auto unlabeled = pool::algorithm::unlabeled;
constexpr auto conflicted = bit_or(pool::algorithm::absent,
    pool::algorithm::conflicted);

constexpr size_t repeats = 5;
constexpr size_t table_rows = 1'000'000;
constexpr uint32_t table_seed = 11;

// Rows are validated at a height and median time past below this, under these
// flags.
constexpr uint32_t table_heights = 1'000;
constexpr uint32_t table_flags = 1;

// A pool of confirmed rows with a tail of live rows, a block of the live rows
// with its conflicts, and the rows sampled for the walks of one row.
constexpr size_t pool_live = 100'000;
constexpr size_t pool_block = 3'000;
constexpr size_t pool_conflicts = 10;
constexpr size_t pool_samples = 10'000;

// A template: the block less its coinbase, the chunk depth, the outcomes of a
// component, the bytes charged a sigop, and the blocks of a fee projection.
constexpr totals_t template_budget{ 0, 999'000, 80'000 };
constexpr size_t template_depth = 25;
constexpr size_t template_outcomes = 1'024;
constexpr uint32_t template_sigop_bytes = 20;
constexpr size_t template_blocks = 6;

// The blocks of each fee estimate from the projection and the bytes it finds
// room for, and the chargeable bytes arriving a block at a rate of 1,000 (less
// in proportion above it).
constexpr std_array<size_t, 4> template_targets{ 1, 2, 3, 6 };
constexpr uint64_t template_room = 1'000;
constexpr uint64_t template_arrivals = 500'000;

// A pool of chains: its rows alone, its chains and their greatest length, and
// its parents with children.
constexpr size_t chains_alone = 3'000;
constexpr size_t chains_count = 2'300;
constexpr size_t chains_length = 25;
constexpr size_t chains_parents = 500;

// One ring is started at one in this many of the eligible inputs.
constexpr uint32_t uncontested = max_uint32;
constexpr uint32_t sparse = 2'000;
constexpr uint32_t dense = 4;

// helpers
// ----------------------------------------------------------------------------

// A random pool. A row has one to three inputs, each spending an earlier row
// or none with equal odds, and one row in fifty is conflicted (if conflicts).
// First inputs that spend no row are joined in rings of two to four, one ring
// in every odds. Each row is validated at a random height.
struct table
{
    table() NOEXCEPT = default;

    table(size_t count, uint32_t seed, uint32_t odds,
        bool conflicts=true) NOEXCEPT
    {
        std::mt19937 generator(seed);
        initial.assign(count, link_t{});
        children.assign(count, none);
        totals.assign(count, totals_t{ 10, 100, 1 });
        edges.resize(count);
        links_t free{};

        for (size_t row{}; row < count; ++row)
        {
            const auto draw = add1(generator() % 3u);
            const auto inputs = possible_narrow_cast<link_t>(draw);
            const auto first = possible_narrow_cast<link_t>(rows.size());
            edges[row] = { first, inputs };
            if (is_zero(generator() % 50u) && conflicts)
                initial[row] = conflicted;

            for (link_t input{}; input < inputs; ++input)
            {
                const auto spend = is_nonzero(row) &&
                    is_zero(generator() % 2u);
                const auto parent = spend ? (generator() % row) : none;
                rows.push_back(possible_narrow_cast<link_t>(parent));
                siblings.push_back(none);
                rivals.push_back(none);
            }

            if (rows[first] == none)
                free.push_back(first);
        }

        for (size_t at{}; at + 4u <= free.size();)
        {
            const auto size = 2u + (generator() % 3u);
            if (is_nonzero(generator() % odds))
            {
                ++at;
                continue;
            }

            for (size_t member{}; member < size; ++member)
                rivals[free[at + member]] = free[at + (add1(member) % size)];

            at += size;
        }

        std::mt19937 heights_generator(add1(seed));
        flags.assign(count, table_flags);
        heights.resize(count);
        for (auto& height: heights)
            height = heights_generator() % table_heights;

        medians = heights;
        cells = initial;
    }

    void reset() NOEXCEPT
    {
        cells = initial;
    }

    columns_t columns() NOEXCEPT
    {
        return
        {
            cells, children, edges, totals, rows, siblings, rivals, flags,
            heights, medians
        };
    }

    size_t contested() const NOEXCEPT
    {
        const auto sole = std::count(rivals.cbegin(), rivals.cend(), none);
        return rivals.size() - to_unsigned(sole);
    }

    // Bytes sent to the gpu by a scan of the inputs.
    size_t staged() const NOEXCEPT
    {
        const auto row = sizeof(link_t) + sizeof(edges_t);
        return cells.size() * row + rows.size() * two * sizeof(link_t);
    }

    // Bytes sent to the gpu by a scan of the contexts.
    size_t contexts() const NOEXCEPT
    {
        return cells.size() * (sizeof(link_t) + 3u * sizeof(uint32_t));
    }

    links_t initial{};
    links_t cells{};
    links_t children{};
    std_vector<edges_t> edges{};
    std_vector<totals_t> totals{};
    links_t rows{};
    links_t siblings{};
    links_t rivals{};
    links_t flags{};
    links_t heights{};
    links_t medians{};
};

// A pool as it stands between prunes: confirmed rows, then a tail of live
// rows. A row has one to three inputs. An input of a live row spends a live
// row at one in eight, and otherwise (as any input of a confirmed row) an
// earlier confirmed row or none with about equal odds. One in two hundred of
// the live first inputs that spend no row starts a ring of two or three. A
// row pays 100 to 10,099 for 100 to 499 virtual bytes.
struct pooled
  : table
{
    pooled(size_t count, size_t live, uint32_t seed) NOEXCEPT
    {
        std::mt19937 generator(seed);
        const auto floor = count - live;
        initial.assign(count, boundary);
        children.assign(count, none);
        totals.assign(count, totals_t{ 10, 100, 1 });
        edges.resize(count);
        links_t free{};

        for (size_t row{}; row < count; ++row)
        {
            const auto draw = add1(generator() % 3u);
            const auto inputs = possible_narrow_cast<link_t>(draw);
            const auto first = possible_narrow_cast<link_t>(rows.size());
            const auto confirmed = std::min(row, floor);
            edges[row] = { first, inputs };
            if (row >= floor)
                initial[row] = unlabeled;

            for (link_t input{}; input < inputs; ++input)
            {
                size_t parent{ none };
                const auto odds = generator() % 16u;
                if (row > floor && odds < 2u)
                    parent = floor + (generator() % (row - floor));
                else if (is_nonzero(confirmed) && odds < 9u)
                    parent = generator() % confirmed;

                const auto up = possible_narrow_cast<link_t>(parent);
                rows.push_back(up);
                siblings.push_back(up == none ? none : children[up]);
                rivals.push_back(none);
                if (up != none)
                    children[up] = first + input;
            }

            if (row >= floor && rows[first] == none)
                free.push_back(first);
        }

        for (size_t at{}; at + 3u <= free.size();)
        {
            const auto size = 2u + (generator() % 2u);
            if (is_nonzero(generator() % 200u))
            {
                ++at;
                continue;
            }

            for (size_t member{}; member < size; ++member)
                rivals[free[at + member]] = free[at + (add1(member) % size)];

            at += size;
        }

        std::mt19937 payer(seed + 2u);
        for (auto& total: totals)
        {
            total.fee = 100u + (payer() % 10'000u);
            total.size = 100u + (payer() % 400u);
        }

        cells = initial;
        pool::algorithm::initialize<link_t>(columns());
        while (pool::algorithm::propagate<link_t>(columns()));
        initial = cells;
    }
};

// A pool of chains, as mainnet on 21 November 2026 (ancestor counts of 1 to
// 25 about equally common, one tx in twenty alone): confirmed rows, then rows
// alone, chains of 20 to 25 rows laid out by depth, and parents of one to
// four children. A row has one input, or two at one in three, the first
// spending its parent if it has one and any other a confirmed row or none
// with equal odds. One in two hundred of the live first inputs that spend no
// row starts a ring of two or three. Seven rows in ten pay 1,000 to 1,299 a
// virtual kilobyte, two up to 3,299 and one up to 32,999, for 110 to 409
// virtual bytes. A chain pays one rate, varied by row in one chain of three,
// and a parent of children pays 500 to 1,499.
struct chained
  : table
{
    chained(size_t confirmed, uint32_t seed) NOEXCEPT
    {
        std::mt19937 generator(seed);
        links_t free{};

        const auto draw = [&](uint32_t below) NOEXCEPT
        {
            return possible_narrow_cast<uint32_t>(generator() % below);
        };

        const auto spend = [&](link_t up) NOEXCEPT
        {
            const auto input = possible_narrow_cast<link_t>(rows.size());
            rows.push_back(up);
            rivals.push_back(none);
            siblings.push_back(up == none ? none : children[up]);
            if (up != none)
                children[up] = input;
        };

        const auto settled = [&]() NOEXCEPT
        {
            const auto spent = is_nonzero(confirmed) && is_zero(draw(2));
            return spent ? possible_narrow_cast<link_t>(
                generator() % confirmed) : none;
        };

        const auto rate = [&]() NOEXCEPT -> uint64_t
        {
            const auto odds = draw(10);
            if (odds < 7u)
                return 1'000u + draw(300);

            return odds < 9u ? 1'300u + draw(2'000) : 3'000u + draw(30'000);
        };

        // A live row spending the parent, a confirmed row or none.
        const auto add = [&](link_t parent, uint64_t pays,
            uint32_t size) NOEXCEPT
        {
            const auto row = possible_narrow_cast<link_t>(edges.size());
            const auto first = possible_narrow_cast<link_t>(rows.size());
            const auto inputs = is_zero(draw(3)) ? 2u : 1u;
            edges.push_back({ first, inputs });
            children.push_back(none);
            initial.push_back(unlabeled);
            totals.push_back({ pays * size / 1'000u, size, add1(draw(4)) });
            spend(parent == none ? settled() : parent);
            if (inputs == 2u)
                spend(settled());

            if (rows[first] == none)
                free.push_back(first);

            return row;
        };

        for (size_t row{}; row < confirmed; ++row)
        {
            const auto first = possible_narrow_cast<link_t>(rows.size());
            edges.push_back({ first, 1u });
            children.push_back(none);
            initial.push_back(boundary);
            totals.push_back({ 10, 100, 1 });
            spend(none);
        }

        for (size_t row{}; row < chains_alone; ++row)
        {
            const auto pays = rate();
            add(none, pays, 110u + draw(300));
        }

        struct chain
        {
            link_t last;
            size_t length;
            uint64_t pays;
            uint32_t size;
            bool varied;
        };

        std_vector<chain> chains(chains_count);
        for (auto& next: chains)
        {
            next.last = none;
            next.length = chains_length - draw(6);
            next.pays = rate();
            next.size = 110u + draw(200);
            next.varied = is_zero(draw(3));
        }

        for (size_t depth{}; depth < chains_length; ++depth)
        {
            for (auto& next: chains)
            {
                if (depth >= next.length)
                    continue;

                const auto pays = next.pays + (next.varied ? draw(200) : 0u);
                next.last = add(next.last, pays, next.size);
            }
        }

        for (size_t parent{}; parent < chains_parents; ++parent)
        {
            const auto low = 500u + draw(1'000);
            const auto up = add(none, low, 150u + draw(300));
            const auto count = add1(draw(4));
            for (uint32_t child{}; child < count; ++child)
            {
                const auto pays = rate() * add1(draw(3));
                add(up, pays, 110u + draw(200));
            }
        }

        for (size_t at{}; at + 3u <= free.size();)
        {
            const auto size = 2u + draw(2);
            if (is_nonzero(draw(200)))
            {
                ++at;
                continue;
            }

            for (size_t member{}; member < size; ++member)
                rivals[free[at + member]] = free[at + (add1(member) % size)];

            at += size;
        }

        cells = initial;
        pool::algorithm::initialize<link_t>(columns());
        while (pool::algorithm::propagate<link_t>(columns()));
        initial = cells;
    }
};

struct sample
{
    double time;
    size_t passes;
};

// Median milliseconds of scan(), with its passes, each over fresh cells.
template <typename Scan>
static sample measure(table& columns, const Scan& scan) NOEXCEPT
{
    size_t passes{};
    std_array<double, repeats> times{};
    for (auto& time: times)
    {
        columns.reset();
        const auto start = clock_t::now();
        passes = scan();
        time = milliseconds(clock_t::now() - start).count();
    }

    std::sort(times.begin(), times.end());
    return { times[to_half(repeats)], passes };
}

// The lane width of the word is built (or the word is integral).
template <typename Word>
constexpr bool built() NOEXCEPT
{
    if constexpr (is_extended<Word>)
        return have<Word>;
    else
        return true;
}

// The label passes at the lane width of the word (or integral).
template <typename Word>
static size_t label_as(table& columns) NOEXCEPT
{
    size_t passes{};
    const auto spans = columns.columns();
    pool::algorithm::initialize<Word>(spans);

    do
    {
        ++passes;
    }
    while (pool::algorithm::propagate<Word>(spans));
    return passes;
}

template <typename Word>
static bool report_label_as(const std::string& name, table& columns,
    const links_t& expected) NOEXCEPT
{
    if (!built<Word>())
    {
        std::cout << "  " << name << ": not built" << std::endl;
        return true;
    }

    const auto result = measure(columns, [&]() NOEXCEPT
    {
        return label_as<Word>(columns);
    });

    std::cout << "  " << name << ": " << result.time << " ms, "
        << result.passes << " passes" << std::endl;

    return columns.cells == expected;
}

// The entry, as its median report by time.
template <typename Entry>
static pool::report_t median(table& columns, const Entry& entry) NOEXCEPT
{
    std_array<pool::report_t, repeats> reports{};
    for (auto& report: reports)
    {
        columns.reset();
        report = entry(columns.columns());
    }

    std::sort(reports.begin(), reports.end(),
        [](const pool::report_t& left, const pool::report_t& right) NOEXCEPT
        {
            return left.time < right.time;
        });

    return reports[to_half(repeats)];
}

template <typename Entry>
static bool report_entry(const std::string& name, table& columns,
    const links_t& expected, size_t bytes, const Entry& entry) NOEXCEPT
{
    // The first use of the gpu in a process loads its context.
    columns.reset();
    entry(columns.columns());

    const auto report = median(columns, entry);
    std::cout << "  " << name << (report.device ? " (gpu): " : " (lanes): ")
        << milliseconds(report.time).count() << " ms, " << report.passes
        << " passes, transfer " << milliseconds(report.transfer).count()
        << " ms of " << bytes << " bytes" << std::endl;

    return report.rows == columns.cells.size() && columns.cells == expected;
}

static bool report_label(const std::string& name, uint32_t odds) NOEXCEPT
{
    table columns{ table_rows, table_seed, odds };
    label_as<link_t>(columns);
    const auto expected = columns.cells;

    std::cout << "label " << name << ": " << columns.cells.size() << " rows, "
        << columns.rows.size() << " inputs, " << columns.contested()
        << " contested" << std::endl;

    const auto& same = expected;
    const auto scalar = report_label_as<link_t>("integral", columns, same);
    const auto lanes128 = report_label_as<xint128_t>("128", columns, same);
    const auto lanes256 = report_label_as<xint256_t>("256", columns, same);
    const auto lanes512 = report_label_as<xint512_t>("512", columns, same);
    const auto bytes = columns.staged();
    const auto entry = report_entry("entry", columns, same, bytes,
        pool::label);

    // The entry falls back to the lanes with the gpu disabled.
    batched::accelerate(false);
    const auto fallback = report_entry("entry", columns, same, bytes,
        pool::label);

    batched::accelerate(true);

    return scalar && lanes128 && lanes256 && lanes512 && entry && fallback;
}

// The absence passes at the lane width of the word (or integral).
template <typename Word>
static size_t settle_as(table& columns) NOEXCEPT
{
    size_t passes{};
    const auto spans = columns.columns();
    pool::algorithm::reset<Word>(spans);

    do
    {
        ++passes;
    }
    while (pool::algorithm::descend<Word>(spans));
    return passes;
}

template <typename Word>
static bool report_settle_as(const std::string& name, table& columns,
    const links_t& expected) NOEXCEPT
{
    if (!built<Word>())
    {
        std::cout << "  " << name << ": not built" << std::endl;
        return true;
    }

    const auto result = measure(columns, [&]() NOEXCEPT
    {
        return settle_as<Word>(columns);
    });

    std::cout << "  " << name << ": " << result.time << " ms, "
        << result.passes << " passes" << std::endl;

    return columns.cells == expected;
}

static bool report_settle(const std::string& name, uint32_t odds,
    bool conflicts=true) NOEXCEPT
{
    table columns{ table_rows, table_seed, odds, conflicts };
    settle_as<link_t>(columns);
    const auto expected = columns.cells;
    const auto absent = std::count_if(expected.cbegin(), expected.cend(),
        [](link_t value) NOEXCEPT
        {
            return pool::algorithm::is_absent(value);
        });

    std::cout << "settle " << name << ": " << columns.cells.size()
        << " rows, " << columns.rows.size() << " inputs, " << absent
        << " absent" << std::endl;

    const auto& same = expected;
    const auto scalar = report_settle_as<link_t>("integral", columns, same);
    const auto lanes128 = report_settle_as<xint128_t>("128", columns, same);
    const auto lanes256 = report_settle_as<xint256_t>("256", columns, same);
    const auto lanes512 = report_settle_as<xint512_t>("512", columns, same);
    const auto bytes = columns.staged();
    const auto entry = report_entry("entry", columns, same, bytes,
        pool::settle);

    // The entry falls back to the lanes with the gpu disabled.
    batched::accelerate(false);
    const auto fallback = report_entry("entry", columns, same, bytes,
        pool::settle);

    batched::accelerate(true);

    return scalar && lanes128 && lanes256 && lanes512 && entry && fallback;
}

// The context sweep at the lane width of the word (or integral).
template <typename Word>
static size_t suffice_as(table& columns, const context_t& context) NOEXCEPT
{
    return pool::algorithm::suffice<Word>(columns.columns(), context,
        table_flags);
}

template <typename Word>
static bool report_suffice_as(const std::string& name, table& columns,
    const links_t& expected, const context_t& context) NOEXCEPT
{
    if (!built<Word>())
    {
        std::cout << "  " << name << ": not built" << std::endl;
        return true;
    }

    const auto result = measure(columns, [&]() NOEXCEPT
    {
        return suffice_as<Word>(columns, context);
    });

    std::cout << "  " << name << ": " << result.time << " ms, "
        << result.passes << " insufficient" << std::endl;

    return columns.cells == expected;
}

static bool report_suffice(const std::string& name,
    const context_t& context) NOEXCEPT
{
    table columns{ table_rows, table_seed, uncontested, false };
    const auto count = suffice_as<link_t>(columns, context);
    const auto expected = columns.cells;

    std::cout << "suffice " << name << ": " << columns.cells.size()
        << " rows, " << count << " insufficient" << std::endl;

    const auto sweep = [&](const columns_t& spans) NOEXCEPT
    {
        size_t out{};
        return pool::suffice(out, spans, context, table_flags);
    };

    const auto& same = expected;
    const auto& at = context;
    const auto bytes = columns.contexts();
    const auto scalar = report_suffice_as<link_t>("integral", columns, same, at);
    const auto lanes128 = report_suffice_as<xint128_t>("128", columns, same, at);
    const auto lanes256 = report_suffice_as<xint256_t>("256", columns, same, at);
    const auto lanes512 = report_suffice_as<xint512_t>("512", columns, same, at);
    const auto entry = report_entry("entry", columns, same, bytes, sweep);
    return scalar && lanes128 && lanes256 && lanes512 && entry;
}

// The block confirmed (or unconfirmed) by the columns: the cells set, then
// settled and labeled by the entries.
static size_t columns_block(table& columns, const links_t& block,
    const links_t& conflicts, bool confirm) NOEXCEPT
{
    using algorithm = pool::algorithm;
    for (const auto row: block)
        if (confirm || columns.cells[row] == boundary)
            columns.cells[row] = confirm ? boundary : unlabeled;

    for (const auto row: conflicts)
    {
        const auto own = columns.cells[row];
        if (own != boundary)
            columns.cells[row] = confirm ?
                algorithm::raised(own, algorithm::conflicted) :
                algorithm::lowered(own, algorithm::conflicted);
    }

    const auto spans = columns.columns();
    return pool::settle(spans).passes + pool::label(spans).passes;
}

// The block confirmed (or unconfirmed) by the walk, labeled by the columns
// if the walk meets a component over the member limit.
static size_t walk_block(table& columns, const links_t& block,
    const links_t& conflicts, bool confirm) NOEXCEPT
{
    using algorithm = pool::algorithm;
    const auto spans = columns.columns();
    const auto complete = confirm ?
        algorithm::confirm(spans, block, conflicts) :
        algorithm::unconfirm(spans, block, conflicts);

    if (!complete)
        pool::label(spans);

    return complete ? one : zero;
}

// The walk and the columns, each from the initial cells, to the same cells.
static bool report_block(const std::string& name, table& columns,
    const links_t& block, const links_t& conflicts, bool confirm) NOEXCEPT
{
    const auto walk = measure(columns, [&]() NOEXCEPT
    {
        return walk_block(columns, block, conflicts, confirm);
    });

    const auto walked = columns.cells;
    const auto device = measure(columns, [&]() NOEXCEPT
    {
        return columns_block(columns, block, conflicts, confirm);
    });

    const auto scanned = columns.cells;
    batched::accelerate(false);
    const auto lanes = measure(columns, [&]() NOEXCEPT
    {
        return columns_block(columns, block, conflicts, confirm);
    });

    batched::accelerate(true);
    std::cout << "  " << name << " walk: " << walk.time << " ms, complete "
        << walk.passes << std::endl;
    std::cout << "  " << name << " columns (entry): " << device.time
        << " ms, " << device.passes << " passes" << std::endl;
    std::cout << "  " << name << " columns (lanes): " << lanes.time
        << " ms, " << lanes.passes << " passes" << std::endl;

    return walked == scanned && columns.cells == scanned;
}

// Nanoseconds a row for the row's cell made absent and then admitted, and
// for the component of the row collected and related, over sampled live rows.
static void report_rows(table& columns, const links_t& samples) NOEXCEPT
{
    using algorithm = pool::algorithm;
    using nanoseconds = std::chrono::duration<double, std::nano>;
    columns.reset();
    const auto spans = columns.columns();
    const algorithm::graph_t graph{ spans };
    size_t members{}, oversized{};

    auto start = clock_t::now();
    for (const auto row: samples)
    {
        algorithm::component_t component{};
        if (algorithm::collect(component, graph, row) &&
            algorithm::relate(component, graph))
            members += component.count;
        else
            ++oversized;
    }

    const auto related = nanoseconds(clock_t::now() - start).count();
    start = clock_t::now();
    for (const auto row: samples)
    {
        columns.cells[row] = algorithm::absent;
        if (!algorithm::admit(spans, row))
            ++oversized;
    }

    const auto admitted = nanoseconds(clock_t::now() - start).count();
    const auto count = static_cast<double>(samples.size());
    std::cout << "  collect and relate: " << (related / count)
        << " ns a row, " << (static_cast<double>(members) / count)
        << " members a row" << std::endl;
    std::cout << "  admit: " << (admitted / count) << " ns a row, "
        << oversized << " over the member limit" << std::endl;
}

static bool report_walk(const std::string& name, size_t count) NOEXCEPT
{
    pooled columns{ count, pool_live, table_seed };
    std::mt19937 generator(add1(table_seed));
    const auto floor = count - pool_live;
    const auto live = [&]() NOEXCEPT
    {
        return possible_narrow_cast<link_t>(floor +
            (generator() % pool_live));
    };

    links_t block(pool_block);
    links_t conflicts(pool_conflicts);
    links_t samples(pool_samples);
    std::generate(block.begin(), block.end(), live);
    std::generate(conflicts.begin(), conflicts.end(), live);
    std::generate(samples.begin(), samples.end(), live);

    std::cout << "walk " << name << ": " << count << " rows, " << pool_live
        << " live, " << columns.rows.size() << " inputs, block of "
        << pool_block << " with " << pool_conflicts << " conflicts"
        << std::endl;

    // The first use of the gpu in a process loads its context.
    pool::label(columns.columns());
    report_rows(columns, samples);
    const auto in = report_block("block in", columns, block, conflicts, true);

    // Block out starts from the cells of block in.
    columns.reset();
    columns_block(columns, block, conflicts, true);
    columns.initial = columns.cells;
    const auto out = report_block("block out", columns, block, conflicts,
        false);

    return in && out;
}

// One template of the pool, a fee projection of blocks from it, and the fee
// estimates from a projection.
static bool report_template(const std::string& name, table& columns,
    size_t live) NOEXCEPT
{
    using algorithm = pool::algorithm;
    const auto count = columns.cells.size();
    const auto spans = columns.columns();

    std::cout << "template " << name << ": " << count << " rows, " << live
        << " live" << std::endl;

    const auto one_block = measure(columns, [&]() NOEXCEPT
    {
        links_t out{};
        std_vector<algorithm::mark> marks(count);
        algorithm::resolve(marks, spans, template_outcomes, 0,
            template_sigop_bytes);
        algorithm::pack(out, marks, spans, template_budget, template_depth,
            template_sigop_bytes);

        return out.size();
    });

    const auto blocks = measure(columns, [&]() NOEXCEPT
    {
        links_t out{};
        algorithm::projection_t projection{};
        projection.marks.resize(count);
        algorithm::resolve(projection.marks, spans, template_outcomes, 0,
            template_sigop_bytes);
        for (size_t block{}; block < template_blocks; ++block)
            algorithm::project(out, projection, spans, template_budget,
                template_depth, template_sigop_bytes);

        return projection.blocks.size();
    });

    links_t out{};
    algorithm::projection_t projection{};
    projection.marks.resize(count);
    algorithm::resolve(projection.marks, spans, template_outcomes, 0,
        template_sigop_bytes);
    for (size_t block{}; block < template_blocks; ++block)
        algorithm::project(out, projection, spans, template_budget,
            template_depth, template_sigop_bytes);

    const auto arrivals = [](uint64_t rate) NOEXCEPT
    {
        return template_arrivals * 1'000u / greater(rate, 1'000_u64);
    };

    const auto estimates = measure(columns, [&]() NOEXCEPT
    {
        size_t reached{};
        for (const auto target: template_targets)
            if (algorithm::estimate(projection, target, template_budget.size,
                template_room, arrivals) != max_uint64)
                ++reached;

        return reached;
    });

    std::cout << "  template: " << one_block.time << " ms, "
        << one_block.passes << " rows taken" << std::endl;
    std::cout << "  projection: " << blocks.time << " ms, " << blocks.passes
        << " blocks" << std::endl;
    std::cout << "  estimates: " << estimates.time << " ms, "
        << estimates.passes << " of " << template_targets.size()
        << " targets reached" << std::endl;

    return is_nonzero(one_block.passes) && is_nonzero(blocks.passes);
}

// The inputs spending the row, found by equality over the rows column.
template <typename Word>
static size_t find_as(links_t& out, const table& columns,
    link_t row) NOEXCEPT
{
    pool::algorithm::find<Word>(out, columns.rows, row);
    return out.size();
}

template <typename Word>
static bool report_find_as(const std::string& name, table& columns,
    link_t row, size_t expected) NOEXCEPT
{
    if (!built<Word>())
    {
        std::cout << "  " << name << ": not built" << std::endl;
        return true;
    }

    links_t out{};
    const auto result = measure(columns, [&]() NOEXCEPT
    {
        return find_as<Word>(out, columns, row);
    });

    std::cout << "  " << name << ": " << result.time << " ms, "
        << result.passes << " found" << std::endl;

    return result.passes == expected;
}

static bool report_find() NOEXCEPT
{
    constexpr link_t row = 1'000;
    table columns{ table_rows, table_seed, uncontested };
    links_t out{};
    const auto same = find_as<link_t>(out, columns, row);

    std::cout << "find: " << columns.rows.size() << " inputs" << std::endl;

    const auto scalar = report_find_as<link_t>("integral", columns, row, same);
    const auto lanes128 = report_find_as<xint128_t>("128", columns, row, same);
    const auto lanes256 = report_find_as<xint256_t>("256", columns, row, same);
    const auto lanes512 = report_find_as<xint512_t>("512", columns, row, same);

    return scalar && lanes128 && lanes256 && lanes512;
}

// label
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__accelerated__reported)
{
    std::cout << "compiled: " << batched::compiled() << ", accelerated: " << batched::accelerated() << std::endl;
}

BC_AUTO_TEST_CASE(pool_performance__label__uncontested__reported)
{
    BC_REQUIRE(report_label("uncontested", uncontested));
}

BC_AUTO_TEST_CASE(pool_performance__label__sparse__reported)
{
    BC_REQUIRE(report_label("sparse", sparse));
}

BC_AUTO_TEST_CASE(pool_performance__label__dense__reported)
{
    BC_REQUIRE(report_label("dense", dense));
}

// settle
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__settle__clear__reported)
{
    BC_REQUIRE(report_settle("clear", uncontested, false));
}

BC_AUTO_TEST_CASE(pool_performance__settle__uncontested__reported)
{
    BC_REQUIRE(report_settle("uncontested", uncontested));
}

BC_AUTO_TEST_CASE(pool_performance__settle__sparse__reported)
{
    BC_REQUIRE(report_settle("sparse", sparse));
}

BC_AUTO_TEST_CASE(pool_performance__settle__dense__reported)
{
    BC_REQUIRE(report_settle("dense", dense));
}

// suffice
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__suffice__sufficient__reported)
{
    BC_REQUIRE(report_suffice("sufficient", { table_flags, table_heights, table_heights }));
}

BC_AUTO_TEST_CASE(pool_performance__suffice__regressed__reported)
{
    BC_REQUIRE(report_suffice("regressed", { table_flags, table_heights - 10u, table_heights }));
}

// walk
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__walk__one_million__reported)
{
    BC_REQUIRE(report_walk("1M", 1'000'000));
}

BC_AUTO_TEST_CASE(pool_performance__walk__ten_million__reported)
{
    BC_REQUIRE(report_walk("10M", 10'000'000));
}

// template
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__template__one_million__reported)
{
    pooled columns{ 1'000'000, pool_live, table_seed };
    BC_REQUIRE(report_template("1M", columns, pool_live));
}

BC_AUTO_TEST_CASE(pool_performance__template__ten_million__reported)
{
    pooled columns{ 10'000'000, pool_live, table_seed };
    BC_REQUIRE(report_template("10M", columns, pool_live));
}

BC_AUTO_TEST_CASE(pool_performance__template__chains__reported)
{
    constexpr size_t confirmed = 1'000'000;
    chained columns{ confirmed, table_seed };
    BC_REQUIRE(report_template("chains", columns, columns.cells.size() - confirmed));
}

// find
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(pool_performance__find__inputs__reported)
{
    BC_REQUIRE(report_find());
}

BC_AUTO_TEST_SUITE_END()

#endif
