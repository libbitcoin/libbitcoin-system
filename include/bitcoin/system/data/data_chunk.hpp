/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_CHUNK_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_CHUNK_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

// C++ now allows only vectors of "any non-const object type".
// stackoverflow.com/questions/6954906/does-c11-allow-vectorconst-t
// Since const std::vector<T> implies all elements are const, this is ok.
// The following naming conventions are applied to pointers and vectors.
//
// T::ptr implies shared_ptr<T>.
// name is an arbitrary alias for T.
//
// [name]_ptr implies ptr<T>.
// [name]_cptr implies a ptr<const T>.
//
// [name]s_ptr implies a ptr<collection<T>>.
// [name]s_cptr implies a ptr<const collection<T>>.
//
// [name]s implies collection<T>.
// [name]_ptrs implies collection<[name]_ptr>.
// [name]_cptrs implies a collection<[name]_cptr>.
//
// [name]_ptrs_ptr implies a ptr<[name]s_ptr>.
// [name]_cptrs_ptr implies a ptr<[name]s_cptr>.
// [name]_cptrs_cptr implies a ptr<const [name]s_cptr>.

namespace libbitcoin {
namespace system {

/// Define data_chunk types.

typedef std::vector<uint8_t> data_chunk;

typedef std::shared_ptr<data_chunk> chunk_ptr;
typedef std::shared_ptr<const data_chunk> chunk_cptr;

typedef std::vector<chunk_ptr> chunk_ptrs;
typedef std::vector<chunk_cptr> chunk_cptrs;

typedef std::shared_ptr<chunk_ptrs> chunk_ptrs_ptr;
typedef std::shared_ptr<const chunk_ptrs> chunk_ptrs_cptr;

typedef std::shared_ptr<chunk_cptrs> chunk_cptrs_ptr;
typedef std::shared_ptr<const chunk_cptrs> chunk_cptrs_cptr;

/// Define data_stack types.

typedef std::vector<data_chunk> data_stack;

typedef std::shared_ptr<data_stack> stack_ptr;
typedef std::shared_ptr<const data_stack> stack_cptr;

/// Create a single byte data_chunk with given element value.
BC_API data_chunk to_chunk(uint8_t byte) noexcept;

/// Create a data chunk from data slice.
/// to_chunk(to_string(data)) == data.
BC_API data_chunk to_chunk(const data_slice& bytes) noexcept;

/// Concatenate several data slices into a single data_chunk.
/// extra_reserve reserves but does not allocate additional bytes.
BC_API data_chunk build_chunk(const data_loaf& slices,
    size_t extra_reserve=0) noexcept;

/// Concatenate two data_slices into a single data chunk.
BC_API data_chunk splice(const data_slice& left,
    const data_slice& right) noexcept;

} // namespace system
} // namespace libbitcoin

#endif
