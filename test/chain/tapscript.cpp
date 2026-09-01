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

BOOST_AUTO_TEST_SUITE(tapscript_tests)

using namespace system::chain;

// is_control (size must be 33 plus a multiple of 32, within key limit)

BOOST_AUTO_TEST_CASE(tapscript__is_control__empty__false)
{
    BOOST_REQUIRE(!tapscript::is_control({}));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__thirty_two_bytes__false)
{
    BOOST_REQUIRE(!tapscript::is_control(data_chunk(32, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__thirty_three_bytes__true)
{
    BOOST_REQUIRE(tapscript::is_control(data_chunk(33, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__thirty_four_bytes__false)
{
    BOOST_REQUIRE(!tapscript::is_control(data_chunk(34, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__one_path_element__true)
{
    BOOST_REQUIRE(tapscript::is_control(data_chunk(65, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__two_path_elements__true)
{
    BOOST_REQUIRE(tapscript::is_control(data_chunk(97, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__maximum_path__true)
{
    constexpr size_t maximum = 33u + 32u * taproot_max_keys;
    BOOST_REQUIRE(tapscript::is_control(data_chunk(maximum, 0x00)));
}

BOOST_AUTO_TEST_CASE(tapscript__is_control__above_maximum_path__false)
{
    constexpr size_t above_maximum = 65u + 32u * taproot_max_keys;
    BOOST_REQUIRE(!tapscript::is_control(data_chunk(above_maximum, 0x00)));
}

// parsing

BOOST_AUTO_TEST_CASE(tapscript__parsing__odd_parity_tapscript__expected)
{
    const auto control = to_shared(base16_chunk("c1187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"));
    const tapscript instance{ control };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.is_tapscript());
    BOOST_REQUIRE(instance.parity());
    BOOST_REQUIRE_EQUAL(instance.version(), tapscript_version);
    BOOST_REQUIRE_EQUAL(instance.count(), 0u);

    constexpr ec_xonly expected = base16_array("187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27");
    BOOST_REQUIRE_EQUAL(instance.key(), expected);
}

BOOST_AUTO_TEST_CASE(tapscript__parsing__even_parity_with_path__expected)
{
    const auto control = to_shared(base16_chunk("c0ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a"));
    const tapscript instance{ control };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.is_tapscript());
    BOOST_REQUIRE(!instance.parity());
    BOOST_REQUIRE_EQUAL(instance.version(), tapscript_version);
    BOOST_REQUIRE_EQUAL(instance.count(), 1u);

    constexpr ec_xonly expected_key = base16_array("ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592");
    constexpr ec_xonly expected_path = base16_array("f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a");
    BOOST_REQUIRE_EQUAL(instance.key(), expected_key);
    BOOST_REQUIRE_EQUAL(instance.keys().at(0), expected_path);
}

BOOST_AUTO_TEST_CASE(tapscript__is_tapscript__version_250_leaf__false)
{
    const auto control = to_shared(base16_chunk("fa187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"));
    const tapscript instance{ control };
    BOOST_REQUIRE(!instance.is_tapscript());
}

BOOST_AUTO_TEST_SUITE_END()
