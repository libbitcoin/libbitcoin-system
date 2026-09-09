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

BOOST_AUTO_TEST_SUITE(default_ptr_tests)

using type = data_chunk;
using cptr = std::shared_ptr<const type>;

BOOST_AUTO_TEST_CASE(default_ptr__construct__default__default_instance)
{
    const default_ptr<type> instance{};
    BOOST_REQUIRE(instance.get());
    BOOST_REQUIRE(instance->empty());
}

BOOST_AUTO_TEST_CASE(default_ptr__construct__null__default_instance)
{
    const default_ptr<type> instance{ cptr{} };
    BOOST_REQUIRE_EQUAL(instance.get(), default_ptr<type>{}.get());
}

BOOST_AUTO_TEST_CASE(default_ptr__construct__value__expected)
{
    const auto pointer = to_shared<type>(type{ 0x42 });
    const default_ptr<type> instance{ pointer };
    BOOST_REQUIRE_EQUAL(instance.get(), pointer);
    BOOST_REQUIRE_EQUAL(*instance, type{ 0x42 });
}

BOOST_AUTO_TEST_CASE(default_ptr__copy__always__shared)
{
    const default_ptr<type> instance{ to_shared<type>(type{ 0x42 }) };
    const auto copy = instance;
    BOOST_REQUIRE_EQUAL(copy.get(), instance.get());
}

BOOST_AUTO_TEST_CASE(default_ptr__move_construct__always__source_default)
{
    const auto pointer = to_shared<type>(type{ 0x42 });
    default_ptr<type> instance{ pointer };
    const auto moved = std::move(instance);
    BOOST_REQUIRE_EQUAL(moved.get(), pointer);
    BOOST_REQUIRE_EQUAL(instance.get(), default_ptr<type>{}.get());
}

BOOST_AUTO_TEST_CASE(default_ptr__move_assign__always__source_default)
{
    const auto pointer = to_shared<type>(type{ 0x42 });
    default_ptr<type> instance{ pointer };
    default_ptr<type> assigned{};
    assigned = std::move(instance);
    BOOST_REQUIRE_EQUAL(assigned.get(), pointer);
    BOOST_REQUIRE_EQUAL(instance.get(), default_ptr<type>{}.get());
}

BOOST_AUTO_TEST_CASE(default_ptr__assign__null__default_instance)
{
    default_ptr<type> instance{ to_shared<type>(type{ 0x42 }) };
    instance = cptr{};
    BOOST_REQUIRE_EQUAL(instance.get(), default_ptr<type>{}.get());
}

BOOST_AUTO_TEST_CASE(default_ptr__equality__same_pointer__true)
{
    const auto pointer = to_shared<type>(type{ 0x42 });
    const default_ptr<type> left{ pointer };
    const default_ptr<type> right{ pointer };
    BOOST_REQUIRE(left == right);
}

BOOST_AUTO_TEST_CASE(default_ptr__equality__same_value__false)
{
    const default_ptr<type> left{ to_shared<type>(type{ 0x42 }) };
    const default_ptr<type> right{ to_shared<type>(type{ 0x42 }) };
    BOOST_REQUIRE(left != right);
}

BOOST_AUTO_TEST_CASE(default_ptr__default_instance__distinct_types__distinct)
{
    BOOST_REQUIRE_NE(
        static_cast<const void*>(default_ptr<data_chunk>{}.get().get()),
        static_cast<const void*>(default_ptr<hash_digest>{}.get().get()));
}

BOOST_AUTO_TEST_SUITE_END()
