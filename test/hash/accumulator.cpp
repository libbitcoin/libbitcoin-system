/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
    
BOOST_AUTO_TEST_SUITE(accumulator_tests)

BOOST_AUTO_TEST_SUITE(accumulator_engine_tests)

template <typename Algorithm, bool Checked = true>
struct accessor
  : accumulator<Algorithm, Checked>
{
    using base = accumulator<Algorithm, Checked>;
    using base::base;

    constexpr size_t next_() const NOEXCEPT
    {
        return base::next();
    }

    constexpr size_t gap_() const NOEXCEPT
    {
        return base::gap();
    }

    constexpr size_t is_full_() const NOEXCEPT
    {
        return base::is_full();
    }

    constexpr size_t is_empty_() const NOEXCEPT
    {
        return base::is_empty();
    }

    constexpr bool is_buffer_overflow_(size_t bytes) const NOEXCEPT
    {
        return base::is_buffer_overflow(bytes);
    }

    constexpr size_t add_data_(size_t bytes,
        const typename base::byte_t* data) NOEXCEPT
    {
        return base::add_data(bytes, data);
    }

    size_t accumulate_(size_t bytes,
        const typename base::byte_t* data) NOEXCEPT
    {
        return base::accumulate(bytes, data);
    }

    typename base::state_t flush_state_() NOEXCEPT
    {
        return base::flush_state();
    }

    static constexpr typename base::counter serialize_(size_t bytes) NOEXCEPT
    {
        return base::serialize(bytes);
    }

    constexpr size_t pad_size_() const NOEXCEPT
    {
        return base::pad_size();
    }

    static CONSTEVAL typename base::block_t stream_pad_() NOEXCEPT
    {
        return base::stream_pad();
    }
};

using checked = accessor<sha512, true>;
using unchecked = accessor<sha512, false>;
constexpr auto block_size = array_count<sha512::block_t>;
constexpr auto count_size = sha512::count_bytes;

BOOST_AUTO_TEST_CASE(accumulator__construct__default__initial)
{
    constexpr checked writer{};
    BOOST_REQUIRE(writer.is_full_());
    BOOST_REQUIRE(writer.is_empty_());
    BOOST_REQUIRE_EQUAL(writer.next_(), zero);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - count_size);

    // state is the word_t/non-endianed digest of hashing an empty block.
    ////BOOST_REQUIRE_EQUAL(writer.flush_state_(), state);
}

BOOST_AUTO_TEST_CASE(accumulator__construct__sized__expected)
{
    constexpr auto blocks = 2u;
    constexpr sha512::state_t state{ 42 };
    constexpr checked writer{ state, blocks };
    static_assert(writer.is_full_());
    static_assert(writer.is_empty_());
    static_assert(is_zero(writer.next_()));
    static_assert(writer.gap_() == block_size);
    static_assert(writer.pad_size_() == block_size - count_size);
    BOOST_REQUIRE(writer.is_full_());
    BOOST_REQUIRE(writer.is_empty_());
    BOOST_REQUIRE_EQUAL(writer.next_(), zero);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - count_size);

    ////constexpr auto expected_state = sha512::hash(sha512::block_t{ 0 });
    ////BOOST_REQUIRE_EQUAL(writer.flush_state_(), state);
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___checked__expected)
{
    // Test expectation limited to sha256.
    using access = accessor<sha256>;
    constexpr auto overflow = is_same_size<size_t, uint64_t>;
    constexpr access writer{};
    static_assert(!writer.is_buffer_overflow_(zero));
    static_assert(writer.is_buffer_overflow_(max_size_t) == overflow);
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE_EQUAL(writer.is_buffer_overflow_(max_size_t), overflow);
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___unchecked__false)
{
    constexpr unchecked writer{};
    static_assert(!writer.is_buffer_overflow_(zero));
    static_assert(!writer.is_buffer_overflow_(max_size_t));
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE(!writer.is_buffer_overflow_(max_size_t));
}

BOOST_AUTO_TEST_CASE(accumulator__stream_pad__always__expected)
{
    constexpr auto expected = sha512::block_t{ bit_hi<sha512::byte_t> };
    static_assert(checked::stream_pad_() == expected);
    constexpr checked writer{};
    static_assert(writer.stream_pad_() == expected);
    BOOST_REQUIRE_EQUAL(writer.stream_pad_(), expected);
}

BOOST_AUTO_TEST_CASE(accumulator__serialize__default__initial)
{
    // Test expectation limited to sha256.
    using access = accessor<sha256>;
    constexpr auto count = 42u;
    constexpr auto count_bits = to_bits<uint16_t>(count);
    constexpr auto count_lo = lo_word<sha256::byte_t>(count_bits);
    constexpr auto count_hi = hi_word<sha256::byte_t>(count_bits);
    using counter_t = std_array<sha256::byte_t, sha256::count_bytes>;
    constexpr auto expected = counter_t{ 0, 0, 0, 0, 0, 0, count_hi, count_lo };
    static_assert(access::serialize_(count) == expected);
    BOOST_REQUIRE_EQUAL(access::serialize_(count), expected);
}

// write (captures test of add_data1/2, accumulate, pad, flush, double_flush)
// ----------------------------------------------------------------------------
// write is not constexpr due to unsafe_array_cast, which ensures algorithm
// remains fully constexpr (by not accepting byte*).

BOOST_AUTO_TEST_CASE(accumulator__write__zero__true)
{
    constexpr data_array<0> data{};
    checked writer{};
    BOOST_REQUIRE(writer.write(data));
}

BOOST_AUTO_TEST_CASE(accumulator__write_flush__nonzero__expected)
{
    // Test expectation limited to sha256.
    using access = accessor<sha256>;
    constexpr auto size = array_count<sha256::block_t>;
    constexpr auto expected = base16_array("729e145a50396134c294aaf8e2daea0b7c89bb617cd58379ba4c2abdec2d1da7");
    constexpr auto bytes = 42u;
    constexpr auto blocks = 2u;
    constexpr data_array<bytes> data{};
    constexpr sha256::state_t state{};
    access writer{ state, blocks };
    BOOST_REQUIRE(writer.write(data));
    BOOST_REQUIRE_EQUAL(writer.next_(), bytes);
    BOOST_REQUIRE_EQUAL(writer.gap_(), size - bytes);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), size - bytes - sha256::count_bytes);
    BOOST_REQUIRE_EQUAL(writer.flush(), expected);
}

BOOST_AUTO_TEST_CASE(accumulator__write_double_flush__nonzero__expected)
{
    // Test expectation limited to sha256.
    using access = accessor<sha256>;
    constexpr auto expected = base16_array("19b7f210d600599fe35dc4516dac5acb82bb4040f8de6f54e889455fd0b9b106");
    constexpr auto bytes = 42u;
    constexpr auto blocks = 2u;
    constexpr data_array<bytes> data{};
    constexpr sha256::state_t state{};
    access writer{ state, blocks };
    BOOST_REQUIRE(writer.write(data));
    BOOST_REQUIRE_EQUAL(writer.double_flush(), expected);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
