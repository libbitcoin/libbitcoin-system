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
#include <bitcoin/system/wallet/addresses/png.hpp>

#include <bitcoin/system/compression/compression.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

enum png_signature : uint64_t
{
    // "\x89PNG\r\n\x1a\n" detects transmission (line ending) corruption.
    file_signature = 0x89504e470d0a1a0a
};

enum chunk_type : uint32_t
{
    // "IHDR", "IDAT", "IEND".
    image_header = 0x49484452,
    image_data = 0x49444154,
    image_end = 0x49454e44
};

enum bit_depth : uint8_t
{
    one_bit = 1
};

enum color_type : uint8_t
{
    // Zero is black.
    grayscale = 0
};

enum compression_method : uint8_t
{
    deflated = 0
};

enum filter_method : uint8_t
{
    adaptive = 0
};

enum interlace_method : uint8_t
{
    no_interlace = 0
};

enum filter_type : uint8_t
{
    unfiltered = 0
};

// Chunk length is limited to 2^31-1.
constexpr size_t max_chunk_bytes = to_half(max_uint32);

constexpr size_t image_limit = 0x3ffe0000;
size_t png::max_image_bytes = image_limit;

// Chunk is the type followed by the data, both of which the crc covers.
static void write_chunk(write::bytes::ostream& writer,
    const data_chunk& chunk) NOEXCEPT
{
    const auto length = chunk.size() - sizeof(chunk_type);
    const auto checksum = crc32(chunk);

    writer.write_4_bytes_big_endian(possible_narrow_cast<uint32_t>(length));
    writer.write_bytes(chunk);
    writer.write_4_bytes_big_endian(checksum);
}

bool png::to_image(std::ostream& out, const data_chunk& data,
    uint16_t width) NOEXCEPT
{
    // Empty image is not valid PNG.
    if (is_zero(width) || data.empty())
        return false;

    // PNG chunk length limit is max_chunk_bytes.
    // This method is otherwise limited only by the stream capacity.
    if (data.size() > max_image_bytes)
        return false;

    // The width > 0 and therefore row_bytes > 0.
    const auto row_bytes = ceilinged_divide<size_t>(width, byte_bits);

    // Each row is the same width, so there must be no remainder.
    const auto size = data.size();
    if (!is_zero(size % row_bytes))
        return false;

    const auto rows = size / row_bytes;
    const auto height = possible_narrow_cast<uint32_t>(rows);
    const auto raw_size = size + rows;

    // =============== IHDR ===============

    data_chunk header{};
    write::bytes::data header_writer(header);
    header_writer.write_4_bytes_big_endian(chunk_type::image_header);
    header_writer.write_4_bytes_big_endian(width);
    header_writer.write_4_bytes_big_endian(height);
    header_writer.write_byte(bit_depth::one_bit);
    header_writer.write_byte(color_type::grayscale);
    header_writer.write_byte(compression_method::deflated);
    header_writer.write_byte(filter_method::adaptive);
    header_writer.write_byte(interlace_method::no_interlace);
    header_writer.flush();

    // ============ Scanlines =============

    // Each row is prefixed by its filter type and inverted (zero is black).
    data_chunk raw{};
    raw.reserve(raw_size);
    read::bytes::copy image_reader(data);
    write::bytes::data raw_writer(raw);
    for (size_t row{}; row < rows; ++row)
    {
        raw_writer.write_byte(filter_type::unfiltered);
        for (size_t column{}; column < row_bytes; ++column)
            raw_writer.write_byte(bit_not(image_reader.read_byte()));
    }

    raw_writer.flush();

    // =============== IDAT ===============

    const auto stream = zlib_deflate(raw);
    if (stream.size() > max_chunk_bytes)
        return false;

    data_chunk image{};
    image.reserve(sizeof(chunk_type) + stream.size());
    write::bytes::data image_writer(image);
    image_writer.write_4_bytes_big_endian(chunk_type::image_data);
    image_writer.write_bytes(stream);
    image_writer.flush();

    // =============== IEND ===============

    data_chunk end{};
    write::bytes::data end_writer(end);
    end_writer.write_4_bytes_big_endian(chunk_type::image_end);
    end_writer.flush();

    // ====================================

    write::bytes::ostream writer(out);
    writer.write_8_bytes_big_endian(png_signature::file_signature);
    write_chunk(writer, header);
    write_chunk(writer, image);
    write_chunk(writer, end);
    writer.flush();
    return true;
}

} // namespace system
} // namespace libbitcoin
