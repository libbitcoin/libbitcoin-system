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
#include "../test.hpp"
    
BOOST_AUTO_TEST_SUITE(accumulator_tests)

BOOST_AUTO_TEST_SUITE(accumulator_engine_tests)

using algorithm = sha256;
////using algorithm = sha512;
////using algorithm = sha160;

template <bool Checked>
struct accessor
  : accumulator<algorithm, Checked>
{
    using base = accumulator<algorithm, Checked>;
    using base::base;

    constexpr size_t next_() const NOEXCEPT
    {
        return base::next();
    }

    constexpr size_t gap_() const NOEXCEPT
    {
        return base::gap();
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

    constexpr void increment_(size_t blocks) NOEXCEPT
    {
        return base::increment(blocks);
    }

    constexpr size_t pad_size_() const NOEXCEPT
    {
        return base::pad_size();
    }

    static RCONSTEXPR typename base::counter serialize_(size_t bytes) NOEXCEPT
    {
        return base::serialize(bytes);
    }

    static CONSTEVAL typename base::block_t stream_pad_() NOEXCEPT
    {
        return base::stream_pad();
    }
};

using checked = accessor<true>;
using unchecked = accessor<false>;
constexpr auto block_size = array_count<algorithm::block_t>;
constexpr auto count_size = algorithm::count_bytes;

// stream_pad
constexpr auto expected_pad = algorithm::block_t{ bit_hi<algorithm::byte_t> };
static_assert(checked::stream_pad_() == expected_pad);

// construct/next/gap/pad_size
BOOST_AUTO_TEST_CASE(accumulator__construct__default__initial)
{
    const checked writer{};
    BOOST_REQUIRE_EQUAL(writer.next_(), zero);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - count_size);
}

BOOST_AUTO_TEST_CASE(accumulator__construct__sized__expected)
{
    constexpr auto blocks = 2u;
    constexpr algorithm::state_t state{};

    const checked writer{ state, blocks };
    BOOST_REQUIRE_EQUAL(writer.next_(), zero);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - count_size);
}

// serialize conditionally constexpr (reverse views).
BOOST_AUTO_TEST_CASE(accumulator__serialize__default__initial)
{
    constexpr auto count = 42u;
    constexpr auto count_bits = to_bits<uint16_t>(count);
    constexpr auto count_lo = lo_word<algorithm::byte_t>(count_bits);
    constexpr auto count_hi = hi_word<algorithm::byte_t>(count_bits);
    using counter_t = std_array<algorithm::byte_t, algorithm::count_bytes>;
    constexpr auto expected_counter = counter_t{ 0, 0, 0, 0, 0, 0, count_hi, count_lo };
    BOOST_REQUIRE_EQUAL(checked::serialize_(count), expected_counter);
}

BOOST_AUTO_TEST_CASE(accumulator__write__zero__true)
{
    constexpr data_array<0> data{};
    checked writer{};
    BOOST_REQUIRE(writer.write(data));
}

BOOST_AUTO_TEST_CASE(accumulator__write__nonzero__expected)
{
    // Test expectation limited to sha256.
    constexpr auto expected = base16_array("729e145a50396134c294aaf8e2daea0b7c89bb617cd58379ba4c2abdec2d1da7");
    constexpr auto bytes = 42u;
    constexpr auto blocks = 2u;
    constexpr data_array<bytes> data{};
    constexpr algorithm::state_t state{};

    checked writer{ state, blocks };
    BOOST_REQUIRE(writer.write(data));
    BOOST_REQUIRE_EQUAL(writer.next_(), bytes);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size - bytes);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - bytes - algorithm::count_bytes);
    BOOST_REQUIRE_EQUAL(writer.flush(), expected);
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___checked__expected)
{
    constexpr auto overflow = is_same_size<size_t, uint64_t>;
    const checked writer{};
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE_EQUAL(writer.is_buffer_overflow_(max_size_t), overflow);
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___unchecked__false)
{
    const unchecked writer{};
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE(!writer.is_buffer_overflow_(max_size_t));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
