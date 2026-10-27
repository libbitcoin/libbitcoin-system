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

#include <boost/crc.hpp>
#include <bitcoin/system/data/data.hpp>
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
    deflate = 0
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

enum zlib_header : uint16_t
{
    // Deflate, 32k window, fastest (stored), check bits.
    stored_deflate = 0x7801
};

enum block_header : uint8_t
{
    // BFINAL bit and BTYPE stored (00), padded to a byte boundary.
    intermediate_block = 0x00,
    final_block = 0x01
};

// Stored block length and its ones complement follow the block header.
constexpr size_t max_block_bytes = max_uint16;
constexpr auto block_overhead = sizeof(block_header) + sizeof(uint16_t) +
    sizeof(uint16_t);

// Adler-32 modulus, the largest prime less than 2^16.
constexpr uint32_t adler_modulus = 65521;

// Size of the zlib stream of stored blocks for the given filtered data size.
constexpr size_t stream_size(size_t raw) NOEXCEPT
{
    const auto blocks = ceilinged_divide(raw, max_block_bytes);
    return sizeof(zlib_header) + raw + blocks * block_overhead +
        sizeof(uint32_t);
}

// Chunk length is limited to 2^31-1.
constexpr size_t max_chunk_bytes = to_half(max_uint32);

// Rows of one byte double the image size with filter bytes.
constexpr size_t image_limit = 0x3ffe0000;
static_assert(stream_size(two * image_limit) <= max_chunk_bytes,
    "PNG image data must fit in one chunk.");

size_t png::max_image_bytes = image_limit;

static uint32_t adler32(const data_chunk& data) NOEXCEPT
{
    uint32_t low{ 1 };
    uint32_t high{ 0 };
    for (const auto byte: data)
    {
        low = (low + byte) % adler_modulus;
        high = (high + low) % adler_modulus;
    }

    return bit_or(shift_left(high, bits<uint16_t>), low);
}

// Chunk is the type followed by the data, both of which the crc covers.
static void write_chunk(write::bytes::ostream& writer,
    const data_chunk& chunk) NOEXCEPT
{
    const auto length = chunk.size() - sizeof(chunk_type);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    boost::crc_32_type crc{};
    crc.process_bytes(chunk.data(), chunk.size());
    const uint32_t checksum = crc.checksum();
    BC_POP_WARNING()

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

    const auto size = data.size();

    // The width > 0 and therefore row_bytes > 0.
    const auto row_bytes = ceilinged_divide(width, byte_bits);

    // Each row is the same width, so there must be no remainder.
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
    header_writer.write_byte(compression_method::deflate);
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

    data_chunk image{};
    image.reserve(sizeof(chunk_type) + stream_size(raw_size));
    read::bytes::copy raw_reader(raw);
    write::bytes::data image_writer(image);
    image_writer.write_4_bytes_big_endian(chunk_type::image_data);
    image_writer.write_2_bytes_big_endian(zlib_header::stored_deflate);

    // Stored (uncompressed) deflate blocks.
    for (auto remaining = raw_size; !is_zero(remaining);)
    {
        const auto length = std::min(remaining, max_block_bytes);
        const auto block = narrow_cast<uint16_t>(length);
        remaining -= length;

        const auto last = is_zero(remaining);
        image_writer.write_byte(last ? final_block : intermediate_block);
        image_writer.write_2_bytes_little_endian(block);
        image_writer.write_2_bytes_little_endian(bit_not(block));
        image_writer.write_bytes(raw_reader, length);
    }

    image_writer.write_4_bytes_big_endian(adler32(raw));
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
