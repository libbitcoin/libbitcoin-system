/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <chrono>

BOOST_AUTO_TEST_SUITE(chain_header_tests)

using namespace system::chain;

const auto hash1 = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
const auto hash2 = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
const header expected_header
{
    10,
    hash1,
    hash2,
    531234,
    6523454,
    68644
};

// Access protected validation methods.
class accessor
  : public header
{
public:
    // Use base class constructors.
    using header::header;

    bool is_invalid_proof_of_work(uint32_t proof_of_work_limit,
        bool scrypt=false) const
    {
        return header::is_invalid_proof_of_work(proof_of_work_limit, scrypt);
    }

    bool is_invalid_timestamp(uint32_t timestamp_limit_seconds) const
    {
        return header::is_invalid_timestamp(timestamp_limit_seconds);
    }
};

// constructors
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(header__constructor__default__invalid)
{
    header instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header__constructor__move__expected)
{
    header copy(expected_header);
    const header instance(std::move(copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__constructor__copy__expected)
{
    const header instance(expected_header);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__constructor__move_parameters__expected)
{
    const uint32_t version = 10;
    const auto previous = hash1;
    const auto merkle = hash2;
    const uint32_t timestamp = 531234;
    const uint32_t bits = 6523454;
    const uint32_t nonce = 68644;

    auto merkle_copy = merkle;
    auto previous_copy = previous;
    header instance(version, std::move(previous_copy), std::move(merkle_copy), timestamp, bits, nonce);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.version(), version);
    BOOST_REQUIRE_EQUAL(instance.previous_block_hash(), previous);
    BOOST_REQUIRE_EQUAL(instance.merkle_root(), merkle);
    BOOST_REQUIRE_EQUAL(instance.timestamp(), timestamp);
    BOOST_REQUIRE_EQUAL(instance.bits(), bits);
    BOOST_REQUIRE_EQUAL(instance.nonce(), nonce);
}

BOOST_AUTO_TEST_CASE(header__constructor__copy_parameters__expected)
{
    const uint32_t version = 10;
    const auto previous = hash1;
    const auto merkle = hash2;
    const uint32_t timestamp = 531234;
    const uint32_t bits = 6523454;
    const uint32_t nonce = 68644;

    header instance(version, previous, merkle, timestamp, bits, nonce);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.version(), version);
    BOOST_REQUIRE_EQUAL(instance.previous_block_hash(), previous);
    BOOST_REQUIRE_EQUAL(instance.merkle_root(), merkle);
    BOOST_REQUIRE_EQUAL(instance.timestamp(), timestamp);
    BOOST_REQUIRE_EQUAL(instance.bits(), bits);
    BOOST_REQUIRE_EQUAL(instance.nonce(), nonce);
}

BOOST_AUTO_TEST_CASE(header__constructor__data__expected)
{
    const auto data = expected_header.to_data();
    const header instance(data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__constructor__stream__expected)
{
    const auto data = expected_header.to_data();
    stream::in::copy stream(data);
    const header instance(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__constructor__reader__expected)
{
    const auto data = expected_header.to_data();
    read::bytes::copy source(data);
    const header instance(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(header__assign__move__expected)
{
    const auto& alpha = expected_header;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(header__assign__copy__expected)
{
    const auto& alpha = expected_header;
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(header__equality__same__true)
{
    header instance(expected_header);
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__equality__different__false)
{
    header instance;
    BOOST_REQUIRE(!(instance == expected_header));
}

BOOST_AUTO_TEST_CASE(header__inequality__same__true)
{
    header instance(expected_header);
    BOOST_REQUIRE(!(instance != expected_header));
}

BOOST_AUTO_TEST_CASE(header__inequality__different__false)
{
    header instance;
    BOOST_REQUIRE(instance != expected_header);
}

// from_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(header__from_data__insufficient_bytes__invalid)
{
    data_chunk data(10);
    header instance;
    BOOST_REQUIRE(!instance.from_data(data));
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header__from_data__data__valid)
{
    const auto data = expected_header.to_data();
    const header instance(data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__from_data__stream__valid)
{
    const auto data = expected_header.to_data();
    stream::in::copy stream(data);
    const header instance(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

BOOST_AUTO_TEST_CASE(header__from_data__reader__valid)
{
    const auto data = expected_header.to_data();
    read::bytes::copy source(data);
    const header instance(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_header);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(header__to_data__data__expected)
{
    const auto data = expected_header.to_data();
    BOOST_REQUIRE_EQUAL(expected_header.serialized_size(), data.size());
}

BOOST_AUTO_TEST_CASE(header__to_data__stream__expected)
{
    // Write header to stream.
    std::stringstream iostream;
    expected_header.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const header copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_header);
}

BOOST_AUTO_TEST_CASE(header__to_data__writer__expected)
{
    // Write header to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_header.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const header copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_header);
}

// properties
// ----------------------------------------------------------------------------

// hash

BOOST_AUTO_TEST_CASE(header__difficulty__genesis_block__expected)
{
    const auto block = settings(selection::mainnet).genesis_block;
    BOOST_REQUIRE_EQUAL(block.header().difficulty(), 0x0000000100010001);
}

// validation (public)
// ----------------------------------------------------------------------------

// check
// accept

// validation (protected)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(header__is_invalid_proof_of_work__bits_exceeds_maximum__true)
{
    const settings settings(selection::mainnet);
    const accessor instance{ {}, {}, {}, {}, add1(settings.proof_of_work_limit), {} };
    BOOST_REQUIRE(instance.is_invalid_proof_of_work(settings.proof_of_work_limit, false));
}

BOOST_AUTO_TEST_CASE(header__is_invalid_proof_of_work__hash_greater_bits__true)
{
    const settings settings(selection::mainnet);
    const accessor instance
    {
        11234,
        base16_hash("abababababababababababababababababababababababababababababababab"),
        base16_hash("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234,
        0,
        34564
    };

    BOOST_REQUIRE(instance.is_invalid_proof_of_work(settings.proof_of_work_limit, false));
}

BOOST_AUTO_TEST_CASE(header__is_invalid_proof_of_work__hash_less_than_bits_false)
{
    const settings settings(selection::mainnet);
    const accessor instance
    {
        4,
        base16_hash("000000000000000003ddc1e929e2944b8b0039af9aa0d826c480a83d8b39c373"),
        base16_hash("a6cb0b0d6531a71abe2daaa4a991e5498e1b6b0b51549568d0f9d55329b905df"),
        1474388414,
        402972254,
        2842832236
    };

    BOOST_REQUIRE(!instance.is_invalid_proof_of_work(settings.proof_of_work_limit, false));
}

BOOST_AUTO_TEST_CASE(header__is_valid_scrypt_proof_of_work__hash_greater_than_bits__false)
{
    const settings settings(selection::mainnet);
    const accessor instance
    {
        536870912,
        base16_hash("abababababababababababababababababababababababababababababababab"),
        base16_hash("5163359dde15eb3f49cbd0926981f065ef1405fc9d4cece8818662b3b65f5dc6"),
        1535119178,
        436332170,
        2135224651
    };

    BOOST_REQUIRE(instance.is_invalid_proof_of_work(settings.proof_of_work_limit, true));
}

BOOST_AUTO_TEST_CASE(header__is_valid_scrypt_proof_of_work__hash_less_than_bits__false)
{
    const settings settings(selection::mainnet);
    const accessor instance
    {
        536870912,
        base16_hash("313ced849aafeff324073bb2bd31ecdcc365ed215a34e827bb797ad33d158542"),
        base16_hash("5163359dde15eb3f49cbd0926981f065ef1405fc9d4cece8818662b3b65f5dc6"),
        1535119178,
        436332170,
        2135224651
    };

    BOOST_REQUIRE(!instance.is_invalid_proof_of_work(settings.proof_of_work_limit, true));
    BOOST_REQUIRE(instance.is_invalid_proof_of_work(settings.proof_of_work_limit, false));
}

BOOST_AUTO_TEST_CASE(header__is_invalid_timestamp__timestamp_less_than_2_hours_from_now__false)
{
    const auto now = std::chrono::system_clock::now();
    const auto now_time = std::chrono::system_clock::to_time_t(now);
    const accessor instance{ {}, {}, {}, static_cast<uint32_t>(now_time), {}, {} };
    BOOST_REQUIRE(!instance.is_invalid_timestamp(settings().timestamp_limit_seconds));
}

BOOST_AUTO_TEST_CASE(header__is_invalid_timestamp__timestamp_greater_than_2_hours_from_now__true)
{
    const auto now = std::chrono::system_clock::now();
    const auto duration = std::chrono::hours(3);
    const auto future = std::chrono::system_clock::to_time_t(now + duration);
    const accessor instance{ {}, {}, {}, static_cast<uint32_t>(future), {}, {} };
    BOOST_REQUIRE(instance.is_invalid_timestamp(settings().timestamp_limit_seconds));
}

BOOST_AUTO_TEST_SUITE_END()
