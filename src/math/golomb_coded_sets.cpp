/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/math/golomb_coded_sets.hpp>

namespace libbitcoin {
namespace system {
namespace gcs {

data_chunk construct(data_stack items, uint64_t bit_param, half_hash entropy,
	uint64_t target_false_positive_rate)
{
	return data_chunk{};
}

bool match(half_hash entropy, data_chunk compressed_set, uint64_t set_size,
	uint64_t bit_param, uint64_t target_false_positive_rate)
{
	return false;
}


//hash_to_range(item: []byte, F: uint64, k: [16]byte) -> uint64:
//    return (siphash(k, item) * F) >> 64

//hashed_set_construct(raw_items: [][]byte, k: [16]byte, M: uint) -> []uint64:
//    let N = len(raw_items)
//    let F = N * M
//
//    let set_items = []
//
//    for item in raw_items:
//        let set_value = hash_to_range(item, F, k)
//        set_items.append(set_value)
//
//    return set_items

//golomb_encode(stream, x: uint64, P: uint):
//	let q = x >> P
//
//	while q > 0:
//		write_bit(stream, 1)
//		q--
//	write_bit(stream, 0)
//
//	write_bits_big_endian(stream, x, P)
//
//golomb_decode(stream, P: uint) -> uint64:
//	let q = 0
//	while read_bit(stream) == 1:
//		q++
//
//	let r = read_bits_big_endian(stream, P)
//
//	let x = (q << P) + r
//	return x

} // namespace gcs
} // namespace system
} // namespace libbitcoin
