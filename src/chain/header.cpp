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
#include <bitcoin/system/chain/header.hpp>

#include <chrono>
#include <cstddef>
#include <utility>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/compact.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Use system clock because we require accurate time of day.
using wall_clock = std::chrono::system_clock;

// Constructors.
// ----------------------------------------------------------------------------

header::header() noexcept
  : header(0, {}, {}, 0, 0, 0, false)
{
}

header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce) noexcept
  : header(version, std::move(previous_block_hash), std::move(merkle_root),
      timestamp, bits, nonce, true)
{
}

header::header(uint32_t version, const hash_digest& previous_block_hash,
    const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce) noexcept
  : header(version, previous_block_hash, merkle_root, timestamp, bits, nonce,
      true)
{
}

header::header(const data_slice& data) noexcept
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : header(stream::in::copy(data))
    BC_POP_WARNING()
{
}

header::header(std::istream&& stream) noexcept
  : header(read::bytes::istream(stream))
{
}

header::header(std::istream& stream) noexcept
  : header(read::bytes::istream(stream))
{
}

header::header(reader&& source) noexcept
  : header(from_data(source))
{
}

header::header(reader& source) noexcept
  : header(from_data(source))
{
}

// protected
header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce, bool valid) noexcept
  : version_(version),
    previous_block_hash_(std::move(previous_block_hash)),
    merkle_root_(std::move(merkle_root)),
    timestamp_(timestamp),
    bits_(bits),
    nonce_(nonce),
    valid_(valid)
{
}

// protected
header::header(uint32_t version, const hash_digest& previous_block_hash,
    const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce, bool valid) noexcept
  : version_(version),
    previous_block_hash_(previous_block_hash),
    merkle_root_(merkle_root),
    timestamp_(timestamp),
    bits_(bits),
    nonce_(nonce),
    valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool header::operator==(const header& other) const noexcept
{
    return (version_ == other.version_)
        && (previous_block_hash_ == other.previous_block_hash_)
        && (merkle_root_ == other.merkle_root_)
        && (timestamp_ == other.timestamp_)
        && (bits_ == other.bits_)
        && (nonce_ == other.nonce_);
}

bool header::operator!=(const header& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
header header::from_data(reader& source) noexcept
{
    return
    {
        source.read_4_bytes_little_endian(),
        source.read_hash(),
        source.read_hash(),
        source.read_4_bytes_little_endian(),
        source.read_4_bytes_little_endian(),
        source.read_4_bytes_little_endian(),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk header::to_data() const noexcept
{
    data_chunk data(serialized_size(), no_fill_byte_allocator);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream);
    return data;
}

void header::to_data(std::ostream& stream) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void header::to_data(writer& sink) const noexcept
{
    sink.write_4_bytes_little_endian(version_);
    sink.write_bytes(previous_block_hash_);
    sink.write_bytes(merkle_root_);
    sink.write_4_bytes_little_endian(timestamp_);
    sink.write_4_bytes_little_endian(bits_);
    sink.write_4_bytes_little_endian(nonce_);
}

// Properties.
// ----------------------------------------------------------------------------

bool header::is_valid() const noexcept
{
    return valid_;
}

uint32_t header::version() const noexcept
{
    return version_;
}

const hash_digest& header::previous_block_hash() const noexcept
{
    return previous_block_hash_;
}

const hash_digest& header::merkle_root() const noexcept
{
    return merkle_root_;
}

uint32_t header::timestamp() const noexcept
{
    return timestamp_;
}

uint32_t header::bits() const noexcept
{
    return bits_;
}

uint32_t header::nonce() const noexcept
{
    return nonce_;
}

// computed
hash_digest header::hash() const noexcept
{
    hash_digest sha256{};
    hash::sha256::copy sink(sha256);
    to_data(sink);
    sink.flush();
    return sha256_hash(sha256);
}

// static/private
uint256_t header::difficulty(uint32_t bits) noexcept
{
    auto target = compact::expand(bits);

    //*************************************************************************
    // CONSENSUS: bits may be overflowed, which is guarded here.
    // A target of zero is disallowed so is useful as a sentinel value.
    //*************************************************************************
    if (is_zero(target))
        return target;

    //*************************************************************************
    // CONSENSUS: If target is (2^256)-1, division will fail, however compact
    // cannot expand bits into a value of (2^256)-1 (see comments in compact).
    //*************************************************************************

    // We need to compute 2**256 / (target + 1), but we can't represent 2**256
    // as it's too large for uint256. However as 2**256 is at least as large as
    // target + 1, it is equal to ((2**256 - target - 1) / (target + 1)) + 1, or
    // (~target / (target + 1)) + 1.
    return ++(bit_not(target) / add1(target));
}

// computed
uint256_t header::difficulty() const noexcept
{
    // Returns zero if bits_ mantissa is less than one or bits_ is overflowed.
    return difficulty(bits_);
}

// Check.
// ----------------------------------------------------------------------------

bool header::is_invalid_proof_of_work(uint32_t proof_of_work_limit,
    bool scrypt) const noexcept
{
    static const auto limit = compact::expand(proof_of_work_limit);
    const auto target = compact::expand(bits_);

    //*************************************************************************
    // CONSENSUS: bits_ may be overflowed, which is guarded here.
    // A target of zero is disallowed so is useful as a sentinel value.
    //*************************************************************************
    if (is_zero(target))
        return true;

    // Ensure claimed work is at or above minimum (less is more).
    if (target > limit)
        return true;

    // Conditionally use scrypt proof of work (e.g. Litecoin).
    return to_uint256(scrypt ? scrypt_hash(to_data()) : hash()) > target;
}

// ****************************************************************************
/// CONSENSUS: bitcoin 32bit unix time: en.wikipedia.org/wiki/Year_2038_problem
// ****************************************************************************
bool header::is_invalid_timestamp(
    uint32_t timestamp_limit_seconds) const noexcept
{
    using namespace std::chrono;
    static const auto two_hours = seconds(timestamp_limit_seconds);
    const auto time = wall_clock::from_time_t(timestamp_);
    const auto future = wall_clock::now() + two_hours;
    return time > future;
}

// Validation.
// ----------------------------------------------------------------------------

code header::check(uint32_t timestamp_limit_seconds,
    uint32_t proof_of_work_limit, bool scrypt) const noexcept
{
    if (is_invalid_proof_of_work(proof_of_work_limit, scrypt))
        return error::invalid_proof_of_work;

    if (is_invalid_timestamp(timestamp_limit_seconds))
        return error::futuristic_timestamp;

    return error::success;
}

code header::accept(const chain_state& state) const noexcept
{
    if (state.is_checkpoint_conflict(hash()))
        return error::checkpoints_failed;

    if (version_ < state.minimum_block_version())
        return error::invalid_block_version;

    if (timestamp_ <= state.median_time_past())
        return error::timestamp_too_early;

    if (bits_ != state.work_required())
        return error::incorrect_proof_of_work;

    return error::success;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

header tag_invoke(json::value_to_tag<header>,
    const json::value& value) noexcept
{
    hash_digest previous, merkle_root;
    if (!decode_hash(previous, value.at("previous").get_string().c_str()) ||
        !decode_hash(merkle_root, value.at("merkle_root").get_string().c_str()))
        return {};

    return
    {
        value.at("version").to_number<uint32_t>(),
        previous,
        merkle_root,
        value.at("timestamp").to_number<uint32_t>(),
        value.at("bits").to_number<uint32_t>(),
        value.at("nonce").to_number<uint32_t>()
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const header& tx) noexcept
{
    value =
    {
        { "version", tx.version() },
        { "previous", encode_hash(tx.previous_block_hash()) },
        { "merkle_root", encode_hash(tx.merkle_root()) },
        { "timestamp", tx.timestamp() },
        { "bits", tx.bits() },
        { "nonce", tx.nonce() }
    };
}

BC_POP_WARNING()

header::cptr tag_invoke(json::value_to_tag<header::cptr>,
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<header>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const header::cptr& tx) noexcept
{
    tag_invoke(tag, value, *tx);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
