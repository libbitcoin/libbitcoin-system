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

BC_AUTO_TEST_SUITE(gzip_tests)

// gzip members produced by zlib (python gzip), header fields as named.
const auto hello = to_chunk(std::string{ "hello world" });
const auto hello_gzip = base16_chunk("1f8b08000000000002ffcb48cdc9c95728cf2fca49010085114a0d0b000000");
const auto hello_named = base16_chunk("1f8b08080000000002ff6e616d652e74787400cb48cdc9c95728cf2fca49010085114a0d0b000000");
const auto hello_header_crc = base16_chunk("1f8b08020000000000ff90c9cb48cdc9c95728cf2fca49010085114a0d0b000000");
const auto hello_extra_comment = base16_chunk("1f8b08140000000000ff03006162636e6f746500cb48cdc9c95728cf2fca49010085114a0d0b000000");
const auto bang_gzip = base16_chunk("1f8b08000000000002ff530400d3ff6b9e01000000");


// The output of a decoder fed one byte at a time, false if invalid or
// incomplete.
template <typename Decoder>
static bool fed(data_chunk& out, const data_chunk& data)
{
    Decoder decoder{};
    for (const auto byte: data)
    {
        size_t consumed{};
        if (!decoder.write(out, consumed, data_chunk{ byte }))
            return false;
    }

    return decoder.done();
}

// The stream of data written to an encoder in parts of step bytes.
template <typename Encoder>
static data_chunk written(const data_chunk& data, size_t step)
{
    data_chunk out{};
    Encoder encoder{};
    for (size_t offset{}; offset < data.size(); offset += step)
    {
        const auto size = std::min(step, data.size() - offset);
        const auto begin = std::next(data.begin(), offset);
        encoder.write(out, data_slice(begin, std::next(begin, size)));
    }

    encoder.flush(out);
    return out;
}

BC_AUTO_TEST_CASE(gzip_inflater__bytewise_members__expected)
{
    data_chunk out{};
    BC_REQUIRE(fed<gzip_inflater>(out, splice(hello_gzip, bang_gzip)));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "hello world!" }));
}

BC_AUTO_TEST_CASE(gzip_inflater__bytewise_header_crc__expected)
{
    data_chunk out{};
    BC_REQUIRE(fed<gzip_inflater>(out, hello_header_crc));
    BC_REQUIRE_EQUAL(out, hello);
}

BC_AUTO_TEST_CASE(gzip_inflater__bytewise_partial_member__incomplete)
{
    data_chunk out{};
    BC_REQUIRE(!fed<gzip_inflater>(out, splice(hello_gzip, base16_chunk("1f8b"))));
}

BC_AUTO_TEST_CASE(gzip_deflater__written_parts__gzip_deflate)
{
    const data_chunk data(70000, 'g');
    BC_REQUIRE_EQUAL(written<gzip_deflater>(data, 4096), gzip_deflate(data));
}

// Bytes of noise, repetition and noise, coded in stored and dynamic blocks.
static data_chunk mixed(uint32_t seed)
{
    data_chunk out(120000, 'x');
    for (size_t index{}; index < 40000; ++index)
    {
        seed = seed * 1103515245_u32 + 12345_u32;
        out[index] = narrow_cast<uint8_t>(shift_right(seed, 16));
        out[index + 80000] = narrow_cast<uint8_t>(shift_right(seed, 8));
    }

    return out;
}

BC_AUTO_TEST_CASE(gzip_inflate__mixed_members__expected)
{
    const auto first = mixed(1);
    const auto second = mixed(2);
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, splice(gzip_deflate(first), gzip_deflate(second))));
    BC_REQUIRE_EQUAL(out, splice(first, second));
}

BC_AUTO_TEST_CASE(gzip_deflate__empty__expected)
{
    BC_REQUIRE_EQUAL(gzip_deflate(data_chunk{}), base16_chunk("1f8b08000000000000ff03000000000000000000"));
}

BC_AUTO_TEST_CASE(gzip_inflate__member__expected)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, hello_gzip));
    BC_REQUIRE_EQUAL(out, hello);
}

BC_AUTO_TEST_CASE(gzip_inflate__file_name__expected)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, hello_named));
    BC_REQUIRE_EQUAL(out, hello);
}

BC_AUTO_TEST_CASE(gzip_inflate__header_crc__expected)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, hello_header_crc));
    BC_REQUIRE_EQUAL(out, hello);
}

BC_AUTO_TEST_CASE(gzip_inflate__extra_and_comment__expected)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, hello_extra_comment));
    BC_REQUIRE_EQUAL(out, hello);
}

BC_AUTO_TEST_CASE(gzip_inflate__members__concatenated)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, splice(hello_gzip, bang_gzip)));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "hello world!" }));
}

BC_AUTO_TEST_CASE(gzip_inflate__round_trip__expected)
{
    const data_chunk data(70000, 'g');
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, gzip_deflate(data)));
    BC_REQUIRE_EQUAL(out, data);
}

BC_AUTO_TEST_CASE(gzip_inflate__empty_input__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, data_chunk{}));
}

BC_AUTO_TEST_CASE(gzip_inflate__bad_id__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8c08000000000000ff03000000000000000000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__reserved_flag__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08200000000000ff03000000000000000000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__bad_header_crc__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08020000000000ff91c9cb48cdc9c95728cf2fca49010085114a0d0b000000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__unterminated_name__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08080000000002ff6e616d65")));
}

BC_AUTO_TEST_CASE(gzip_inflate__bad_crc__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08000000000002ffcb48cdc9c95728cf2fca49010085114a0e0b000000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__bad_size__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08000000000002ffcb48cdc9c95728cf2fca49010085114a0d0c000000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__short_trailer__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, base16_chunk("1f8b08000000000002ffcb48cdc9c95728cf2fca49010085114a0d0b0000")));
}

BC_AUTO_TEST_CASE(gzip_inflate__trailing_garbage__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, splice(hello_gzip, base16_chunk("00"))));
}

BC_AUTO_TEST_CASE(gzip_inflate__limit_equal__true)
{
    data_chunk out{};
    BC_REQUIRE(gzip_inflate(out, splice(hello_gzip, bang_gzip), 12u));
    BC_REQUIRE_EQUAL(out.size(), 12u);
}

BC_AUTO_TEST_CASE(gzip_inflate__limit_exceeded_across_members__false)
{
    data_chunk out{};
    BC_REQUIRE(!gzip_inflate(out, splice(hello_gzip, bang_gzip), 11u));
}

BC_AUTO_TEST_SUITE_END()
