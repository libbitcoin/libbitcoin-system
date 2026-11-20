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
#ifndef LIBBITCOIN_SYSTEM_POOL_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_POOL_ALGORITHM_HPP

#include <span>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pool {

/// Conflict components, outcomes and templates over the pool columns.
class algorithm
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Row index (pool link), input index (spends link) and label.
    using link_t = uint32_t;

    /// Members of a component, one bit each.
    using mask_t = uint64_t;

    /// Fee, virtual size and sigops of a row.
    struct totals_t
    {
        uint64_t fee{};
        uint32_t size{};
        uint32_t sigops{};
    };

    /// The inputs of a row (first index and count).
    struct edges_t
    {
        link_t first{};
        link_t count{};
    };

    /// Flags, height and median time past of a validation.
    struct context_t
    {
        uint32_t flags{};
        uint32_t height{};
        uint32_t median{};
    };

    /// An edge from a table row or input to a candidate input.
    struct bridge_t
    {
        link_t from{};
        link_t to{};
    };

    /// Rows of a component, with ancestor and clique masks over its members.
    struct component_t
    {
        std_array<link_t, bits<mask_t>> rows{};
        std_array<mask_t, bits<mask_t>> ancestors{};
        std_array<mask_t, bits<mask_t>> cliques{};
        size_t count{};
        size_t contests{};
    };

    /// Columns of rows and of their inputs.
    struct columns_t
    {
        /// State and label of the row.
        std::span<link_t> cells{};

        /// First input spending the row, none if childless.
        std::span<const link_t> children{};

        /// Inputs of the row.
        std::span<const edges_t> edges{};

        /// Totals of the row.
        std::span<const totals_t> totals{};

        /// Row spent by the input, none if its parent is not a row.
        std::span<const link_t> rows{};

        /// Next input spending the same row, none at the end.
        std::span<const link_t> siblings{};

        /// Next input spending the same point (a ring), none if sole.
        std::span<const link_t> rivals{};

        /// Flags, height and median time past of the validation of the row.
        std::span<const uint32_t> flags{};
        std::span<const uint32_t> heights{};
        std::span<const uint32_t> medians{};
    };

    /// The table columns with candidate columns and bridging edges appended.
    struct graph_t
    {
        columns_t table{};
        columns_t extra{};

        /// Table row to a candidate input spending it.
        std::span<const bridge_t> children{};

        /// Table input to a candidate input spending the same point.
        std::span<const bridge_t> rivals{};
    };

    /// Mark of a row in a template.
    enum class mark : uint8_t
    {
        /// Present and in the chosen outcome of its component, if any.
        unmarked,

        /// Not present, or not in the chosen outcome of its component.
        excluded,

        /// In the template.
        taken
    };

    /// Rate and chargeable size of a chunk.
    struct take_t
    {
        uint64_t rate{};
        uint64_t size{};
    };

    /// The chunks taken by a block.
    using takes_t = std::vector<take_t>;

    /// Rows to recompute and rows to relabel in a walk of the table, and if
    /// no component of more than maximum_members was met.
    struct walk_t
    {
        std::vector<link_t> pending{};
        std::vector<link_t> affected{};
        bool complete{ true };
    };

    /// Blocks taken in turn from the rows, and the marks that continue them.
    struct projection_t
    {
        std::vector<mark> marks{};
        std::vector<takes_t> blocks{};
        bool exhausted{};
    };

    /// Lane counts of a word.
    template <typename Word>
    static constexpr auto link_lanes = capacity<Word, link_t>;
    template <typename Word>
    static constexpr auto mask_lanes = capacity<Word, mask_t>;

    /// Constants.
    /// -----------------------------------------------------------------------

    /// A component holds at most one member, and one clique, per mask bit.
    static constexpr size_t maximum_members = bits<mask_t>;

    /// No row or input.
    static constexpr link_t none = max_uint32;

    /// Cell of a present row in no component (the label bits).
    static constexpr link_t unlabeled = sub1(power2<link_t>(bits<link_t> - 2));

    /// Cell of a row that cannot confirm (absent) or has confirmed (boundary).
    static constexpr link_t absent = add1(unlabeled);
    static constexpr link_t boundary = shift_left(absent, one);

    /// Cause of absence (flag of an absent cell): a parent is absent.
    static constexpr link_t ancestor = power2<link_t>(0u);

    /// Cause of absence: a rival has confirmed.
    static constexpr link_t rivaled = power2<link_t>(1u);

    /// Cause of absence: a confirmed tx spends its point (a block conflict).
    static constexpr link_t conflicted = power2<link_t>(2u);

    /// Cause of absence: its context is not sufficient.
    static constexpr link_t insufficient = power2<link_t>(3u);

    /// The causes derived from the columns.
    static constexpr link_t derived = bit_or(ancestor, rivaled);

    /// Cells.
    /// -----------------------------------------------------------------------

    /// The row can confirm (present, labeled or not).
    static constexpr bool is_present(link_t cell) NOEXCEPT;

    /// The row is in a component (which implies present).
    static constexpr bool is_labeled(link_t cell) NOEXCEPT;

    /// The row has confirmed.
    static constexpr bool is_boundary(link_t cell) NOEXCEPT;

    /// The row cannot confirm, for the causes it carries.
    static constexpr bool is_absent(link_t cell) NOEXCEPT;

    /// The cell made absent for the cause.
    static constexpr link_t raised(link_t cell, link_t cause) NOEXCEPT;

    /// The absent cell without the cause, present if none remains.
    static constexpr link_t lowered(link_t cell, link_t cause) NOEXCEPT;

    /// Stored flags include current and required, height and mtp not greater.
    static constexpr bool is_sufficient(const context_t& stored,
        const context_t& current, uint32_t required) NOEXCEPT;

    /// Chargeable size of a row (virtual size or sigops at bytes per sigop).
    static constexpr uint64_t chargeable(const totals_t& totals,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// Fee rate of a row (satoshis per chargeable kilobyte).
    static constexpr uint64_t fee_rate(const totals_t& totals,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// Column values (stored little-endian).
    /// -----------------------------------------------------------------------

    /// The value as stored in a column.
    static constexpr link_t to_column(link_t value) NOEXCEPT;
    static constexpr edges_t to_column(const edges_t& value) NOEXCEPT;
    static constexpr totals_t to_column(const totals_t& value) NOEXCEPT;

    /// The value as read from a column.
    static constexpr link_t from_column(link_t value) NOEXCEPT;
    static constexpr edges_t from_column(const edges_t& value) NOEXCEPT;
    static constexpr totals_t from_column(const totals_t& value) NOEXCEPT;

    /// Candidates.
    /// -----------------------------------------------------------------------

    /// Each candidate cell set present, labeled by its own index if relevant.
    static constexpr void initialize(const graph_t& graph,
        const std::span<const link_t>& candidates) NOEXCEPT;

    /// Components (frontier).
    /// -----------------------------------------------------------------------

    /// The component of the row, false if more than maximum_members.
    static constexpr bool collect(component_t& out, const graph_t& graph,
        link_t row) NOEXCEPT;

    /// Ancestor and clique masks of the rows, false if over maximum_members.
    static constexpr bool relate(component_t& out,
        const graph_t& graph) NOEXCEPT;

    /// Mask of the members among the rows.
    static constexpr mask_t members(const component_t& component,
        const std::span<const link_t>& rows) NOEXCEPT;

    /// Totals of the members and outside ancestors, false if over the limit.
    static bool ancestry(totals_t& out, const graph_t& graph,
        const component_t& component, const totals_t& limit) NOEXCEPT;

    /// Outcomes.
    /// -----------------------------------------------------------------------

    /// Number of outcomes, the product of the clique sizes.
    static constexpr size_t outcomes(const component_t& component) NOEXCEPT;

    /// The outcome at index, one winner per clique by mixed radix.
    static constexpr mask_t outcome(const component_t& component,
        size_t index) NOEXCEPT;

    /// Highest fee over the outcomes, and over those without the candidate.
    template <typename Word = xint512_t>
    static constexpr void evaluate(uint64_t& best, uint64_t& best_without,
        const std::span<const mask_t>& outcomes, mask_t candidate,
        const component_t& component, const graph_t& graph) NOEXCEPT;

    /// The fee added by the candidate covers its bytes at the rate.
    static constexpr bool covered(uint64_t best, uint64_t best_without,
        uint64_t candidate_bytes, uint64_t rate) NOEXCEPT;

    /// Transitions (frontier, table columns).
    /// -----------------------------------------------------------------------

    /// Every member labeled by the lowest member row.
    static constexpr void relabel(const columns_t& columns,
        const component_t& component) NOEXCEPT;

    /// Each transition brings the causes and labels of the rows it reaches up
    /// to date, and is false if it met a component of more than
    /// maximum_members, which leaves labels to be made by the columns.

    /// The row made present, or conflicted (a confirmed tx spends its point).
    static bool admit(const columns_t& columns, link_t row,
        bool conflict=false) NOEXCEPT;

    /// The rows made boundary and the conflicts conflicted.
    static bool confirm(const columns_t& columns,
        const std::span<const link_t>& rows,
        const std::span<const link_t>& conflicts) NOEXCEPT;

    /// The boundary rows made unconfirmed and the conflicts not conflicted.
    static bool unconfirm(const columns_t& columns,
        const std::span<const link_t>& rows,
        const std::span<const link_t>& conflicts) NOEXCEPT;

    /// Template (frontier, table columns).
    /// -----------------------------------------------------------------------

    /// Rows absent from the best outcome of their component marked excluded.
    template <typename Word = xint512_t>
    static void resolve(std::span<mark> marks, const columns_t& columns,
        size_t maximum_outcomes) NOEXCEPT;

    /// The unmarked rows taken by chunk rate within the budget, ascending.
    static void pack(std::vector<link_t>& out, std::span<mark> marks,
        const columns_t& columns, const totals_t& budget, size_t depth,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// The next block of the projection taken by chunk rate, as its rows.
    static void project(std::vector<link_t>& out, projection_t& projection,
        const columns_t& columns, const totals_t& budget, size_t depth,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// Estimate (projected blocks).
    /// -----------------------------------------------------------------------

    /// Chargeable size of the takes at or above the rate.
    static constexpr uint64_t above(const std::span<const take_t>& takes,
        uint64_t rate) NOEXCEPT;

    /// Lowest rate taken within the count of blocks, max_uint64 if none.
    template <typename Arrivals>
    static uint64_t estimate(const projection_t& projection, size_t count,
        uint64_t space, const Arrivals& arrivals) NOEXCEPT;

    /// Columns (whole table).
    /// -----------------------------------------------------------------------

    /// Each present cell labeled by its own index if in a clique.
    template <typename Word = xint512_t>
    static constexpr void initialize(const columns_t& columns) NOEXCEPT;

    /// One pass of minimum label propagation, true if any cell changed.
    template <typename Word = xint512_t>
    static constexpr bool propagate(const columns_t& columns) NOEXCEPT;

    /// Each absent cell left its stored causes, rivals of the confirmed absent.
    template <typename Word = xint512_t>
    static constexpr void reset(const columns_t& columns) NOEXCEPT;

    /// One pass of absence from parents to children, true if any cell changed.
    template <typename Word = xint512_t>
    static constexpr bool descend(const columns_t& columns) NOEXCEPT;

    /// Each unconfirmed cell absent or not for its context, the count absent.
    template <typename Word = xint512_t>
    static constexpr size_t suffice(const columns_t& columns,
        const context_t& context, uint32_t required) NOEXCEPT;

    /// Rows with the label, false if more than maximum_members.
    template <typename Word = xint512_t>
    static constexpr bool collect(component_t& out, link_t label,
        const std::span<const link_t>& cells) NOEXCEPT;

    /// Indexes of the values equal to the value.
    template <typename Word = xint512_t>
    static void find(std::vector<link_t>& out,
        const std::span<const link_t>& values, link_t value) NOEXCEPT;

protected:
    /// Internals.
    /// -----------------------------------------------------------------------

    /// Graph navigation over table and candidate columns.
    static constexpr size_t rows(const graph_t& graph) NOEXCEPT;
    static constexpr link_t cell(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr void set_cell(const graph_t& graph, link_t row,
        link_t value) NOEXCEPT;
    static constexpr edges_t edges(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr totals_t totals(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr link_t child(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr link_t parent(const graph_t& graph, link_t input) NOEXCEPT;

    /// The row of the input, by search of the edges (ordered by first input).
    static constexpr link_t owner(const graph_t& graph, link_t input) NOEXCEPT;
    static constexpr link_t sibling(const graph_t& graph,
        link_t input) NOEXCEPT;
    static constexpr link_t rival(const graph_t& graph, link_t input) NOEXCEPT;

    /// Each child row of the row (table list and bridges).
    template <typename Visitor>
    static constexpr void each_child(const graph_t& graph, link_t row,
        const Visitor& visit) NOEXCEPT;

    /// Each other input spending the input's point (ring and bridges).
    template <typename Visitor>
    static constexpr void each_rival(const graph_t& graph, link_t input,
        const Visitor& visit) NOEXCEPT;

    /// The row is in a clique (has a present rival).
    static constexpr bool contested(const graph_t& graph, link_t row) NOEXCEPT;

    /// The row has a relevant parent.
    static constexpr bool descended(const graph_t& graph, link_t row) NOEXCEPT;

    /// Member index of the row, or maximum_members if not a member.
    static constexpr size_t index(const component_t& component,
        link_t row) NOEXCEPT;

    /// Mask of the losers of the outcome at index.
    static constexpr mask_t losers(const component_t& component,
        size_t index) NOEXCEPT;

    /// Index of the outcome of highest fee (the first of equals).
    template <typename Word>
    static constexpr size_t best(const std::span<const mask_t>& outcomes,
        const component_t& component, const graph_t& graph) NOEXCEPT;

    /// The row and its descendants marked excluded.
    static void discard(std::span<mark> marks, const graph_t& graph,
        link_t row) NOEXCEPT;

    /// The component of the row and its descendants marked excluded.
    static void forfeit(std::span<mark> marks, const graph_t& graph,
        link_t row) NOEXCEPT;

    /// As pack, with the rate and chargeable size of each chunk taken.
    static void pack_(std::vector<link_t>& out, takes_t& takes,
        std::span<mark> marks, const columns_t& columns,
        const totals_t& budget, size_t depth,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// The rate is taken within the count of blocks.
    template <typename Arrivals>
    static bool reaches(const projection_t& projection, size_t count,
        uint64_t space, uint64_t rate, const Arrivals& arrivals) NOEXCEPT;

    /// The rows and totals of the chunk of the row, false if none fits.
    static bool chunk(std::vector<link_t>& out, totals_t& sum,
        const std::span<const mark>& marks, const graph_t& graph, link_t row,
        size_t depth, const totals_t& available) NOEXCEPT;

    /// The cell of the row with its derived causes recomputed.
    static link_t resolved(const graph_t& graph, link_t row) NOEXCEPT;

    /// The cell of the row set, with the rows that depend on the change left
    /// pending and those whose labels it affects noted.
    static void change(walk_t& walk, const columns_t& columns, link_t row,
        link_t next) NOEXCEPT;

    /// The pending rows recomputed, with those their changes reach, and the
    /// affected rows relabeled, false if a component is over maximum_members.
    static bool spread(walk_t& walk, const columns_t& columns) NOEXCEPT;

    /// Each present row labeled into its component, false if one is over
    /// maximum_members.
    static bool reform(const columns_t& columns,
        const std::span<const link_t>& rows) NOEXCEPT;

    /// The next narrower extended word, else the integral.
    template <typename Word, typename Integral>
    using narrower = iif<is_same_type<Word, xint512_t>, xint256_t,
        iif<is_same_type<Word, xint256_t>, xint128_t, Integral>>;

    /// Lanes of fee sums of the outcomes, members broadcast and selected.
    template <typename Word>
    INLINE static constexpr Word fees(Word outcomes,
        const component_t& component, const graph_t& graph) NOEXCEPT;

    /// Each outcome index and its fee, from the index.
    template <typename Word, typename Visitor>
    static constexpr void each_fee(size_t index,
        const std::span<const mask_t>& outcomes, const component_t& component,
        const graph_t& graph, const Visitor& visit) NOEXCEPT;

    /// The first row from the row whose inputs end beyond the input.
    static constexpr link_t advance(const graph_t& graph, link_t row,
        link_t input) NOEXCEPT;

    /// The row holds the input.
    static constexpr bool holds(const graph_t& graph, link_t row,
        link_t input) NOEXCEPT;

    /// The input's row labeled by its own index if unlabeled with a rival.
    template <typename Owner>
    static constexpr void seed(const graph_t& graph, link_t row, link_t input,
        const Owner& owning) NOEXCEPT;

    /// The input's row and parent lowered to its least label, true if changed.
    template <typename Owner, typename Lower>
    static constexpr bool exchange(const graph_t& graph, link_t row,
        link_t input, const Owner& owning, const Lower& lower) NOEXCEPT;

    /// The seed and exchange of an input over the table columns alone.
    static constexpr void seed(const graph_t& graph, link_t input) NOEXCEPT;
    static constexpr bool exchange(const graph_t& graph, link_t row,
        link_t input) NOEXCEPT;

    /// The cell without its derived causes, present if none remains.
    static constexpr link_t reduced(link_t cell) NOEXCEPT;

    /// The input's row made absent for a confirmed rival, true if changed.
    template <typename Owner, typename Raise>
    static constexpr bool contest(const graph_t& graph, link_t row,
        link_t input, const Owner& owning, const Raise& raise) NOEXCEPT;

    /// The input's row made absent for an absent parent, true if changed.
    template <typename Raise>
    static constexpr bool inherit(const graph_t& graph, link_t row,
        link_t input, const Raise& raise) NOEXCEPT;

    /// The contest and inherit of an input over the table columns alone.
    static constexpr bool contest(const graph_t& graph, link_t input) NOEXCEPT;
    static constexpr bool inherit(const graph_t& graph, link_t row,
        link_t input) NOEXCEPT;

    /// The row's cell absent or not for its context, true if absent for it.
    static constexpr bool suffice(const columns_t& columns, link_t row,
        const context_t& context, uint32_t required) NOEXCEPT;

    /// Lanes of the stored words in native order (and the reverse).
    template <typename Word>
    INLINE static constexpr Word native(Word word) NOEXCEPT;

    /// Lanes of the inputs that can lower a cell or that have a rival.
    template <typename Word>
    INLINE static constexpr Word active(Word own, Word parents, Word rivals,
        const link_t* cells) NOEXCEPT;

    /// The lane operations, from the index.
    template <typename Word>
    static constexpr void unlabel_(size_t index,
        std::span<link_t> cells) NOEXCEPT;
    template <typename Word>
    static constexpr void seed_(size_t index, const graph_t& graph) NOEXCEPT;
    template <typename Word>
    static constexpr bool propagate_(size_t index, link_t& row,
        const graph_t& graph) NOEXCEPT;
    template <typename Word>
    static constexpr void reduce_(size_t index,
        std::span<link_t> cells) NOEXCEPT;
    template <typename Word>
    static constexpr void contest_(size_t index,
        const graph_t& graph) NOEXCEPT;
    template <typename Word>
    static constexpr bool descend_(size_t index, link_t& row,
        const graph_t& graph) NOEXCEPT;
    template <typename Word>
    static constexpr size_t suffice_(size_t index, const columns_t& columns,
        const context_t& context, uint32_t required) NOEXCEPT;
    template <typename Word>
    static constexpr bool collect_(size_t index, component_t& out,
        link_t label, const std::span<const link_t>& cells) NOEXCEPT;
    template <typename Word>
    static void find_(size_t index, std::vector<link_t>& out,
        const std::span<const link_t>& values, link_t value) NOEXCEPT;

    /// Lanes of integrals at the pointer.
    template <typename Word, typename Integral>
    INLINE static constexpr Word load(const Integral* at) NOEXCEPT;
    template <typename Word, typename Integral>
    INLINE static constexpr void store(Integral* at, Word word) NOEXCEPT;
};

} // namespace pool
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/pool/algorithm.ipp>
#include <bitcoin/system/impl/pool/label.ipp>
#include <bitcoin/system/impl/pool/settle.ipp>
#include <bitcoin/system/impl/pool/suffice.ipp>

#endif
