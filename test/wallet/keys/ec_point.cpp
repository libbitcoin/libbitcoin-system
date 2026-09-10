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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(ec_point_tests)

const ec_compressed pointx = base16_array("0245dbb7e2cd3a5de19fde8d556fd567a036f9c377ecf69a9202aa4affce41c623");
const ec_compressed pointy = base16_array("02cfc43e064c50cfd1896766ef70e7da82b16e8cfebd8d5dec618212d0db1e6d12");
const ec_compressed sum_xy = base16_array("03332bf6821c7c0e1080efc131d2b745760a8245c0b91a05f13308ff8600d30525");

BOOST_AUTO_TEST_CASE(ec_point__sum__cool__expected)
{
    // Cool construction here.
    BOOST_REQUIRE_EQUAL((pointx + pointy), sum_xy);
}

// The secp256k1 generator point [SEC 2].
const ec_compressed generator_point = base16_array("0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");

BOOST_AUTO_TEST_CASE(ec_point__generator__always__sec_generator)
{
    BOOST_REQUIRE_EQUAL(ec_point::generator.point(), generator_point);
}

// construction and assignment
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_point__construct__default__invalid)
{
    const ec_point instance{};
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE_EQUAL(instance.point(), null_ec_compressed);
}

BOOST_AUTO_TEST_CASE(ec_point__construct__copy__expected)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.point(), pointx);
}

BOOST_AUTO_TEST_CASE(ec_point__construct__move__expected)
{
    auto copy = pointx;
    const ec_point instance{ std::move(copy) };
    BOOST_REQUIRE_EQUAL(instance.point(), pointx);
}

BOOST_AUTO_TEST_CASE(ec_point__assign__copy__expected)
{
    ec_point instance{};
    instance = pointx;
    BOOST_REQUIRE_EQUAL(instance.point(), pointx);
}

BOOST_AUTO_TEST_CASE(ec_point__assign__move__expected)
{
    auto copy = pointx;
    ec_point instance{};
    instance = std::move(copy);
    BOOST_REQUIRE_EQUAL(instance.point(), pointx);
}

BOOST_AUTO_TEST_CASE(ec_point__cast__compressed__point)
{
    const ec_point instance{ pointx };
    const ec_compressed& cast = instance;
    BOOST_REQUIRE_EQUAL(cast, pointx);
}

// comparison
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_point__equality__same__true)
{
    BOOST_REQUIRE(ec_point{ pointx } == ec_point{ pointx });
    BOOST_REQUIRE(!(ec_point{ pointx } != ec_point{ pointx }));
}

BOOST_AUTO_TEST_CASE(ec_point__equality__different__false)
{
    BOOST_REQUIRE(ec_point{ pointx } != ec_point{ pointy });
    BOOST_REQUIRE(!(ec_point{ pointx } == ec_point{ pointy }));
}

// negation
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_point__negate__twice__round_trips)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(-(-instance) == instance);
}

BOOST_AUTO_TEST_CASE(ec_point__negate__invalid__invalid)
{
    const ec_point instance{};
    BOOST_REQUIRE(!(-instance));
}

// addition and subtraction
// ----------------------------------------------------------------------------

// The sum of a point and its inverse is the point at infinity.
BOOST_AUTO_TEST_CASE(ec_point__add__inverse__invalid)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(!(instance + -instance));
}

BOOST_AUTO_TEST_CASE(ec_point__add__invalid_operand__invalid)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(!(instance + ec_point{}));
    BOOST_REQUIRE(!(ec_point{} + instance));
}

BOOST_AUTO_TEST_CASE(ec_point__subtract__addend__original)
{
    const ec_point sum{ sum_xy };
    BOOST_REQUIRE((sum - ec_point{ pointy }) == ec_point{ pointx });
}

BOOST_AUTO_TEST_CASE(ec_point__subtract__self__invalid)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(!(instance - instance));
}

BOOST_AUTO_TEST_CASE(ec_point__subtract__invalid_operand__invalid)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(!(instance - ec_point{}));
    BOOST_REQUIRE(!(ec_point{} - instance));
}

// multiplication
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_point__multiply__two__doubles)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE((instance * ec_scalar{ 2 }) == (instance + instance));
}

BOOST_AUTO_TEST_CASE(ec_point__multiply__scalar_first__same)
{
    const ec_point instance{ pointx };
    const ec_scalar scalar{ 3 };
    BOOST_REQUIRE((scalar * instance) == (instance * scalar));
}

BOOST_AUTO_TEST_CASE(ec_point__multiply__invalid_scalar__invalid)
{
    const ec_point instance{ pointx };
    BOOST_REQUIRE(!(instance * ec_scalar{}));
}

BOOST_AUTO_TEST_CASE(ec_point__multiply__invalid_point__invalid)
{
    BOOST_REQUIRE(!(ec_point{} * ec_scalar{ 2 }));
}

// arithmetic assignment
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_point__add_assign__valid__sum)
{
    ec_point instance{ pointx };
    instance += ec_point{ pointy };
    BOOST_REQUIRE_EQUAL(instance.point(), sum_xy);
}

BOOST_AUTO_TEST_CASE(ec_point__add_assign__invalid_self__unchanged)
{
    ec_point instance{};
    instance += ec_point{ pointy };
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(ec_point__subtract_assign__valid__difference)
{
    ec_point instance{ sum_xy };
    instance -= ec_point{ pointy };
    BOOST_REQUIRE_EQUAL(instance.point(), pointx);
}

BOOST_AUTO_TEST_CASE(ec_point__subtract_assign__invalid_self__unchanged)
{
    ec_point instance{};
    instance -= ec_point{ pointy };
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(ec_point__multiply_assign__valid__product)
{
    ec_point instance{ pointx };
    instance *= ec_scalar{ 2 };
    BOOST_REQUIRE(instance == (ec_point{ pointx } + ec_point{ pointx }));
}

BOOST_AUTO_TEST_CASE(ec_point__multiply_assign__invalid_self__unchanged)
{
    ec_point instance{};
    instance *= ec_scalar{ 2 };
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_SUITE_END()
