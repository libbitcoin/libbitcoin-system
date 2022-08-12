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

BOOST_AUTO_TEST_CASE(const_iterable__data_arrays__parsing__expected)
{
    const data_chunk data{};
    using type = data_array<8>;
    static_assert(is_same_type<decltype(*const_iterable<type>(data).begin()), const type&>);
    static_assert(is_same_type<decltype(*(++const_iterable<type>(data).begin())), const type&>);

    constexpr auto hash = base16_array("1111111122222222333333334444444455555555666666667777777788888888");
    const auto iterable = const_iterable<type>(hash);
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

BOOST_AUTO_TEST_CASE(const_iterable__nested_arrays__parsing__expected)
{
    const data_chunk data{};
    using type = std::array<std::array<uint8_t, 8>, 2>;
    static_assert(is_same_type<decltype(*const_iterable<type>(data).begin()), const type&>);
    static_assert(is_same_type<decltype(*(++const_iterable<type>(data).begin())), const type&>);

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
    auto it = const_iterable<type>(hash).begin();
    BOOST_REQUIRE_EQUAL(*it, expected0);
    BOOST_REQUIRE_EQUAL(*(++it), expected1);
}

BOOST_AUTO_TEST_CASE(iterable__nested_arrays__insertions__expected)
{
    data_chunk data{};
    using type = std::array<std::array<uint8_t, 2>, 4>;
    static_assert(is_same_type<decltype(*iterable<type>(data).begin()), type& > );
    static_assert(is_same_type<decltype(*(++iterable<type>(data).begin())), type& > );

    constexpr auto expected = base16_array("11aa11112222222233aa33334444444455aa55556666666677aa777788888888");
    auto hash______________ = base16_array("1111111122222222333333334444444455555555666666667777777788888888");

    for (auto& chunk: iterable<type>(hash______________))
        chunk.front().at(1) = 0xaa;

    BOOST_REQUIRE_EQUAL(hash______________, expected);
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
