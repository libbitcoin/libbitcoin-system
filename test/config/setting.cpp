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

using namespace bc::system::config;

BOOST_AUTO_TEST_SUITE(setting_tests)

BOOST_AUTO_TEST_CASE(setting__is_configured__empty_variables__false)
{
    const variables_map variables{};
    BOOST_REQUIRE(!is_configured(variables, "network.threads"));
}

BOOST_AUTO_TEST_CASE(setting__values__true__true_text)
{
    auto store = true;
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "true" }));
}

BOOST_AUTO_TEST_CASE(setting__values__false__false_text)
{
    auto store = false;
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "false" }));
}

BOOST_AUTO_TEST_CASE(setting__values__integer__decimal_text)
{
    uint32_t store = 42;
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "42" }));
}

BOOST_AUTO_TEST_CASE(setting__values__empty_string__empty_element)
{
    std::string store{};
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "" }));
}

BOOST_AUTO_TEST_CASE(setting__values__empty_path__empty_element)
{
    std::filesystem::path store{};
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "" }));
}

BOOST_AUTO_TEST_CASE(setting__values__path__unquoted_text)
{
    std::filesystem::path store{ "bitcoin.cfg" };
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "bitcoin.cfg" }));
}

BOOST_AUTO_TEST_CASE(setting__values__empty_vector__empty)
{
    string_list store{};
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{}));
}

BOOST_AUTO_TEST_CASE(setting__values__vector__element_each)
{
    string_list store{ "alpha", "beta" };
    const std::unique_ptr<const printable> instance{ setting(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "alpha", "beta" }));
}

BOOST_AUTO_TEST_CASE(secret__values__set_scalar__one_empty_value)
{
    std::string store{ "password" };
    const std::unique_ptr<const printable> instance{ secret(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "" }));
}

BOOST_AUTO_TEST_CASE(secret__values__unset_scalar__one_empty_value)
{
    std::string store{};
    const std::unique_ptr<const printable> instance{ secret(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "" }));
}

BOOST_AUTO_TEST_CASE(secret__values__set_collection__one_empty_value)
{
    string_list store{ "user:pass", "other:pass" };
    const std::unique_ptr<const printable> instance{ secret(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "" }));
}

BOOST_AUTO_TEST_CASE(secret__values__unset_collection__empty)
{
    string_list store{};
    const std::unique_ptr<const printable> instance{ secret(&store) };
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{}));
}

BOOST_AUTO_TEST_CASE(setting__values__mutated_store__current_value)
{
    uint32_t store = 1;
    const std::unique_ptr<const printable> instance{ setting(&store) };
    store = 2;
    BOOST_REQUIRE_EQUAL(instance->values(), (string_list{ "2" }));
}

BOOST_AUTO_TEST_SUITE_END()
