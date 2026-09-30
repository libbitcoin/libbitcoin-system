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

// ungroup

BOOST_AUTO_TEST_CASE(setting__ungroup__grouped__ungrouped)
{
    BOOST_REQUIRE_EQUAL(ungroup("1,000"), "1000");
    BOOST_REQUIRE_EQUAL(ungroup("20,160"), "20160");
    BOOST_REQUIRE_EQUAL(ungroup("950,000"), "950000");
    BOOST_REQUIRE_EQUAL(ungroup("100,000,000"), "100000000");
}

BOOST_AUTO_TEST_CASE(setting__ungroup__signed__ungrouped)
{
    BOOST_REQUIRE_EQUAL(ungroup("-1,000"), "-1000");
    BOOST_REQUIRE_EQUAL(ungroup("+1,000"), "1000");
}

BOOST_AUTO_TEST_CASE(setting__ungroup__not_grouped__unchanged)
{
    BOOST_REQUIRE_EQUAL(ungroup(""), "");
    BOOST_REQUIRE_EQUAL(ungroup("950000"), "950000");
    BOOST_REQUIRE_EQUAL(ungroup("-42"), "-42");
    BOOST_REQUIRE_EQUAL(ungroup("text"), "text");
}

BOOST_AUTO_TEST_CASE(setting__ungroup__malformed__unchanged)
{
    BOOST_REQUIRE_EQUAL(ungroup(",000"), ",000");
    BOOST_REQUIRE_EQUAL(ungroup("1,00"), "1,00");
    BOOST_REQUIRE_EQUAL(ungroup("1,0000"), "1,0000");
    BOOST_REQUIRE_EQUAL(ungroup("1000,000"), "1000,000");
    BOOST_REQUIRE_EQUAL(ungroup("1,000,"), "1,000,");
    BOOST_REQUIRE_EQUAL(ungroup("1,000x"), "1,000x");
    BOOST_REQUIRE_EQUAL(ungroup("a,000"), "a,000");
    BOOST_REQUIRE_EQUAL(ungroup("1,a00"), "1,a00");
}

// xparse

BOOST_AUTO_TEST_CASE(setting__parse__grouped_integer__expected)
{
    uint32_t store{};
    const std::unique_ptr<const setting_value<uint32_t>> instance{ setting(&store) };
    boost::any value{};
    instance->parse(value, { "950,000" }, true);
    BOOST_REQUIRE_EQUAL(boost::any_cast<uint32_t>(value), 950000u);
}

BOOST_AUTO_TEST_CASE(setting__parse__malformed_grouping__throws)
{
    uint32_t store{};
    const std::unique_ptr<const setting_value<uint32_t>> instance{ setting(&store) };
    boost::any value{};
    BOOST_REQUIRE_THROW(instance->parse(value, { "95,0000" }, true), boost::program_options::error);
}

BOOST_AUTO_TEST_CASE(setting__parse__grouped_string__unchanged)
{
    std::string store{};
    const std::unique_ptr<const setting_value<std::string>> instance{ setting(&store) };
    boost::any value{};
    instance->parse(value, { "1,000" }, true);
    BOOST_REQUIRE_EQUAL(boost::any_cast<std::string>(value), "1,000");
}

BOOST_AUTO_TEST_SUITE_END()
