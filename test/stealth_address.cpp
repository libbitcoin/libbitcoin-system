/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <sstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(stealth_address_tests)

BOOST_AUTO_TEST_CASE(stealth_address_test)
{
    ec_secret ephem_privkey{{
        95, 112, 167, 123, 50, 38, 10, 122, 50, 198, 34, 66, 56, 31, 186, 44,
        244, 12, 14, 32, 158, 102, 90, 121, 89, 65, 142, 174, 79, 45, 162,
        43}};
    ec_secret scan_privkey{{
        250, 99, 82, 30, 51, 62, 75, 159, 106, 152, 161, 66, 104, 13, 58, 239,
        77, 142, 127, 121, 114, 60, 224, 4, 54, 145, 219, 85, 195, 107, 217,
        5}};
    ec_secret spend_privkey{{
        220, 193, 37, 11, 81, 192, 240, 58, 228, 233, 120, 224, 37, 110, 222,
        81, 220, 17, 68, 227, 69, 201, 38, 38, 43, 151, 23, 177, 188, 201,
        189, 27}};
    BOOST_REQUIRE_EQUAL(ephem_privkey.size(), ec_secret_size);
    BOOST_REQUIRE_EQUAL(scan_privkey.size(), ec_secret_size);
    BOOST_REQUIRE_EQUAL(spend_privkey.size(), ec_secret_size);
    //ec_secret c{{
    //    75,73,116,38,110,230,200,190,217,239,242,205,16,135,187,193,16,31,23,
    //    186,217,195,120,20,248,86,27,103,245,80,197,68}};

    ec_point scan_pubkey = secret_to_public_key(scan_privkey);
    BOOST_REQUIRE_EQUAL(scan_pubkey.size(), ec_compressed_size);
    ec_point spend_pubkey = secret_to_public_key(spend_privkey);
    BOOST_REQUIRE_EQUAL(spend_pubkey.size(), ec_compressed_size);
    ec_point ephem_pubkey = secret_to_public_key(ephem_privkey);
    BOOST_REQUIRE_EQUAL(ephem_pubkey.size(), ec_compressed_size);

    ec_point sender_pubkey = wallet::uncover_stealth(
        scan_pubkey, ephem_privkey, spend_pubkey);
    ec_point receiver_pubkey = wallet::uncover_stealth(
        ephem_pubkey, scan_privkey, spend_pubkey);
    BOOST_REQUIRE(sender_pubkey == receiver_pubkey);

    ec_secret stealth_secret = wallet::uncover_stealth_secret(
        ephem_pubkey, scan_privkey, spend_privkey);
    BOOST_REQUIRE(secret_to_public_key(stealth_secret) == sender_pubkey);

    // sx ec-add
    //   03d5b3853bbee336b551ff999b0b1d656e65a7649037ae0dcb02b3c4ff5f29e5be
    //   4b4974266ee6c8bed9eff2cd1087bbc1101f17bad9c37814f8561b67f550c544
    //   | sx ec-to-address
    // 1Gvq8pSTRocNLDyf858o4PL3yhZm5qQDgB

    wallet::payment_address payaddr;
    payaddr.set_public_key(sender_pubkey);

#ifdef ENABLE_TESTNET
    BOOST_REQUIRE_EQUAL(payaddr.to_string(), "mwSnRsXSEq3d7LTGqe7AtJYNqhATwHdhMb");
#else
    BOOST_REQUIRE_EQUAL(payaddr.to_string(), "1Gvq8pSTRocNLDyf858o4PL3yhZm5qQDgB");
#endif
}

BOOST_AUTO_TEST_CASE(stealth_address__encoding__scan_mainnet__round_trips)
{
    const std::string encoded =
        "vJmzLu29obZcUGXXgotapfQLUpz7dfnZpbr4xg1R75qctf8xaXAteRdi3ZUk3T2Z"
        "MSad5KyPbve7uyH6eswYAxLHRVSbWgNUeoGuXp";
    wallet::stealth_address address;
    address.from_string(encoded);
    BOOST_REQUIRE_EQUAL(address.to_string(), encoded);
}

BOOST_AUTO_TEST_CASE(stealth_address__encoding__scan_testnet__round_trips)
{
    const std::string encoded =
        "waPXhQwQE9tDugfgLkvpDs3dnkPx1RsfDjFt4zBq7EeWeATRHpyQpYrFZR8T4BQy"
        "91Vpvshm2TDER8b9ZryuZ8VSzz8ywzNzX8NqF4";
    wallet::stealth_address address;
    address.from_string(encoded);
    BOOST_REQUIRE_EQUAL(address.to_string(), encoded);
}

BOOST_AUTO_TEST_CASE(stealth_address__encoding__scan_pub_mainnet__round_trips)
{
    const std::string encoded =
        "hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i";
    wallet::stealth_address address;
    address.from_string(encoded);
    BOOST_REQUIRE_EQUAL(address.to_string(), encoded);
}

BOOST_AUTO_TEST_CASE(stealth_address__encoding__scan_pub_testnet__round_trip)
{
    const std::string encoded =
        "idPayBqZUpZH7Y5GTaoEyGxDsEmU377JUmhtqG8yoHCkfGfhnAHmGUJbL";
    wallet::stealth_address address;
    address.from_string(encoded);
    BOOST_REQUIRE_EQUAL(address.to_string(), encoded);
}

BOOST_AUTO_TEST_CASE(prefix_to_string__32_bits__expected_value)
{
    const data_chunk blocks{{ 0xba, 0xad, 0xf0, 0x0d }};
    const binary_type prefix(32, blocks);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(stream.str(), "10111010101011011111000000001101");
}

BOOST_AUTO_TEST_CASE(string_to_prefix__32_bits__expected_value)
{
    const data_chunk blocks{{ 0xba, 0xad, 0xf0, 0x0d }};
    const binary_type prefix("10111010101011011111000000001101");
    BOOST_REQUIRE(prefix.blocks() == blocks);
}

BOOST_AUTO_TEST_CASE(prefix_to_bytes__32_bits__expected_value)
{
    const data_chunk blocks{{ 0xba, 0xad, 0xf0, 0x0d }};
    const binary_type prefix(32, blocks);
    BOOST_REQUIRE(prefix.blocks() == blocks);
}

BOOST_AUTO_TEST_CASE(bytes_to_prefix__zero_bits__round_trips)
{
    const data_chunk bytes;
    const binary_type prefix(0, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 0u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 0u);
    BOOST_REQUIRE(stream.str().empty());
}

BOOST_AUTO_TEST_CASE(prefix_to_bytes__zero_bits__round_trips)
{
    const data_chunk blocks{{ 0x00, 0x00, 0x00, 0x00 }};
    const binary_type prefix(0, blocks);
    const auto bytes = prefix.blocks();
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 0u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 0u);
    BOOST_REQUIRE_EQUAL(bytes.size(), 0u);
    BOOST_REQUIRE(stream.str().empty());
}

BOOST_AUTO_TEST_CASE(bytes_to_prefix__one_bit__round_trips)
{
    data_chunk bytes{{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
    auto prefix = binary_type(1, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 1u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "1");
}

BOOST_AUTO_TEST_CASE(prefix_to_bytes__one_bit__round_trips)
{
    const data_chunk blocks{{ 0xff, 0xff, 0xff, 0xff }};
    const binary_type prefix(1, blocks);
    const auto bytes = prefix.blocks();
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 1u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 1u);
    BOOST_REQUIRE_EQUAL(bytes.size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "1");
}

BOOST_AUTO_TEST_CASE(bytes_to_prefix__two_bits_leading_zero__round_trips)
{
    const data_chunk bytes{{ 0x01, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42 }};
    const auto prefix = binary_type(2, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 2u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "00");
}

BOOST_AUTO_TEST_CASE(prefix_to_bytes__two_bits_leading_zero__round_trips)
{
    const data_chunk blocks{{ 0x42, 0x42, 0x42, 0x01 }};
    const binary_type prefix(2, blocks);
    auto bytes = prefix.blocks();
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 2u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 1u);
    BOOST_REQUIRE_EQUAL(bytes.size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "01");
}

BOOST_AUTO_TEST_CASE(bytes_to_prefix__two_bytes_leading_null_byte__round_trips)
{
    const data_chunk bytes{{ 0xFF, 0x00 }};
    const auto prefix = binary_type(16, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 16u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 2u);
    BOOST_REQUIRE_EQUAL(stream.str(), "1111111100000000");
}

BOOST_AUTO_TEST_CASE(prefix_to_bytes__two_bytes_leading_null_byte__round_trips)
{
    const data_chunk blocks{{ 0x00, 0x00 }};
    const binary_type prefix(16, blocks);
    auto bytes = prefix.blocks();
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.size(), 16u);
    BOOST_REQUIRE_EQUAL(prefix.blocks().size(), 2u);
    BOOST_REQUIRE_EQUAL(bytes.size(), 2u);
    BOOST_REQUIRE_EQUAL(stream.str(), "0000000000000000");
}

BOOST_AUTO_TEST_SUITE_END()
