/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_UTREEXO_HPP
#define LIBBITCOIN_SYSTEM_UTREEXO_HPP

#include <bit>
#include <tuple>
#include <utility>
#include <bitcoin/system.hpp>

namespace libbitcoin {
namespace system {
namespace utreexo {
    
using positions = std::vector<uint64_t>;
using node_hash = hash_digest;
constexpr auto empty_hash = node_hash{};
constexpr auto dummy_hash = base16_hash("4242424242424242424242424242424242424242424242424242424242424242");

constexpr node_hash parent_hash(const node_hash& left,
    const node_hash& right) NOEXCEPT
{
    return sha512_256::hash(left, right);
}

constexpr node_hash hash_from_u8(uint8_t byte) NOEXCEPT
{
    return sha256::hash(byte);
}

constexpr uint64_t parent(uint64_t child, uint8_t forest_rows) NOEXCEPT
{
    return set_right(shift_right(child), forest_rows);
}

constexpr uint64_t children(uint64_t parent, uint8_t forest_rows) NOEXCEPT
{
    // What happens when these bits are lost?
    BC_ASSERT(!is_left_shift_overflow(parent, add1<size_t>(forest_rows)));

    return bit_and(shift_left(parent),
        unmask_right<uint64_t>(add1<size_t>(forest_rows)));
}

constexpr uint64_t left_child(uint64_t parent, uint8_t forest_rows) NOEXCEPT
{
    return children(parent, forest_rows);
}

constexpr uint64_t right_child(uint64_t parent, uint8_t forest_rows) NOEXCEPT
{
    // What happens when these bits are lost?
    BC_ASSERT(!is_add_overflow<uint64_t>(children(parent, forest_rows), one));

    return add1(children(parent, forest_rows));
}

constexpr uint64_t left_sibling(uint64_t node) NOEXCEPT
{
    return set_right(node, zero, false);
}

constexpr bool is_left_niece(uint64_t node) NOEXCEPT
{
    return !get_right(node);
}

constexpr bool is_right_sibling(uint64_t node, uint64_t next) NOEXCEPT
{
    return set_right(node) == next;
}

constexpr bool is_sibling(uint64_t node1, uint64_t node2) NOEXCEPT
{
    return bit_xor<uint64_t>(node1, one) == node2;
}

// NOTE: parameters reversed from example.
constexpr bool is_root_populated(uint64_t leaves, uint8_t row) NOEXCEPT
{
    return get_right(leaves, row);
}

constexpr uint8_t detect_row(uint64_t node, uint8_t forest_rows) NOEXCEPT
{
    auto bit = forest_rows;
    while (!is_zero(bit) && get_right(node, bit)) { --bit; }
    return subtract(forest_rows, bit);
}

// TODO: arbitrary behavior if given row does not have a root.
constexpr uint64_t root_position(uint64_t leaves, uint8_t row,
    uint8_t forest_rows) NOEXCEPT
{
    BC_ASSERT(!is_subtract_overflow<size_t>(add1<size_t>(forest_rows), row));

    const auto mask = unmask_right<uint64_t>(add1<size_t>(forest_rows));
    const auto before = bit_and(leaves, shift_left(mask, add1<size_t>(row)));
    const auto left = shift_left(mask, subtract(add1<size_t>(forest_rows), row));
    const auto right = shift_right(before, row);
    const auto shifted = bit_or(left, right);
    return bit_and(shifted, mask);
}

constexpr bool is_root_position(uint64_t position, uint64_t leaves,
    uint8_t forest_rows) NOEXCEPT
{
    const auto row = detect_row(position, forest_rows);
    return get_right(leaves, row) &&
        (position == root_position(leaves, row, forest_rows));
}

constexpr uint64_t remove_bit(uint64_t value, size_t bit) NOEXCEPT
{
    const auto hi = unmask_right<uint64_t>(bit);
    const auto lo = mask_right<uint64_t>(add1(bit));
    return bit_or(shift_right(bit_and(value, lo)), bit_and(value, hi));
}

constexpr bool calculate_next(uint64_t& out, uint64_t node,
    uint64_t delete_node, uint8_t forest_rows) NOEXCEPT
{
    const auto node_row = detect_row(node, forest_rows);
    const auto delete_row = detect_row(delete_node, forest_rows);
    if (is_subtract_overflow(delete_row, node_row))
        return false;

    const auto bit = subtract(delete_row, node_row);
    const auto lo = remove_bit(node, bit);

    const auto row = add1(node_row);
    const auto hi = shift_left(bit_right<uint64_t>(row),
        subtract(forest_rows, row));

    out = bit_or(hi, lo);
    return true;
}

constexpr uint64_t start_position_at_row(uint8_t row,
    uint8_t forest_rows) NOEXCEPT
{
    // Second subtraction cannot overflow if this one does not.
    BC_ASSERT(!is_subtract_overflow(forest_rows, row));

    return subtract(
        bit_right<uint64_t>(add1<size_t>(forest_rows)),
        bit_right<uint64_t>(add1<size_t>(subtract(forest_rows, row))));
}

constexpr size_t number_of_roots(uint64_t leaves) NOEXCEPT
{
    return std::popcount(leaves);
}

constexpr uint8_t tree_rows(uint64_t leaves) NOEXCEPT
{
    return narrow_cast<uint8_t>(is_zero(leaves) ? zero :
        subtract(bits<uint64_t>, left_zeros<uint64_t>(sub1(leaves))));
}

inline positions detwin(const positions& nodes, uint8_t forest_rows) NOEXCEPT
{
    if (nodes.empty())
        return {};

    auto out{ nodes };
    for (auto index{ one }; index < out.size(); ++index)
    {
        const auto node = out.at(sub1(index));
        const auto next = out.at(index);

        if (is_right_sibling(node, next))
        {
            const auto dad = parent(node, forest_rows);
            const auto from = std::next(out.begin(), sub1(index));
            const auto stop = std::next(out.begin(), add1(index));
            out.erase(from, stop);
            out.push_back(dad);
            sort(out);
            --index;
        }
    }

    return out;
}

// TODO: test.
// TODO: guard overflows.
inline bool roots_to_destroy(positions& out, std::vector<node_hash>&& roots,
    uint64_t adding, uint64_t leaves) NOEXCEPT
{
    uint8_t row{};

    for (auto leaf{ adding }; !is_zero(leaf); --leaf, ++leaves)
    {
        while (get_right(leaves, add1(row)))
        {
            if (roots.empty())
                return false;

            if (roots.back() == empty_hash)
            {
                const auto rows = tree_rows(add(leaves, leaf));
                out.push_back(root_position(leaves, row, rows));
            }

            roots.pop_back();
            ++row;
        }

        roots.push_back(dummy_hash);
    }

    return true;
}

// TODO: test.
// TODO: guard overflows.
constexpr std::tuple<uint8_t, uint8_t, uint64_t> detect_offset(uint64_t node,
    uint64_t leaves) NOEXCEPT
{
    uint8_t trees{};
    auto rows = tree_rows(leaves);
    const auto row = detect_row(node, rows);

    while (true)
    {
        const auto mask = unmask_right<uint64_t>(rows);
        const auto size = bit_and(set_right<uint64_t>(rows), leaves);
        if (bit_and(shift_left(node, row), mask) < size)
            break;

        if (!is_zero(size))
        {
            node -= size;
            ++trees;
        }

        --rows;
    };

    return { trees, subtract(rows, row), !node };
}

// TODO: test.
constexpr bool parent_many(uint64_t& out, uint64_t node, uint8_t rise,
    uint8_t forest_rows) NOEXCEPT
{
    if (is_zero(rise))
    {
        out = node;
        return true;
    }

    if (rise > forest_rows)
        return false;

    const auto left = subtract(forest_rows, sub1(rise));
    const auto mask = sub1(bit_right<uint64_t>(add1<size_t>(forest_rows)));
    out = bit_and(bit_or(shift_right(node, rise), shift_left(mask, left)), mask);
    return true;
}

// TODO: test.
constexpr bool max_position_at_row(uint64_t& out, uint8_t row, uint8_t rows,
    uint64_t leaves) NOEXCEPT
{
    uint64_t many{};
    if (!parent_many(many, leaves, row, rows))
        return false;

    out = floored_subtract(many, one);
    return true;
}

// TODO: test.
constexpr bool is_ancestor(uint64_t higer, uint64_t lower,
    uint8_t forest_rows) NOEXCEPT
{
    if (higer == lower)
        return false;

    uint64_t ancestor{};
    const auto lo = detect_row(lower, forest_rows);
    const auto hi = detect_row(higer, forest_rows);
    return !is_subtract_overflow(hi, lo) &&
        parent_many(ancestor, lower, subtract(hi, lo), forest_rows) &&
        (ancestor == higer);
}

inline positions get_proof_positions(positions&& targets, uint64_t leaves,
    uint8_t forest_rows) NOEXCEPT
{
    sort(targets);
    positions proof{};

    for (uint8_t row{}; row < forest_rows; ++row)
    {
        auto sorted{ true };
        const auto rows{ targets };

        for (auto it = rows.begin(); it != rows.end(); ++it)
        {
            const auto node = *it;
            if ((detect_row(node, forest_rows) != row) ||
                is_root_position(node, leaves, forest_rows))
                continue;
        
            const auto next = std::next(it);
            if (next != rows.end() && is_sibling(node, *next))
                ++it;
            else
                proof.push_back(bit_xor<uint64_t>(node, one));
 
            targets.push_back(parent(node, forest_rows));
            sorted = false;
        }

        if (!sorted)
            sort(targets);
    }

    return proof;
}

} // namespace utreexo
} // namespace system
} // namespace libbitcoin

#endif
