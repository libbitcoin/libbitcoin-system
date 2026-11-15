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
#ifndef LIBBITCOIN_SYSTEM_COMPRESSION_DEFLATE_HPP
#define LIBBITCOIN_SYSTEM_COMPRESSION_DEFLATE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Not thread safe.
/// Raw deflate (rfc1951) encoder of a stream written in parts, each block
/// stored, fixed or dynamic Huffman, whichever is smallest. Memory is bounded
/// by the window and one block, independent of the stream size.
class BC_API deflater
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(deflater);

    deflater() NOEXCEPT;

    /// Encode data, appending completed output to out.
    void write(data_chunk& out, const data_slice& data) NOEXCEPT;

    /// Encode the remainder and end the stream, appending to out, after
    /// which another stream may be written.
    void flush(data_chunk& out) NOEXCEPT;

private:
    void insert(size_t position) NOEXCEPT;
    void slide() NOEXCEPT;
    void tokenize(data_chunk& out, bool final) NOEXCEPT;
    void add(data_chunk& out, uint32_t symbol, size_t size) NOEXCEPT;
    void emit(data_chunk& out, bool last) NOEXCEPT;

    data_chunk window_{};
    std_vector<uint32_t> heads_;
    std_vector<uint32_t> chains_;
    std_vector<uint32_t> symbols_{};
    data_chunk raw_{};
    size_t position_{};
    size_t buffer_{};
    size_t bit_count_{};
};

/// Raw deflate (rfc1951) encode of the entire input.
BC_API data_chunk deflate(const data_slice& in) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
