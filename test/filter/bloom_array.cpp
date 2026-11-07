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

BC_AUTO_TEST_SUITE(bloom_array_tests)

template <size_t m, size_t k>
class accessor
  : public bloom_array<m, k>
{
public:
    using base = bloom_array<m, k>;
    using base::get_bit;
};

// Ensure default/nop behavior.
static_assert( bloom_array<0, 0>::is_screened({}, 42));
static_assert( bloom_array<1, 0>::is_screened({}, 42));
static_assert(!bloom_array<1, 1>::is_screened({}, 42));

// get_bit
// ----------------------------------------------------------------------------

constexpr auto entropy1 = 0x40000000'40000000_u64;
constexpr auto entropy2 = 0x20000000'40000000_u64;
constexpr auto entropy3 = 0x40000000'00000000_u64;
constexpr auto bit1 = accessor<100, 4>::get_bit(0, entropy1);
constexpr auto bit2 = accessor<100, 4>::get_bit(1, entropy1);
constexpr auto bit3 = accessor<100, 4>::get_bit(2, entropy1);
constexpr auto bit4 = accessor<100, 4>::get_bit(3, entropy1);
constexpr auto bit5 = accessor<100, 4>::get_bit(1, entropy2);
constexpr auto bit6 = accessor<100, 4>::get_bit(1, entropy3);
constexpr auto bit7 = accessor<1'000, 20>::get_bit(0, 0_u64);
constexpr auto bit8 = accessor<1'000, 20>::get_bit(0, max_uint64);
static_assert(bit1 == 25);
static_assert(bit2 == 50);
static_assert(bit3 == 75);
static_assert(is_zero(bit4));
static_assert(bit5 == 37);
static_assert(bit6 == 25);
static_assert(is_zero(bit7));
static_assert(bit8 == 999);

// screen/is_screened
// ----------------------------------------------------------------------------

// A distinct uniform entropy for each index.
static uint64_t to_entropy(size_t index)
{
    const auto hash = sha256_hash(to_little_endian<uint64_t>(index));
    return unsafe_from_little_endian<uint64_t>(hash.data());
}

// The entropies of [first, first + count).
static std_vector<uint64_t> to_entropies(size_t first, size_t count)
{
    std_vector<size_t> indexes(count);
    std::iota(indexes.begin(), indexes.end(), first);
    std_vector<uint64_t> entropies(count);
    std::ranges::transform(indexes, entropies.begin(), to_entropy);

    return entropies;
}

// The number of set bits in the words.
template <typename Words>
static size_t ones(const Words& words)
{
    const auto count = [](size_t total, uint64_t word) NOEXCEPT
    {
        return total + ones_count(word);
    };

    return std::accumulate(words.cbegin(), words.cend(), zero, count);
}

BC_AUTO_TEST_CASE(bloom_array__is_screened__empty__false)
{
    using bloom_t = bloom_array<1'000, 4>;
    const bloom_t::type value{};
    const auto entropy = to_entropy(0);
    BC_CHECK(!bloom_t::is_screened(value, entropy));
}

BC_AUTO_TEST_CASE(bloom_array__screen__one__screened)
{
    using bloom_t = bloom_array<1'000, 4>;
    const auto entropy = to_entropy(0);
    bloom_t::type value{};
    bloom_t::screen(value, entropy);
    BC_CHECK(bloom_t::is_screened(value, entropy));
}

BC_AUTO_TEST_CASE(bloom_array__screen__one__k_bits_set)
{
    using bloom_t = bloom_array<100'000, 4>;
    const auto entropy = to_entropy(0);
    bloom_t::type value{};
    bloom_t::screen(value, entropy);
    BC_CHECK_EQUAL(ones(value), 4_size);
}

BC_AUTO_TEST_CASE(bloom_array__screen__runtime_k__screened_at_lesser_k)
{
    using bloom_t = bloom_array<1'000, 4>;
    const auto entropy = to_entropy(0);
    bloom_t::type value{};
    bloom_t::screen(value, entropy, 2);
    BC_CHECK(bloom_t::is_screened(value, entropy, 1));
    BC_CHECK(bloom_t::is_screened(value, entropy, 2));
}

BC_AUTO_TEST_CASE(bloom_array__is_screened__loaded__false_positives_bounded)
{
    using bloom_t = bloom_array<65'536, 8>;
    const auto loaded = to_entropies(0, 4'000);
    const auto absent = to_entropies(4'000, 100'000);

    bloom_t::type value{};
    std::for_each(loaded.cbegin(), loaded.cend(), [&](uint64_t entropy) NOEXCEPT
    {
        bloom_t::screen(value, entropy);
    });

    const auto positives = std::accumulate(absent.cbegin(), absent.cend(), zero, [&](size_t total, uint64_t entropy) NOEXCEPT
    {
        return total + to_int<size_t>(bloom_t::is_screened(value, entropy));
    });

    BC_CHECK_LT(positives, 100_size);
}

BC_AUTO_TEST_SUITE_END()
