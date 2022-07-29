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

template <typename Algorithm>
typename system::accumulator<Algorithm>::digest_t accumulated(
    const data_slice& data) NOEXCEPT
{
    system::accumulator<Algorithm> accumulator{};
    accumulator.write(data.size(), data.data());
    return accumulator.flush();
}

template <typename Algorithm>
typename system::accumulator<Algorithm>::digest_t accumulated(
    const data_slice& left, const data_slice& right) NOEXCEPT
{
    system::accumulator<Algorithm> accumulator{};
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    return accumulator.flush();
}

BOOST_AUTO_TEST_SUITE(rmd_accumulator_tests)

// Concurrency off for debug testing.
using rmd_128 = rmd::algorithm<rmd::h128<>, false>;
using rmd_160 = rmd::algorithm<rmd::h160<>, false>;

// rmd128
// ----------------------------------------------------------------------------
constexpr auto half128 = rmd_128::half_t{};
constexpr auto full128 = rmd_128::block_t{};
constexpr auto pair128 = std_array<uint8_t, array_count<rmd_128::block_t>* two>{};
constexpr auto expected_half128 = base16_array("00000000000000000000000000000000");
constexpr auto expected_full128 = base16_array("00000000000000000000000000000000");
constexpr auto expected_pair128 = base16_array("00000000000000000000000000000000");

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_half128__null_hashes__expected)
{
    // no baseline ripemd128_hash function.
    ////BOOST_CHECK_NE(accumulated<rmd_128>(half128), expected_half128);
    ////BOOST_CHECK_EQUAL(system::ripemd128_hash(half128), expected_half128);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_one128__null_hashes__expected)
{
    // no baseline ripemd128_hash function.
    ////BOOST_CHECK_NE(accumulated<rmd_128>(full128), expected_full128);
    ////BOOST_CHECK_EQUAL(system::ripemd128_hash(full128), expected_full128);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_two128__null_hashes__expected)
{
    // no baseline ripemd128_hash function.
    ////BOOST_CHECK_NE(accumulated<rmd_128>(full128, full128), expected_pair128);
    ////BOOST_CHECK_EQUAL(system::ripemd128_hash(pair128), expected_pair128);
}

// rmd160
// ----------------------------------------------------------------------------

constexpr auto half160 = rmd_160::half_t{};
constexpr auto full160 = rmd_160::block_t{};
constexpr auto pair160 = std_array<uint8_t, array_count<rmd_160::block_t>* two>{};
constexpr auto expected_half160 = base16_array("d1a70126ff7a149ca6f9b638db084480440ff842");
constexpr auto expected_full160 = base16_array("9b8ccc2f374ae313a914763cc9cdfb47bfe1c229");
constexpr auto expected_pair160 = base16_array("4300a157335cb7c9fc9423e011d7dd51090d093f");

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_half160__null_hashes__expected)
{
    ////BOOST_CHECK_NE(accumulated<rmd_160>(half160), expected_half160);
    ////BOOST_CHECK_EQUAL(system::ripemd160_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_one160__null_hashes__expected)
{
    ////BOOST_CHECK_NE(accumulated<rmd_160>(full160), expected_full160);
    ////BOOST_CHECK_EQUAL(system::ripemd160_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_hash_two160__null_hashes__expected)
{
    ////BOOST_CHECK_NE(accumulated<rmd_160>(full160, full160), expected_pair160);
    ////BOOST_CHECK_EQUAL(system::ripemd160_hash(pair160), expected_pair160);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sha_accumulator_tests)

using sha_160 = sha::algorithm<sha::h160>;
using sha_256 = sha::algorithm<sha::h256<>>;
using sha_512 = sha::algorithm<sha::h512<>>;

// sha160
// ----------------------------------------------------------------------------

constexpr auto half160 = sha_160::half_t{};
constexpr auto full160 = sha_160::block_t{};
constexpr auto pair160 = std_array<uint8_t, array_count<sha_160::block_t> * two>{};
constexpr auto expected_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
constexpr auto expected_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
constexpr auto expected_pair160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
constexpr auto expected_merk160 = base16_array("a8782546751527a4a1ad999058165d6af22421c8");

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_half160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_160>(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::sha1_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_one160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_160>(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::sha1_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_160>(full160, full160), expected_pair160);
    BOOST_CHECK_EQUAL(system::sha1_hash(pair160), expected_pair160);
}

// sha256
// ----------------------------------------------------------------------------

constexpr auto half256 = sha_256::half_t{};
constexpr auto full256 = sha_256::block_t{};
constexpr auto pair256 = std_array<uint8_t, array_count<sha_256::block_t> * two>{};
constexpr auto expected_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
constexpr auto expected_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
constexpr auto expected_pair256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
constexpr auto expected_merk256 = base16_array("e2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9");

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_half256__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_256>(half256), expected_half256);
    BOOST_CHECK_EQUAL(system::sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_one256__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_256>(full256), expected_full256);
    BOOST_CHECK_EQUAL(system::sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two256__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_256>(full256, full256), expected_pair256);
    BOOST_CHECK_EQUAL(system::sha256_hash(pair256), expected_pair256);

    // dedicated doubler in bc lib for sha256.
    BOOST_CHECK_EQUAL(system::sha256_hash(full256, full256), system::sha256_hash(pair256));
}

// sha512
// ----------------------------------------------------------------------------

constexpr auto half512 = sha_512::half_t{};
constexpr auto full512 = sha_512::block_t{};
constexpr auto pair512 = std_array<uint8_t, array_count<sha_512::block_t> * two>{};
constexpr auto expected_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
constexpr auto expected_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
constexpr auto expected_pair512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
constexpr auto expected_merk512 = base16_array("56d3e5825edf06e467e50dfeb09c1df2d9940121c05d61a162bfcb80aea3aa5fe958d917ac993d76cd3ea86240fedbb79520ce7b9c275793e3c75a82116cc320");

BOOST_AUTO_TEST_CASE(accumulator__sha_hash__half512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_512>(half512), expected_half512);
    BOOST_CHECK_EQUAL(system::sha512_hash(half512), expected_half512);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash__one512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_512>(full512), expected_full512);
    BOOST_CHECK_EQUAL(system::sha512_hash(full512), expected_full512);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two512__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulated<sha_512>(full512, full512), expected_pair512);
    BOOST_CHECK_EQUAL(system::sha512_hash(pair512), expected_pair512);
}

BOOST_AUTO_TEST_SUITE_END()

using algorithm = sha::algorithm<sha::sha256>;

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

    static constexpr typename base::counter serialize_(size_t bytes) NOEXCEPT
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

// serialize
constexpr auto count = 42_size;
constexpr auto count_bits = to_bits<uint16_t>(count);
constexpr auto count_lo = lo_word<algorithm::byte_t>(count_bits);
constexpr auto count_hi = hi_word<algorithm::byte_t>(count_bits);
using counter_t = std_array<algorithm::byte_t, algorithm::count_bytes>;
constexpr auto expected_counter = counter_t{ 0, 0, 0, 0, 0, 0, count_hi, count_lo };
static_assert(checked::serialize_(count) == expected_counter);

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
    constexpr auto blocks = 2;
    constexpr algorithm::state_t state{};

    const checked writer{ blocks, state };
    BOOST_REQUIRE_EQUAL(writer.next_(), zero);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - count_size);
}

BOOST_AUTO_TEST_CASE(accumulator__write__zero__true)
{
    constexpr data_array<0> data{};
    checked writer{};
    BOOST_REQUIRE(writer.write(data));
}

BOOST_AUTO_TEST_CASE(accumulator__write__nonzero__expected)
{
    constexpr auto bytes = 42;
    constexpr auto blocks = 2;
    constexpr data_array<bytes> data{};
    constexpr algorithm::state_t state{};

    checked writer{ blocks, state };
    BOOST_REQUIRE(writer.write(data));
    BOOST_REQUIRE_EQUAL(writer.next_(), bytes);
    BOOST_REQUIRE_EQUAL(writer.gap_(), block_size - bytes);
    BOOST_REQUIRE_EQUAL(writer.pad_size_(), block_size - bytes - algorithm::count_bytes);
    BOOST_REQUIRE_EQUAL(writer.flush(), base16_array("729e145a50396134c294aaf8e2daea0b7c89bb617cd58379ba4c2abdec2d1da7"));
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___checked__expected)
{
    const checked writer{};
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE(writer.is_buffer_overflow_(max_uint64));
}

BOOST_AUTO_TEST_CASE(accumulator__is_buffer_overflow___unchecked__false)
{
    const unchecked writer{};
    BOOST_REQUIRE(!writer.is_buffer_overflow_(zero));
    BOOST_REQUIRE(!writer.is_buffer_overflow_(max_uint64));
}

BOOST_AUTO_TEST_SUITE_END()
