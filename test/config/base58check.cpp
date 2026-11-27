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
#include <sstream>

BC_AUTO_TEST_SUITE(base58check_tests)

using namespace bc::system::config;

// The compressed mainnet wif of secret one.
const std::string encoded{ "KwDiBf89QgGbjEhKnhXJuH7LrciVrZi3qYjgd9M7rFU73sVHnoWn" };
const auto decoded = base16_chunk("80000000000000000000000000000000000000000000000000000000000000000101");

BC_AUTO_TEST_CASE(base58check__constructor__default__does_not_throw)
{
    BC_REQUIRE_NO_THROW(base58check());
}

BC_AUTO_TEST_CASE(base58check__constructor__valid_string__decodes_without_checksum)
{
    const data_chunk instance(base58check{ encoded });
    BC_REQUIRE_EQUAL(instance, decoded);
}

BC_AUTO_TEST_CASE(base58check__construct__copy_chunk__expected)
{
    const config::base58check instance(decoded);
    BC_REQUIRE_EQUAL((const data_chunk&)instance, decoded);
}

BC_AUTO_TEST_CASE(base58check__construct__move_chunk__expected)
{
    auto value = decoded;
    const config::base58check instance(std::move(value));
    BC_REQUIRE_EQUAL((const data_chunk&)instance, decoded);
}

BC_AUTO_TEST_CASE(base58check__construct__invalid_string__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::base58check("0OIl"), istream_exception);
}

BC_AUTO_TEST_CASE(base58check__construct__bad_checksum__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::base58check("KwDiBf89QgGbjEhKnhXJuH7LrciVrZi3qYjgd9M7rFU73sVHnoWm"), istream_exception);
}

BC_AUTO_TEST_CASE(base58check__construct__shorter_than_checksum__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::base58check("1"), istream_exception);
}

BC_AUTO_TEST_CASE(base58check__stream__chunk__encoded)
{
    std::ostringstream output{};
    output << config::base58check(decoded);
    BC_REQUIRE_EQUAL(output.str(), encoded);
}

BC_AUTO_TEST_CASE(base58check__stream__round_trip__expected)
{
    const data_chunk value{ 0x01, 0x02, 0x03, 0x04 };
    const config::base58check instance(value);
    std::ostringstream output{};
    output << instance;
    config::base58check parsed{};
    std::istringstream(output.str()) >> parsed;
    BC_REQUIRE_EQUAL((const data_chunk&)parsed, value);
}

BC_AUTO_TEST_SUITE_END()
