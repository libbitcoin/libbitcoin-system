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
#include <utility>
#include <bitcoin/system.hpp>

namespace libbitcoin {
namespace system {
namespace utreexo {

using node_hash = hash_digest;
using positions = std::vector<uint64_t>;
constexpr auto empty_hash = node_hash{};

constexpr std::pair<bool, std::string> okay(bool result) NOEXCEPT
{
    return { result, (result ? "success" : "fail") };
}

constexpr node_hash parent_hash(const node_hash& left,
    const node_hash& right) NOEXCEPT
{
    return sha512_256::hash(left, right);
}

constexpr node_hash hash_from_u8(uint8_t byte) NOEXCEPT
{
    return sha256::hash(byte);
}

// TODO: test.
// return parent position of passed-in child
constexpr uint64_t parent(uint64_t position, uint8_t forest_rows) NOEXCEPT
{
    return set_right(shift_right(position), forest_rows);
}

constexpr uint64_t children(uint64_t position, uint8_t forest_rows) NOEXCEPT
{
    const auto mask = unmask_right<uint64_t>(add1<size_t>(forest_rows));
    return bit_and(shift_left(position), mask);
}

// TODO: test.
constexpr uint64_t left_child(uint64_t position, uint8_t forest_rows) NOEXCEPT
{
    return children(position, forest_rows);
}

// TODO: test.
constexpr uint64_t right_child(uint64_t position, uint8_t forest_rows) NOEXCEPT
{
    BC_ASSERT(!is_add_overflow<uint64_t>(children(position, forest_rows), one));

    return add1(children(position, forest_rows));
}

constexpr uint64_t left_sibling(uint64_t position) NOEXCEPT
{
    return set_right(position, zero, false);
}

constexpr bool is_left_niece(uint64_t position) NOEXCEPT
{
    return !get_right(position);
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

constexpr uint8_t detect_row(uint64_t position, uint8_t forest_rows) NOEXCEPT
{
    auto bit = forest_rows;
    while (!is_zero(bit) && get_right(position, bit)) --bit;
    return subtract(forest_rows, bit);
}

constexpr uint64_t root_position(uint64_t leaves, uint8_t row,
    uint8_t forest_rows) NOEXCEPT
{
    // TODO: undefined behavior if given row does not have a root.
    BC_ASSERT(!is_add_overflow<uint8_t>(row, 1));
    BC_ASSERT(!is_add_overflow<uint8_t>(forest_rows, 1));
    BC_ASSERT(!is_subtract_overflow<uint8_t>(add1(forest_rows), row));

    // sub1 cannot overflow here.
    const auto mask = unmask_right<uint64_t>(add1(forest_rows));
    const auto before = bit_and(leaves, shift_left(mask, add1(row)));
    const auto left = shift_left(mask, subtract(add1(forest_rows), row));
    const auto right = shift_right(before, row);
    const auto shifted = bit_or(left, right);
    return bit_and(shifted, mask);
}

constexpr bool is_root_position(uint64_t position, uint64_t leaves,
    uint8_t forest_rows) NOEXCEPT
{
    const auto row = detect_row(position, forest_rows);
    const auto present = get_right(leaves, row);
    const auto expected = root_position(leaves, row, forest_rows);
    return present && (position == expected);
}

constexpr uint64_t remove_bit(uint64_t value, size_t bit) NOEXCEPT
{
    const auto mask_lo = mask_right<uint64_t>(add1(bit));
    const auto mask_hi = unmask_right<uint64_t>(bit);
    return bit_or(shift_right(bit_and(value, mask_lo), 1),
        bit_and(value, mask_hi));
}

constexpr bool calculate_next(uint64_t& out, uint64_t position,
    uint64_t delete_position, uint8_t forest_rows) NOEXCEPT
{
    const auto position_row = detect_row(position, forest_rows);
    const auto delete_row = detect_row(delete_position, forest_rows);
    if (delete_row < position_row)
        return false;

    const auto bit = subtract<uint64_t>(delete_row, position_row);
    const auto lo = remove_bit(position, bit);

    const auto row = add1(position_row);
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
    // nothing here can overflow.
    return narrow_cast<uint8_t>(is_zero(leaves) ? zero :
        subtract(bits<uint64_t>, left_zeros<uint64_t>(sub1(leaves))));
}

inline void detwin(positions& nodes, uint8_t forest_rows) NOEXCEPT
{
    if (nodes.empty())
        return;

    for (auto node = std::next(nodes.begin()); node != nodes.end();)
    {
        const auto prior = std::prev(node);

        if (is_right_sibling(*prior, *node))
        {
            const auto dad = parent(*prior, forest_rows);
            node = nodes.erase(prior, std::next(node));
            nodes.push_back(dad);
            sort(nodes);
            continue;
        }

        ++node;
    }
}

} // namespace utreexo
} // namespace system
} // namespace libbitcoin

#endif

////// roots_to_destroy returns the empty roots that get written over after num_adds
////// amount of leaves have been added.
////pub fn roots_to_destroy<Hash: AccumulatorHash>(
////    num_adds: u64,
////    mut num_leaves: u64,
////    orig_roots: &[Hash],
////) -> Vec<u64> {
////    let mut roots = orig_roots.to_vec();
////    let mut deleted = vec![];
////    let mut h = 0;
////    for add in 0..num_adds {
////        while (num_leaves >> h) & 1 == 1 {
////            let root = roots
////                .pop()
////                .expect("If (num_leaves >> h) & 1 == 1, it must have at least one root left");
////            if root.is_empty() {
////                let root_pos =
////                    root_position(num_leaves, h, tree_rows(num_leaves + (num_adds - add)));
////                deleted.push(root_pos);
////            }
////            h += 1;
////        }
////        // Just adding a non-zero value to the slice.
////        roots.push(AccumulatorHash::placeholder());
////        num_leaves += 1;
////    }
////
////    deleted
////}

////pub fn detect_offset(pos: u64, num_leaves: u64) -> (u8, u8, u64) {
////    let mut tr = tree_rows(num_leaves);
////    let nr = detect_row(pos, tr);
////
////    let mut bigger_trees: u8 = 0;
////    let mut marker = pos;
////
////    // add trees until you would exceed position of node
////
////    // This is a bit of an ugly predicate.  The goal is to detect if we've
////    // gone past the node we're looking for by inspecting progressively shorter
////    // trees; once we have, the loop is over.

////    // The predicate breaks down into 3 main terms:
////    // A: pos << nh
////    // B: mask
////    // C: 1<<th & num_leaves (tree_size)
////    // The predicate is then if (A&B >= C)
////    // A is position up-shifted by the row of the node we're targeting.
////    // B is the "mask" we use in other functions; a bunch of 0s at the MSB side
////    // and then a bunch of 1s on the LSB side, such that we can use bitwise AND
////    // to discard high bits.  Together, A&B is shifting position up by nr bits,
////    // and then discarding (zeroing out) the high bits.  This is the same as in
////    // n_grandchild.  C checks for whether a tree exists at the current tree
////    // rows.  If there is no tree at tr, C is 0.  If there is a tree, it will
////    // return a power of 2: the base size of that tree.
////    // The C term actually is used 3 times here, which is ugly; it's redefined
////    // right on the next line.
////    // In total, what this loop does is to take a node position, and
////    // see if it's in the next largest tree.  If not, then subtract everything
////    // covered by that tree from the position, and proceed to the next tree,
////    // skipping trees that don't exist.

////    while (marker << nr) & ((2 << tr) - 1) >= (1 << tr) & num_leaves {
////        let tree_size = (1 << tr) & num_leaves;
////        if tree_size != 0 {
////            marker -= tree_size;
////            bigger_trees += 1;
////        }
////        tr -= 1;
////    }
////
////    (bigger_trees, tr - nr, !marker)
////}

/////// max_position_at_row returns the biggest position an accumulator can have for the
/////// requested row for the given num_leaves.
////pub fn max_position_at_row(row: u8, total_rows: u8, num_leaves: u64) -> Result<u64, String> {
////    Ok(parent_many(num_leaves, row, total_rows)?.saturating_sub(1))
////}
////
////pub fn read_u64<Source: Read>(buf: &mut Source) -> Result<u64, String> {
////    let mut bytes = [0u8; 8];
////    buf.read_exact(&mut bytes)
////        .map_err(|_| "Failed to read u64")?;
////    Ok(u64::from_le_bytes(bytes))
////}

////pub fn parent_many(pos: u64, rise: u8, forest_rows: u8) -> Result<u64, String> {
////    if rise == 0 {
////        return Ok(pos);
////    }
////    if rise > forest_rows {
////        return Err(format!(
////            "Cannot rise more than the forestRows: rise: {} forest_rows: {}",
////            rise, forest_rows
////        ));
////    }
////
////    let mask = (2_u64 << forest_rows) - 1;
////    Ok((pos >> rise | (mask << (forest_rows - (rise - 1)) as u64)) & mask)
////}

////std::pair<bool, std::string> is_ancestor(uint64_t higher_pos,
////    uint64_t lower_pos, uint8_t forest_rows)
////{
////    if (higher_pos == lower_pos)
////        return okay(false);
////
////    auto lower_row = detect_row(lower_pos, forest_rows);
////    auto higher_row = detect_row(higher_pos, forest_rows);
////
////    // Prevent underflows by checking that the higherRow is not less than lowerRow.
////    if (higher_row < lower_row)
////        return okay(false);
////
////    // TODO: Return false if we error out or the calculated ancestor doesn't match the higherPos.
////    auto ancestor = parent_many(lower_pos, higher_row - lower_row, forest_rows);
////
////    return (higher_pos == ancestor);
////}

/////// Returns which node should have its hashes on the proof, along with all nodes
/////// whose hashes will be calculated to reach a root
////Vec<u64> get_proof_positions(targets: &[u64], num_leaves: u64, forest_rows: u8) NOEXCEPT
////{
////    let mut proof_positions = vec![];
////    let mut computed_positions = targets.to_vec();
////    computed_positions.sort();
////
////    for row in 0..=forest_rows {
////        let mut row_targets = computed_positions
////            .iter()
////            .cloned()
////            .filter(|x| super::util::detect_row(*x, forest_rows) == row)
////            .collect::<Vec<_>>()
////            .into_iter()
////            .peekable();
////
////        while let Some(node) = row_targets.next() {
////            if is_root_position(node, num_leaves, forest_rows) {
////                continue;
////            }
////
////            if let Some(next) = row_targets.peek() {
////                if !is_sibling(node, *next) {
////                    proof_positions.push(node ^ 1);
////                } else {
////                    row_targets.next();
////                }
////            } else {
////                proof_positions.push(node ^ 1);
////            }
////
////            computed_positions.push(parent(node, forest_rows));
////        }
////
////        computed_positions.sort();
////    }
////
////    proof_positions
////}
