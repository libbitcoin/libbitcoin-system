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

BOOST_AUTO_TEST_SUITE(accumulator1_tests)

// TODO: merge with accumulator.cpp and delete this file.

using algorithm = sha::algorithm<sha::sha256>;

struct accessor
  : accumulator<algorithm, true>
{
    using base = accumulator<algorithm, true>;
    using base::base;

    constexpr size_t next() const NOEXCEPT
    {
        return base::next();
    }

    constexpr size_t gap() const NOEXCEPT
    {
        return base::gap();
    }

    constexpr bool is_buffer_overflow(size_t bytes) const NOEXCEPT
    {
        return base::is_buffer_overflow(bytes);
    }

    constexpr size_t add_data(size_t bytes, const byte_t* data) NOEXCEPT
    {
        return base::add_data(bytes, data);
    }

    constexpr void increment(size_t blocks) NOEXCEPT
    {
        return base::increment(blocks);
    }

    constexpr size_t pad_size() const NOEXCEPT
    {
        return base::pad_size();
    }

    static constexpr counter serialize(size_t bytes) NOEXCEPT
    {
        return base::serialize(bytes);
    }

    static CONSTEVAL block_t stream_pad() NOEXCEPT
    {
        return base::stream_pad();
    }
};

BOOST_AUTO_TEST_CASE(accumulator__construct__default__initial)
{
    const accessor writer{};
    BOOST_REQUIRE_EQUAL(writer.next(), 0u);
    BOOST_REQUIRE_EQUAL(writer.gap(), 64u);
    BOOST_REQUIRE_EQUAL(writer.pad_size(), 64u);
    BOOST_REQUIRE(!writer.is_buffer_overflow(max_uint16));
}

BOOST_AUTO_TEST_CASE(accumulator__construct__sized__initial)
{
    constexpr algorithm::state_t state{ 1, 2, 3, 4, 5, 6, 7, 8 };
    constexpr data_array<42> data{};
    const accessor writer{ 42, state };
    BOOST_REQUIRE_EQUAL(writer.next(), 0u);
    BOOST_REQUIRE_EQUAL(writer.gap(), 64u);
    BOOST_REQUIRE_EQUAL(writer.pad_size(), 64u);
    BOOST_REQUIRE(!writer.is_buffer_overflow(max_uint16));
}

BOOST_AUTO_TEST_SUITE_END()
