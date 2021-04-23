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
#include <bitcoin/system/wallet/witness_address.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <boost/program_options.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/formats/base_32.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/ec_private.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

using namespace bc::system::machine;

constexpr char separator = '1';
constexpr uint8_t version_0 = 0;
constexpr uint8_t version_maximum = 16;
constexpr uint8_t version_invalid = max_uint8;

// The prefix is factored into the checksum but not the base32 encoding.
const std::string witness_address::mainnet = "bc";
const std::string witness_address::testnet = "tb";

const char witness_address::prefix_minimum_character = 33;
const char witness_address::prefix_maximum_character = 126;
const size_t witness_address::prefix_minimum_length = 1;
const size_t witness_address::address_maximum_length = 90;
const size_t witness_address::program_minimum_size = 2;
const size_t witness_address::program_maximum_size = 40;
const size_t witness_address::version_0_p2kh_program_size = 20;
const size_t witness_address::version_0_p2sh_program_size = 32;
const size_t witness_address::checksum_length = 6;

// Contructors.
// ----------------------------------------------------------------------------

// invalid
witness_address::witness_address()
  : program_(),
    prefix_(),
    identifier_(program_type::invalid),
    version_(version_invalid)
{
}

// move
witness_address::witness_address(witness_address&& other)
  : program_(std::move(other.program_)),
    prefix_(other.prefix_),
    identifier_(other.identifier_),
    version_(other.version_)
{
}

// copy
witness_address::witness_address(const witness_address& other)
  : program_(other.program_),
    prefix_(other.prefix_),
    identifier_(other.identifier_),
    version_(other.version_)
{
}

// fully specified by address
witness_address::witness_address(const std::string& address)
  : witness_address(from_address(address))
{
}

// version_0_p2kh
witness_address::witness_address(const short_hash& public_key_hash,
    const std::string& prefix)
  : witness_address(from_short(public_key_hash, prefix))
{
}

// version_0_p2kh
witness_address::witness_address(const ec_private& secret,
    const std::string& prefix)
  : witness_address(from_private(secret, prefix))
{
}

// version_0_p2kh
witness_address::witness_address(const ec_public& point,
    const std::string& prefix)
  : witness_address(from_public(point, prefix))
{
}

// version_0_p2sh
witness_address::witness_address(const hash_digest& script_hash,
    const std::string& prefix)
  : witness_address(from_long(script_hash, prefix))
{
}

// version_0_p2sh
witness_address::witness_address(const chain::script& script,
    const std::string& prefix)
  : witness_address(from_script(script, prefix))
{
}

// private
witness_address::witness_address(data_chunk&& program, program_type identifier,
    const std::string& prefix, uint8_t version)
  : program_(std::move(program)),
    prefix_(prefix),
    identifier_(identifier),
    version_(version)
{
}

// Helpers.
// ----------------------------------------------------------------------------

// Lower case required for creation but not validation.
bool witness_address::is_valid_prefix(const std::string& prefix)
{
    if (is_ascii_mixed_case(prefix))
        return false;

    if (prefix.length() < prefix_minimum_length)
        return false;

    // One separator and one version byte are required.
    if (prefix.length() > (address_maximum_length - checksum_length - 2))
        return false;

    return (std::none_of(prefix.begin(), prefix.end(), [](const char character)
    {
        return character < prefix_minimum_character ||
            character > prefix_maximum_character;
    }));
}

// Enforces only address structural conformance, all future versions are valid.
// Version is exactly 1 char but encoded into other program and checksum data.
// Limits: [[prefix:1-][separator:1]([version:1][program:0-][checksum:6])]:-90.
bool witness_address::parse_address(std::string& out_prefix,
    uint8_t& out_version, data_chunk& out_program,
    const std::string& address)
{
    if (address.length() > address_maximum_length)
        return false;

    if (is_ascii_mixed_case(address))
        return false;

    const auto split = address.rfind(separator);
    if (split == address.npos)
        return false;

    // Split the parts and discard the separator character.
    out_prefix = address.substr(0, split - 1);
    const auto payload = address.substr(split + 1);

    if (!is_valid_prefix(out_prefix))
        return false;

    // The payload must contain at least the checksum and version byte.
    if (payload.length() < checksum_length + 1)
        return false;

    data_chunk data;
    if (!decode_base32(data, payload))
        return false;

    // Verify the bech32 checksum and extract version and program.
    return bech32_verify_checked(out_version, out_program, data, out_prefix);
}

witness_address::program_type witness_address::to_program_type(uint8_t version,
    const data_slice& program)
{
    if (version > version_maximum)
        return program_type::invalid;

    switch (version)
    {
        case version_0:
        {
            switch (program.size())
            {
                case version_0_p2kh_program_size:
                    return program_type::version_0_p2kh;
                case version_0_p2sh_program_size:
                    return program_type::version_0_p2sh;
                default:
                    return program_type::invalid;
            }
        }
        default:
        {
            return program_type::unknown;
        }
    }
}

// Factories.
// ----------------------------------------------------------------------------

// fully specified by address
witness_address witness_address::from_address(const std::string& address)
{
    std::string prefix;
    uint8_t version;
    data_chunk program;
    if (!parse_address(prefix, version, program, address))
        return {};

    const auto identifier = to_program_type(version, program);

    // Unknown programs are valid but output_script will return empty.
    if (identifier == program_type::invalid)
        return {};

    // Address is the only way to construct unknown program type addresses.
    return { std::move(program), identifier, prefix, version };
}

// version_0_p2kh
witness_address witness_address::from_short(const short_hash& hash,
    const std::string& prefix)
{
    // Constructed addresses must have lower case prefixes.
    if (!is_valid_prefix(prefix) || ascii_to_lower(prefix) != prefix)
        return {};

    // github.com/bitcoin/bips/blob/master/bip-0141.mediawiki
    // If the version byte is 0, and the witness program is 20 bytes it is
    // interpreted as a pay-to-witness-public-key-hash (P2WPKH) program.
    return { hash, program_type::version_0_p2kh, prefix, version_0 };
}

// version_0_p2kh
witness_address witness_address::from_private(const ec_private& secret,
    const std::string& prefix)
{
    if (!secret)
        return {};

    // Contruction from secret utilizes the public key.
    return from_public(secret, prefix);
}

// version_0_p2kh
witness_address witness_address::from_public(const ec_public& point,
    const std::string& prefix)
{
    if (!point)
        return {};

    // .point() is always the compressed public key, as required.
    return from_short(bitcoin_short_hash(point.point()), prefix);
}

// version_0_p2sh
witness_address witness_address::from_long(const hash_digest& hash,
    const std::string& prefix)
{
    // Constructed addresses must have lower case prefixes.
    if (!is_valid_prefix(prefix) || ascii_to_lower(prefix) != prefix)
        return {};

    // github.com/bitcoin/bips/blob/master/bip-0141.mediawiki
    // If the version byte is 0, and the witness program is 32 bytes it is
    // interpreted as a pay-to-witness-script-hash (P2WSH) program.
    return { hash, program_type::version_0_p2sh, prefix, version_0 };
}

// version_0_p2sh
witness_address witness_address::from_script(const chain::script& script,
    const std::string& prefix)
{
    // Disallow construction of a valid address for an invalid script.
    if (!script.is_valid())
        return {};

    // .to_payments_key() is the sha256 hash of the serialized script.
    return from_long(script.to_payments_key(), prefix);
}

// Cast operators.
// ----------------------------------------------------------------------------

witness_address::operator bool() const
{
    // The program may be unknown, in which case output_script returns empty.
    // Yet all other properties are valid and conformant, so unknown is valid.
    return identifier_ != program_type::invalid;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string witness_address::encoded() const
{
    const auto checked = bech32_build_checked(version_, program_, prefix_);
    return prefix_ + separator + encode_base32(checked);
}

// Properties.
// ----------------------------------------------------------------------------

const std::string& witness_address::prefix() const
{
    return prefix_;
}

// Version is simple numeric [0-16], not an opcode represenation.
uint8_t witness_address::version() const
{
    return version_;
}

const data_chunk& witness_address::program() const
{
    return program_;
}

witness_address::program_type witness_address::identifier() const
{
    return identifier_;
}

// Return constructed output script for known program types.
chain::script witness_address::output_script() const
{
    switch (identifier_)
    {
        case program_type::version_0_p2kh:
            return chain::script::to_pay_witness_key_hash_pattern(
                to_array<short_hash_size>(program_));
        case program_type::version_0_p2sh:
            return chain::script::to_pay_witness_script_hash_pattern(
                to_array<hash_size>(program_));
        case program_type::unknown:
        case program_type::invalid:
        default:
            return {};
    }
}

// Operators.
// ----------------------------------------------------------------------------

witness_address& witness_address::operator=(witness_address&& other)
{
    program_ = std::move(other.program_);
    prefix_ = other.prefix_;
    identifier_ = other.identifier_;
    version_ = other.version_;
    return *this;
}

witness_address& witness_address::operator=(const witness_address& other)
{
    program_ = other.program_;
    prefix_ = other.prefix_;
    identifier_ = other.identifier_;
    version_ = other.version_;
    return *this;
}

bool witness_address::operator<(const witness_address& other) const
{
    return encoded() < other.encoded();
}

bool operator==(const witness_address& left, const witness_address& right)
{
    return left.encoded() == right.encoded();
}

bool operator!=(const witness_address& left, const witness_address& right)
{
    return !(left == right);
}

std::istream& operator>>(std::istream& in, witness_address& to)
{
    std::string value;
    in >> value;
    to = witness_address(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const witness_address& of)
{
    out << of.encoded();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
