/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <utility>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/compact.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Use system clock because we require accurate time of day.
using wall_clock = std::chrono::system_clock;

// Constructors.
// ----------------------------------------------------------------------------

header::header() NOEXCEPT
  : header(0, {}, {}, 0, 0, 0, false)
{
}

header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce) NOEXCEPT
  : header(version, std::move(previous_block_hash), std::move(merkle_root),
      timestamp, bits, nonce, true)
{
}

header::header(uint32_t version, const hash_digest& previous_block_hash,
    const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce) NOEXCEPT
  : header(version, previous_block_hash, merkle_root, timestamp, bits, nonce,
      true)
{
}

header::header(const data_slice& data) NOEXCEPT
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : header(stream::in::copy(data))
    BC_POP_WARNING()
{
}

header::header(stream::in::fast&& stream) NOEXCEPT
  : header(read::bytes::fast(stream))
{
}

header::header(stream::in::fast& stream) NOEXCEPT
  : header(read::bytes::fast(stream))
{
}

header::header(std::istream&& stream) NOEXCEPT
  : header(read::bytes::istream(stream))
{
}

header::header(std::istream& stream) NOEXCEPT
  : header(read::bytes::istream(stream))
{
}

header::header(reader&& source) NOEXCEPT
  : header(source)
{
}

header::header(reader& source) NOEXCEPT
{
    assign_data(source);
}

// protected
header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce, bool valid) NOEXCEPT
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
    uint32_t nonce, bool valid) NOEXCEPT
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

bool header::operator==(const header& other) const NOEXCEPT
{
    return (version_ == other.version_)
        && (previous_block_hash_ == other.previous_block_hash_)
        && (merkle_root_ == other.merkle_root_)
        && (timestamp_ == other.timestamp_)
        && (bits_ == other.bits_)
        && (nonce_ == other.nonce_);
}

bool header::operator!=(const header& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// private
void header::assign_data(reader& source) NOEXCEPT
{
    // Hashes are copied directly into to header-allocated space.
    // Integrals are stack-allocated and copied to header-allocated space.
    version_ = source.read_4_bytes_little_endian();
    source.read_bytes(previous_block_hash_.data(), hash_size);
    source.read_bytes(merkle_root_.data(), hash_size);
    timestamp_ = source.read_4_bytes_little_endian();
    bits_ = source.read_4_bytes_little_endian();
    nonce_ = source.read_4_bytes_little_endian();
    valid_ = source;
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk header::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size());
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out);
    return data;
}

void header::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void header::to_data(writer& sink) const NOEXCEPT
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

bool header::is_valid() const NOEXCEPT
{
    return valid_;
}

uint32_t header::version() const NOEXCEPT
{
    return version_;
}

const hash_digest& header::previous_block_hash() const NOEXCEPT
{
    return previous_block_hash_;
}

const hash_digest& header::merkle_root() const NOEXCEPT
{
    return merkle_root_;
}

uint32_t header::timestamp() const NOEXCEPT
{
    return timestamp_;
}

uint32_t header::bits() const NOEXCEPT
{
    return bits_;
}

uint32_t header::nonce() const NOEXCEPT
{
    return nonce_;
}

void header::set_hash(const hash_digest& hash) const NOEXCEPT
{
    hash_ = hash;
}

// computed
hash_digest header::hash() const NOEXCEPT
{
    if (hash_)
        return *hash_;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    to_data(sink);
    sink.flush();
    return digest;
}

const hash_digest& header::get_hash() const NOEXCEPT
{
    if (!hash_)
        set_hash(hash());

    return *hash_;
}

// static
uint256_t header::proof(uint32_t bits) NOEXCEPT
{
    auto target = compact::expand(bits);

    //*************************************************************************
    // CONSENSUS: bits may be overflowed, which is guarded here.
    // A target of zero is disallowed so is useful as a sentinel value.
    //*************************************************************************
    if (is_zero(target))
        return target;

    //*************************************************************************
    // CONSENSUS: If target is (2^256)-1, division would fail, however compact
    // compression is lossy, and therefore unable to produce negative one.
    //*************************************************************************

    // We need to compute 2**256 / (target + 1), but we can't represent 2**256
    // as it's too large for uint256. However as 2**256 is at least as large as
    // target + 1, it is equal to ((2**256 - target - 1) / (target + 1)) + 1, or
    // (~target / (target + 1)) + 1.
    return ++(~target / (target + one));
}

// computed
uint256_t header::proof() const NOEXCEPT
{
    // Returns zero if bits_ mantissa is less than one or bits_ is overflowed.
    return proof(bits_);
}

// Check.
// ----------------------------------------------------------------------------

bool header::is_invalid_proof_of_work(uint32_t proof_of_work_limit,
    bool scrypt) const NOEXCEPT
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
    return to_uintx(scrypt ? scrypt_hash(to_data()) : hash()) > target;
}

// ****************************************************************************
// CONSENSUS: bitcoin 32bit unix time: en.wikipedia.org/wiki/Year_2038_problem
// ****************************************************************************
bool header::is_invalid_timestamp(
    uint32_t timestamp_limit_seconds) const NOEXCEPT
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
    uint32_t proof_of_work_limit, bool scrypt) const NOEXCEPT
{
    if (is_invalid_proof_of_work(proof_of_work_limit, scrypt))
        return error::invalid_proof_of_work;
    if (is_invalid_timestamp(timestamp_limit_seconds))
        return error::futuristic_timestamp;

    return error::block_success;
}

// minimum_block_version
// median_time_past
// work_required

// Checkpoints and previous_block_hash are chain validation (not here).
// bits_ below is the consensus direct comparison of the header.bits value.
// All other work comparisons performed on expanded/normalized bits values.
code header::accept(const context& ctx) const NOEXCEPT
{
    if (version_ < ctx.minimum_block_version)
        return error::invalid_block_version;
    if (timestamp_ <= ctx.median_time_past)
        return error::timestamp_too_early;
    if (bits_ != ctx.work_required)
        return error::incorrect_proof_of_work;

    return error::block_success;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

header tag_invoke(json::value_to_tag<header>,
    const json::value& value) NOEXCEPT
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
    const header& tx) NOEXCEPT
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
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<header>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const header::cptr& tx) NOEXCEPT
{
    tag_invoke(tag, value, *tx);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
