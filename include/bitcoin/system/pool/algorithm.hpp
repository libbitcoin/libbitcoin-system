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

/// Conflict components and outcomes of pooled transactions, over the columns
/// of the pool. Rows are pool links and inputs are spends links, each column
/// aligned with its index. A cell holds the state and component label of a
/// row. Rows are related by parent rows (an input to the row it spends),
/// children (a list through the inputs spending a row) and rivals (a ring
/// through the inputs spending a point).
///
/// A row is relevant if it is in a clique (two or more present rivals of a
/// point) or has a relevant parent. Only relevant rows are labeled, so an
/// uncontested row of any ancestry is in no component.
///
/// Frontier operations walk from a row and touch only its component and
/// ancestry. Column operations pass over every row, one element per lane of
/// an extended integral word.
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

    /// An edge from a table row or input to a candidate input.
    struct bridge_t
    {
        link_t from{};
        link_t to{};
    };

    /// Rows of a component, the ancestors of each member (transitive within
    /// the component) and the members spending each contested point, each as
    /// a mask over the member indexes.
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
    };

    /// The table columns with candidate columns appended, as rows and inputs
    /// numbered from the table counts, and the edges from the table into the
    /// candidates that the table columns cannot hold.
    struct graph_t
    {
        columns_t table{};
        columns_t extra{};

        /// Table row to a candidate input spending it.
        std::span<const bridge_t> children{};

        /// Table input to a candidate input spending the same point.
        std::span<const bridge_t> rivals{};
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

    /// Cells.
    /// -----------------------------------------------------------------------

    /// The row can confirm (present, labeled or not).
    static constexpr bool is_present(link_t cell) NOEXCEPT;

    /// The row is in a component (which implies present).
    static constexpr bool is_labeled(link_t cell) NOEXCEPT;

    /// The row has confirmed.
    static constexpr bool is_boundary(link_t cell) NOEXCEPT;

    /// Chargeable size of a row (virtual size or sigops at bytes per sigop).
    static constexpr uint64_t chargeable(const totals_t& totals,
        uint32_t bytes_per_sigop) NOEXCEPT;

    /// Candidates.
    /// -----------------------------------------------------------------------

    /// Each candidate cell set present, labeled by its own index if relevant,
    /// in candidate order (parents precede children).
    static constexpr void initialize(const graph_t& graph,
        const std::span<const link_t>& candidates) NOEXCEPT;

    /// Components (frontier).
    /// -----------------------------------------------------------------------

    /// The component of the row, by walking its relevant parents, present
    /// children and present rivals, false if more than maximum_members.
    static constexpr bool collect(component_t& out, const graph_t& graph,
        link_t row) NOEXCEPT;

    /// Ancestor and clique masks of the collected rows, false if more cliques
    /// than maximum_members.
    static constexpr bool relate(component_t& out,
        const graph_t& graph) NOEXCEPT;

    /// Mask of the members among the rows.
    static constexpr mask_t members(const component_t& component,
        const std::span<const link_t>& rows) NOEXCEPT;

    /// Totals of the members and of their present ancestors outside the
    /// component, false if over the limit in size or in sigops.
    static bool ancestry(totals_t& out, const graph_t& graph,
        const component_t& component, const totals_t& limit) NOEXCEPT;

    /// Outcomes.
    /// -----------------------------------------------------------------------

    /// Number of outcomes, the product of the clique sizes.
    static constexpr size_t outcomes(const component_t& component) NOEXCEPT;

    /// The outcome at index: one winner per clique by mixed radix, the losers
    /// and their descendants excluded, so conflict-free and ancestry-closed.
    static constexpr mask_t outcome(const component_t& component,
        size_t index) NOEXCEPT;

    /// Highest fee over the outcomes, and over those excluding every member
    /// of the candidate mask, one outcome per lane.
    template <typename Word>
    static constexpr void evaluate(uint64_t& best, uint64_t& best_without,
        const std::span<const mask_t>& outcomes, mask_t candidate,
        const component_t& component, const graph_t& graph) NOEXCEPT;

    /// The fee added by the candidate covers its bytes at the rate (satoshis
    /// per virtual kilobyte).
    static constexpr bool covered(uint64_t best, uint64_t best_without,
        uint64_t candidate_bytes, uint64_t rate) NOEXCEPT;

    /// Transitions (frontier, table columns).
    /// -----------------------------------------------------------------------

    /// Every member labeled by the lowest member row.
    static constexpr void relabel(const columns_t& columns,
        const component_t& component) NOEXCEPT;

    /// The row and its present descendants made absent.
    static void exclude(const columns_t& columns, link_t row) NOEXCEPT;

    /// The row made present and labeled into its component (which may merge
    /// components).
    static void admit(const columns_t& columns, link_t row) NOEXCEPT;

    /// The row made boundary, its present rivals excluded, and the rest of
    /// its component relabeled (which may split it).
    static void confirm(const columns_t& columns, link_t row) NOEXCEPT;

    /// The row made present, its confirmable rivals and their confirmable
    /// descendants revived, and each relabeled into its component.
    static void unconfirm(const columns_t& columns, link_t row) NOEXCEPT;

    /// The row made present if absent and confirmable, with its confirmable
    /// descendants, and each relabeled into its component.
    static void revive(const columns_t& columns, link_t row) NOEXCEPT;

    /// Columns (whole table).
    /// -----------------------------------------------------------------------

    /// Each value at its index (out[i] = values[indexes[i]]), none for none.
    template <typename Word>
    static constexpr void gather(std::span<link_t> out,
        const std::span<const link_t>& values,
        const std::span<const link_t>& indexes) NOEXCEPT;

    /// Each present cell labeled by its own index if in a clique, otherwise
    /// unlabeled.
    static constexpr void initialize(const columns_t& columns) NOEXCEPT;

    /// One pass of each present row taking the minimum label over itself, its
    /// relevant parents, its rivals and (if relevant) its children, true if
    /// any cell changed. Repeated to a fixed point, labels identify
    /// components. Scratch holds one link per input.
    template <typename Word>
    static constexpr bool propagate(const columns_t& columns,
        std::span<link_t> scratch) NOEXCEPT;

    /// Rows with the label, false if more than maximum_members.
    template <typename Word>
    static constexpr bool collect(component_t& out, link_t label,
        const std::span<const link_t>& cells) NOEXCEPT;

    /// Indexes of the values equal to the value.
    template <typename Word>
    static void find(std::vector<link_t>& out,
        const std::span<const link_t>& values, link_t value) NOEXCEPT;

protected:
    /// Internals.
    /// -----------------------------------------------------------------------

    /// Graph navigation over table and candidate columns.
    static constexpr size_t rows(const graph_t& graph) NOEXCEPT;
    static constexpr link_t& cell(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr edges_t edges(const graph_t& graph, link_t row) NOEXCEPT;
    static constexpr const totals_t& totals(const graph_t& graph,
        link_t row) NOEXCEPT;
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

    /// The row has no absent parent and no boundary rival.
    static constexpr bool confirmable(const graph_t& graph,
        link_t row) NOEXCEPT;

    /// Member index of the row, or maximum_members if not a member.
    static constexpr size_t index(const component_t& component,
        link_t row) NOEXCEPT;

    /// Mask of the losers of the outcome at index.
    static constexpr mask_t losers(const component_t& component,
        size_t index) NOEXCEPT;

    /// The rows made present (if confirmable), with confirmable descendants.
    static void restore(std::vector<link_t>& out, const columns_t& columns,
        link_t row) NOEXCEPT;

    /// Each present row given its own label if relevant else unlabeled, then
    /// collected and relabeled into its component (one over maximum_members
    /// keeps its own labels).
    static void reform(const columns_t& columns,
        const std::span<const link_t>& rows) NOEXCEPT;

    /// Lanes of fee sums of the outcomes, members broadcast and selected.
    template <typename Word>
    INLINE static constexpr Word fees(Word outcomes,
        const component_t& component, const graph_t& graph) NOEXCEPT;

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

#endif
