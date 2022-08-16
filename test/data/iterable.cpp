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

BOOST_AUTO_TEST_SUITE(iterable_tests)

BOOST_AUTO_TEST_CASE(iterable__default_construct__always__empty)
{
    const system::iterable<data_array<42>> iterable{};
    BOOST_REQUIRE(iterable.empty());
    BOOST_REQUIRE_EQUAL(iterable.size(), 0u);
    BOOST_REQUIRE_EQUAL(to_unsigned(std::distance(iterable.begin(), iterable.end())), 0u);
}

BOOST_AUTO_TEST_CASE(mutable_iterable__default_construct__always__empty)
{
    mutable_iterable<data_array<42>> iterable{};
    BOOST_REQUIRE(iterable.empty());
    BOOST_REQUIRE_EQUAL(iterable.size(), 0u);
    BOOST_REQUIRE_EQUAL(to_unsigned(std::distance(iterable.begin(), iterable.end())), 0u);
}

BOOST_AUTO_TEST_CASE(iterable__distance__always__expected)
{
    const auto& data = null_hash;
    constexpr auto count = hash_size / 3;
    const auto iterable = system::iterable<data_array<3>>(data);
    BOOST_REQUIRE_EQUAL(iterable.size(), count);
    BOOST_CHECK_EQUAL(to_unsigned(std::distance(iterable.begin(), iterable.end())), count);
}

BOOST_AUTO_TEST_CASE(mutable_iterable__distance__always__expected)
{
    auto data = null_hash;
    constexpr auto count = hash_size / 3;
    auto iterable = mutable_iterable<data_array<3>>(data);
    BOOST_REQUIRE_EQUAL(iterable.size(), count);
    BOOST_CHECK_EQUAL(to_unsigned(std::distance(iterable.begin(), iterable.end())), count);
}

BOOST_AUTO_TEST_CASE(iterable__size__always__expected)
{
    constexpr auto& data = null_hash;
    BOOST_REQUIRE_EQUAL(iterable<data_array<0>>(data).size(),  0u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<1>>(data).size(), 32u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<2>>(data).size(), 16u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<3>>(data).size(), 10u); // r2
    BOOST_REQUIRE_EQUAL(iterable<data_array<4>>(data).size(),  8u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<5>>(data).size(),  6u); // r2
    BOOST_REQUIRE_EQUAL(iterable<data_array<6>>(data).size(),  5u); // r2
    BOOST_REQUIRE_EQUAL(iterable<data_array<7>>(data).size(),  4u); // r4
    BOOST_REQUIRE_EQUAL(iterable<data_array<8>>(data).size(),  4u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<9>>(data).size(),  3u); // r5
    BOOST_REQUIRE_EQUAL(iterable<data_array<10>>(data).size(), 3u); // r2
    BOOST_REQUIRE_EQUAL(iterable<data_array<11>>(data).size(), 2u); // r10
    BOOST_REQUIRE_EQUAL(iterable<data_array<12>>(data).size(), 2u); // r8
    BOOST_REQUIRE_EQUAL(iterable<data_array<13>>(data).size(), 2u); // r6
    BOOST_REQUIRE_EQUAL(iterable<data_array<14>>(data).size(), 2u); // r4
    BOOST_REQUIRE_EQUAL(iterable<data_array<15>>(data).size(), 2u); // r2
    BOOST_REQUIRE_EQUAL(iterable<data_array<16>>(data).size(), 2u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<17>>(data).size(), 1u); //r15
    BOOST_REQUIRE_EQUAL(iterable<data_array<31>>(data).size(), 1u); //r1
    BOOST_REQUIRE_EQUAL(iterable<data_array<32>>(data).size(), 1u);
    BOOST_REQUIRE_EQUAL(iterable<data_array<33>>(data).size(), 0u);
}

BOOST_AUTO_TEST_CASE(mutable_iterable__size__always__expected)
{
    auto data = null_hash;
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<0>>(data).size(),  0u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<1>>(data).size(), 32u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<2>>(data).size(), 16u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<3>>(data).size(), 10u); // r2
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<4>>(data).size(),  8u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<5>>(data).size(),  6u); // r2
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<6>>(data).size(),  5u); // r2
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<7>>(data).size(),  4u); // r4
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<8>>(data).size(),  4u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<9>>(data).size(),  3u); // r5
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<10>>(data).size(), 3u); // r2
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<11>>(data).size(), 2u); // r10
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<12>>(data).size(), 2u); // r8
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<13>>(data).size(), 2u); // r6
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<14>>(data).size(), 2u); // r4
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<15>>(data).size(), 2u); // r2
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<16>>(data).size(), 2u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<17>>(data).size(), 1u); //r15
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<31>>(data).size(), 1u); //r1
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<32>>(data).size(), 1u);
    BOOST_REQUIRE_EQUAL(mutable_iterable<data_array<33>>(data).size(), 0u);
}

BOOST_AUTO_TEST_CASE(iterable__data_arrays__parsing__expected)
{
    const data_chunk data{};
    using type = data_array<8>;
    static_assert(is_same_type<decltype(*iterable<type>(data).begin()), const type&>);
    static_assert(is_same_type<decltype(*(++iterable<type>(data).begin())), const type&>);

    constexpr auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");
    const system::iterable<type> iterable(hash);
    const auto end = iterable.end();
    auto it = iterable.begin();

    // Boost macros can't handle the inline indirections.
    type element{ *it++ };
    BOOST_REQUIRE_EQUAL(element, base16_array("1111111122222222"));
    element = *it++;
    BOOST_REQUIRE_EQUAL(element, base16_array("3333333344444444"));
    element = *it++;
    BOOST_REQUIRE_EQUAL(element, base16_array("5555555566666666"));
    element = *it++;
    BOOST_REQUIRE_EQUAL(element, base16_array("7777777788888888"));
    BOOST_REQUIRE(it == end);
}

BOOST_AUTO_TEST_CASE(iterable__nested_arrays__parsing__expected)
{
    const data_chunk data{};
    using type = std::array<std::array<uint8_t, 8>, 2>;
    static_assert(is_same_type<decltype(*iterable<type>(data).begin()), const type&>);
    static_assert(is_same_type<decltype(*(++iterable<type>(data).begin())), const type&>);

    constexpr auto expected0 = type
    {
        {
            { 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22 },
            { 0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44 }
        }
    };

    constexpr auto expected1 = type
    {
        {
            { 0x55, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66 },
            { 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88 }
        }
    };

    constexpr auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");
    auto it = iterable<type>(hash).begin();
    BOOST_REQUIRE_EQUAL(*it, expected0);
    BOOST_REQUIRE_EQUAL(*(++it), expected1);
}

BOOST_AUTO_TEST_CASE(iterable__nested_arrays__advance__expected)
{
    const data_chunk data{};
    using type = std::array<std::array<uint8_t, 8>, 2>;

    constexpr auto expected = type
    {
        {
            { 0x55, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66 },
            { 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88 }
        }
    };

    constexpr auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");
    auto it = iterable<type>(hash).template advance<1>().begin();
    BOOST_REQUIRE_EQUAL(*it, expected);
}

BOOST_AUTO_TEST_CASE(mutable_iterable__nested_arrays__insertions__expected)
{
    data_chunk data{};
    using type = std::array<std::array<uint8_t, 2>, 4>;
    static_assert(is_same_type<decltype(*mutable_iterable<type>(data).begin()), type&> );
    static_assert(is_same_type<decltype(*(++mutable_iterable<type>(data).begin())), type&> );

    // type is 8 bytes, so setting .at(1) will set byte 1 and every +8.
    constexpr auto expected = base16_array("11aa11112222222233aa33334444444455aa55556666666677aa777788888888");
    auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");

    for (auto& chunk: mutable_iterable<type>(hash))
        chunk.front().at(1) = 0xaa;

    BOOST_REQUIRE_EQUAL(hash, expected);
}

BOOST_AUTO_TEST_CASE(mutable_iterable__nested_arrays__advance__expected)
{
    data_chunk data{};
    using type = std::array<std::array<uint8_t, 2>, 4>;
    auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");

    // type is 8 bytes, so advance(1) setting .at(0) will set byte 8 and every +8.
    for (auto& chunk: mutable_iterable<type>(hash).template advance<1>())
        chunk.front().at(0) = 0xaa;

    constexpr auto expected = base16_array("1111111122222222aa33333344444444aa55555566666666aa77777788888888");
    BOOST_REQUIRE_EQUAL(hash, expected);
}

BOOST_AUTO_TEST_CASE(iterable__array_cast__always__expected)
{
    constexpr sha256::block_t expected1{ 1 };
    constexpr sha256::block_t expected2{ 2 };
    constexpr sha256::block_t expected3{ 3 };
    constexpr sha256::block_t expected4{ 4 };
    constexpr sha256::block_t expected5{ 5 };
    constexpr sha256::block_t expected6{ 6 };
    constexpr sha256::block_t expected7{ 7 };
    constexpr sha256::block_t expected8{ 8 };
    constexpr sha256::block_t expected9{ 9 };
    constexpr sha256::block_t expected10{ 10 };
    constexpr sha256::block_t expected11{ 11 };
    constexpr sha256::block_t expected12{ 12 };
    constexpr sha256::block_t expected13{ 13 };
    constexpr sha256::block_t expected14{ 14 };
    constexpr sha256::block_t expected15{ 15 };

    using iterable_t = iterable<sha256::block_t>;
    std_vector<sha256::block_t> data{};
    data.reserve(15);

    data.push_back(expected1);
    data.push_back(expected2);
    data.push_back(expected3);
    data.push_back(expected4);
    data.push_back(expected5);
    data.push_back(expected6);
    data.push_back(expected7);
    data.push_back(expected8);
    data.push_back(expected9);
    data.push_back(expected10);
    data.push_back(expected11);
    data.push_back(expected12);
    data.push_back(expected13);
    data.push_back(expected14);
    data.push_back(expected15);

    const auto size = data.size() * array_count<sha256::block_t>;
    const auto bytes = data.front().data();
    const iterable_t iterable(size, bytes);

    BOOST_REQUIRE_EQUAL(iterable.size(), 15u);
    BOOST_REQUIRE_EQUAL(iterable.data(), data.front().data());

    ////const auto& blocks8 = array_cast<8>(iterable);    // zero offset overload.
    ////const auto& blocks4 = array_cast<4>(iterable, 8);
    ////const auto& blocks2 = array_cast<2>(iterable, 12);
    ////const auto& blocks1 = array_cast<1>(iterable, 14);
    ////const auto& blocks0 = array_cast<0>(iterable, 15); // ok

    ////static_assert(is_same_type<decltype(blocks8), const std_array<sha256::block_t, 8>&>);
    ////static_assert(is_same_type<decltype(blocks4), const std_array<sha256::block_t, 4>&>);
    ////static_assert(is_same_type<decltype(blocks2), const std_array<sha256::block_t, 2>&>);
    ////static_assert(is_same_type<decltype(blocks1), const std_array<sha256::block_t, 1>&>);
    ////static_assert(is_same_type<decltype(blocks0), const std_array<sha256::block_t, 0>&>);

    ////// 8 lanes
    ////BOOST_REQUIRE_EQUAL(blocks8[0], expected1);
    ////BOOST_REQUIRE_EQUAL(blocks8[1], expected2);
    ////BOOST_REQUIRE_EQUAL(blocks8[2], expected3);
    ////BOOST_REQUIRE_EQUAL(blocks8[3], expected4);
    ////BOOST_REQUIRE_EQUAL(blocks8[4], expected5);
    ////BOOST_REQUIRE_EQUAL(blocks8[5], expected6);
    ////BOOST_REQUIRE_EQUAL(blocks8[6], expected7);
    ////BOOST_REQUIRE_EQUAL(blocks8[7], expected8);

    ////// 4 lanes
    ////BOOST_REQUIRE_EQUAL(blocks4[0], expected9);
    ////BOOST_REQUIRE_EQUAL(blocks4[1], expected10);
    ////BOOST_REQUIRE_EQUAL(blocks4[2], expected11);
    ////BOOST_REQUIRE_EQUAL(blocks4[3], expected12);

    ////// 2 lanes
    ////BOOST_REQUIRE_EQUAL(blocks2[0], expected13);
    ////BOOST_REQUIRE_EQUAL(blocks2[1], expected14);

    ////// 1 lane
    ////BOOST_REQUIRE_EQUAL(blocks1[0], expected15);

    const auto& blocks8_ = iterable.template to_array<8>(); // no offsetting (use advance)
    const auto& blocks4_ = iterable.template to_array<4>();
    const auto& blocks2_ = iterable.template to_array<2>();
    const auto& blocks1_ = iterable.template to_array<1>();
    const auto& blocks0_ = iterable.template to_array<0>(); // ok

    static_assert(is_same_type<decltype(blocks8_), const std_array<sha256::block_t, 8>&>);
    static_assert(is_same_type<decltype(blocks4_), const std_array<sha256::block_t, 4>&>);
    static_assert(is_same_type<decltype(blocks2_), const std_array<sha256::block_t, 2>&>);
    static_assert(is_same_type<decltype(blocks1_), const std_array<sha256::block_t, 1>&>);
    static_assert(is_same_type<decltype(blocks0_), const std_array<sha256::block_t, 0>&>);

    // 8 lanes
    BOOST_REQUIRE_EQUAL(blocks8_[0], expected1);
    BOOST_REQUIRE_EQUAL(blocks8_[1], expected2);
    BOOST_REQUIRE_EQUAL(blocks8_[2], expected3);
    BOOST_REQUIRE_EQUAL(blocks8_[3], expected4);
    BOOST_REQUIRE_EQUAL(blocks8_[4], expected5);
    BOOST_REQUIRE_EQUAL(blocks8_[5], expected6);
    BOOST_REQUIRE_EQUAL(blocks8_[6], expected7);
    BOOST_REQUIRE_EQUAL(blocks8_[7], expected8);

    // 4 lanes
    BOOST_REQUIRE_EQUAL(blocks4_[0], expected1);
    BOOST_REQUIRE_EQUAL(blocks4_[1], expected2);
    BOOST_REQUIRE_EQUAL(blocks4_[2], expected3);
    BOOST_REQUIRE_EQUAL(blocks4_[3], expected4);

    // 2 lanes
    BOOST_REQUIRE_EQUAL(blocks2_[0], expected1);
    BOOST_REQUIRE_EQUAL(blocks2_[1], expected2);

    // 1 lane
    BOOST_REQUIRE_EQUAL(blocks1_[0], expected1);
}

// Some extra stuff
// ----------------------------------------------------------------------------

template<size_t From, size_t To>
class range
{
public:
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = size_t*;
        using reference = size_t;

        explicit iterator(size_t position = zero) noexcept
          : position_(position)
        {
        }

        constexpr reference operator*() const noexcept
        {
            return position_;
        }

        constexpr iterator& operator++() noexcept
        {
            position_ = (To >= From ? add1(position_) : sub1(position_));
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto self = *this;
            ++(*this);
            return self;
        }

        constexpr bool operator==(iterator other) const noexcept
        {
            return position_ == other.position_;
        }

        constexpr bool operator!=(iterator other) const noexcept
        {
            return !(*this == other);
        }

    private:
        size_t position_ = From;
    };

    constexpr iterator begin() noexcept
    {
        return iterator(From);
    }

    constexpr iterator end() const noexcept
    {
        return iterator(To >= From ? add1(To) : sub1(To));
    }
};

BOOST_AUTO_TEST_SUITE_END()
