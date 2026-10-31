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

BC_AUTO_TEST_SUITE(golomb_tests)

// BIP158 golomb-rice parameters.
constexpr uint8_t golomb_bits = 19;
constexpr uint64_t golomb_rate = 784931;
constexpr auto golomb_entropy = base16_array("0000000000000000000000000000000a");
constexpr auto golomb_key = to_siphash_key(golomb_entropy);

static data_stack golomb_items() NOEXCEPT
{
    return
    {
        base16_chunk("0102030405"),
        base16_chunk("0607080910"),
        base16_chunk("1112131415"),
        base16_chunk("1617181920")
    };
}

static data_chunk golomb_set() NOEXCEPT
{
    const auto items = golomb_items();
    return golomb::construct(items, golomb_bits, golomb_key, golomb_rate);
}

// construct
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(golomb__construct__populated__not_empty)
{
    BC_REQUIRE(!golomb_set().empty());
}

BC_AUTO_TEST_CASE(golomb__construct__half_hash_entropy__same_as_siphash_key)
{
    const auto items = golomb_items();
    const auto expected = golomb_set();
    const auto set = golomb::construct(items, golomb_bits, golomb_entropy, golomb_rate);
    BC_REQUIRE_EQUAL(set, expected);
}

BC_AUTO_TEST_CASE(golomb__hash_distinct__duplicate_item__sorted_distinct)
{
    auto items = golomb_items();
    items.push_back(items.front());
    const auto hashes = golomb::hash_distinct(items, golomb_key);
    BC_REQUIRE_EQUAL(hashes.size(), golomb_items().size());
    BC_REQUIRE(std::is_sorted(hashes.cbegin(), hashes.cend()));
}

BC_AUTO_TEST_CASE(golomb__hash_distinct__slices__same_as_stack)
{
    auto items = golomb_items();
    items.push_back(items.front());
    const std::vector<data_slice> slices(items.cbegin(), items.cend());
    BC_REQUIRE(golomb::hash_distinct(slices, golomb_key) == golomb::hash_distinct(items, golomb_key));
}

BC_AUTO_TEST_CASE(golomb__construct_bound__items__not_below_construct_size)
{
    const auto hashes = golomb::hash_distinct(golomb_items(), golomb_key);
    const auto bound = golomb::construct_bound(hashes.size(), golomb_bits, golomb_rate);
    BC_REQUIRE_GE(bound, golomb_set().size());
}

BC_AUTO_TEST_CASE(golomb__construct_bound__zero__zero)
{
    BC_REQUIRE_EQUAL(golomb::construct_bound(0, golomb_bits, golomb_rate), 0u);
}

BC_AUTO_TEST_CASE(golomb__construct_bound__maximum_values__not_below_construct_size)
{
    data_chunk set{};
    stream::out::data stream(set);
    write::bits::ostream writer(stream);
    const std::vector<uint64_t> hashes{ sub1(sub1(max_uint64)), sub1(max_uint64), max_uint64 };
    golomb::construct(writer, hashes, golomb_bits, golomb_rate);
    writer.flush();
    BC_REQUIRE_GE(golomb::construct_bound(hashes.size(), golomb_bits, golomb_rate), set.size());
}

BC_AUTO_TEST_CASE(golomb__construct__distinct_hashes__same_as_items)
{
    data_chunk set{};
    stream::out::data stream(set);
    write::bits::ostream writer(stream);
    golomb::construct(writer, golomb::hash_distinct(golomb_items(), golomb_key), golomb_bits, golomb_rate);
    writer.flush();
    BC_REQUIRE_EQUAL(set, golomb_set());
}

BC_AUTO_TEST_CASE(golomb__construct__no_items__empty)
{
    const data_stack items{};
    const auto set = golomb::construct(items, golomb_bits, golomb_key, golomb_rate);
    BC_REQUIRE(set.empty());
}

BC_AUTO_TEST_CASE(golomb__construct__rate_overflow__empty)
{
    const auto items = golomb_items();
    const auto set = golomb::construct(items, golomb_bits, golomb_key, max_uint64);
    BC_REQUIRE(set.empty());
}

// match_single
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(golomb__match_single__each_member__true)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();

    for (const auto& item: items)
    {
        BC_REQUIRE(golomb::match_single(set, item, size, golomb_key, golomb_bits, golomb_rate));
    }
}

BC_AUTO_TEST_CASE(golomb__match_single__non_members__false)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack absent
    {
        base16_chunk("ff"),
        base16_chunk("ffffffffff"),
        base16_chunk("00"),
        base16_chunk("2122232425"),
        base16_chunk("de00ad00be00ef00")
    };

    for (const auto& item: absent)
    {
        BC_REQUIRE(!golomb::match_single(set, item, size, golomb_key, golomb_bits, golomb_rate));
    }
}

BC_AUTO_TEST_CASE(golomb__match_single__half_hash_entropy__member_true)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto& item = items.front();
    const auto size = items.size();
    BC_REQUIRE(golomb::match_single(set, item, size, golomb_entropy, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_single__half_hash_entropy__non_member_false)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto absent = base16_chunk("2122232425");
    const auto size = items.size();
    BC_REQUIRE(!golomb::match_single(set, absent, size, golomb_entropy, golomb_bits, golomb_rate));
}

// match_stack
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(golomb__match_stack__member__true)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack targets{ items.front() };
    BC_REQUIRE(golomb::match_stack(set, targets, size, golomb_key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__all_members__true)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    BC_REQUIRE(golomb::match_stack(set, items, size, golomb_key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__empty_targets__false)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack targets{};
    BC_REQUIRE(!golomb::match_stack(set, targets, size, golomb_key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__non_members__false)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack targets
    {
        base16_chunk("2122232425"),
        base16_chunk("2627282930")
    };

    BC_REQUIRE(!golomb::match_stack(set, targets, size, golomb_key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__half_hash_entropy__member_true)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack targets{ items.back() };
    BC_REQUIRE(golomb::match_stack(set, targets, size, golomb_entropy, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__half_hash_entropy__empty_targets_false)
{
    const auto items = golomb_items();
    const auto set = golomb_set();
    const auto size = items.size();
    const data_stack targets{};
    BC_REQUIRE(!golomb::match_stack(set, targets, size, golomb_entropy, golomb_bits, golomb_rate));
}

// bip158
// ----------------------------------------------------------------------------

// The published filter prefixes the set with a compact size element count.
constexpr auto bip158_block_hash = base16_hash("000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943");

// Testnet block 0 filter element, the coinbase output script.
static data_chunk bip158_element() NOEXCEPT
{
    return base16_chunk(
        "4104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61de"
        "b649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
        "ac");
}

static siphash_key bip158_key() NOEXCEPT
{
    const auto data = bip158_block_hash.data();
    const auto& half = unsafe_array_cast<uint8_t, half_hash_size>(data);
    return to_siphash_key(half);
}

static data_chunk bip158_filter() NOEXCEPT
{
    return base16_chunk("9dfca8");
}

BC_AUTO_TEST_CASE(golomb__construct__bip158_testnet_genesis__expected_encoding)
{
    const auto key = bip158_key();
    const data_stack elements{ bip158_element() };
    const auto set = golomb::construct(elements, golomb_bits, key, golomb_rate);
    BC_REQUIRE_EQUAL(set, bip158_filter());
}

BC_AUTO_TEST_CASE(golomb__match_single__bip158_testnet_genesis__true)
{
    const auto key = bip158_key();
    const auto published = bip158_filter();
    const auto element = bip158_element();
    BC_REQUIRE(golomb::match_single(published, element, 1, key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_stack__bip158_testnet_genesis__true)
{
    const auto key = bip158_key();
    const auto published = bip158_filter();
    const data_stack targets{ bip158_element() };
    BC_REQUIRE(golomb::match_stack(published, targets, 1, key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_CASE(golomb__match_single__bip158_wrong_key__false)
{
    const auto published = bip158_filter();
    const auto element = bip158_element();
    BC_REQUIRE(!golomb::match_single(published, element, 1, golomb_key, golomb_bits, golomb_rate));
}

BC_AUTO_TEST_SUITE_END()
