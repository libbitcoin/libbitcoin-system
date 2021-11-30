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
#include <bitcoin/system/chain/header.hpp>

#include <chrono>
#include <cstddef>
#include <utility>
#include <boost/thread.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/compact.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Use system clock because we require accurate time of day.
using wall_clock = std::chrono::system_clock;

// Constructors.
// ----------------------------------------------------------------------------

header::header()
  : header(0, {}, {}, 0, 0, 0, false)
{
}

header::header(header&& other)
  : header(
      other.version_,
      std::move(other.previous_block_hash_),
      std::move(other.merkle_root_),
      other.timestamp_,
      other.bits_,
      other.nonce_,
      other.valid_)
{
}

header::header(const header& other)
: header(
    other.version_,
    other.previous_block_hash_,
    other.merkle_root_,
    other.timestamp_,
    other.bits_,
    other.nonce_,
    other.valid_)
{
}

header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce)
  : header(version, std::move(previous_block_hash), std::move(merkle_root),
      timestamp, bits, nonce, true)
{
}

header::header(uint32_t version, const hash_digest& previous_block_hash,
    const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce)
  : header(version, previous_block_hash, merkle_root, timestamp, bits, nonce,
      true)
{
}

header::header(const data_chunk& data)
{
    from_data(data);
}

header::header(std::istream& stream)
{
    from_data(stream);
}

header::header(reader& source)
{
    from_data(source);
}

// protected
header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
    uint32_t nonce, bool valid)
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
    uint32_t nonce, bool valid)
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

header& header::operator=(header&& other)
{
    version_ = other.version_;
    previous_block_hash_ = std::move(other.previous_block_hash_);
    merkle_root_ = std::move(other.merkle_root_);
    timestamp_ = other.timestamp_;
    bits_ = other.bits_;
    nonce_ = other.nonce_;
    valid_ = other.valid_;
    return *this;
}

header& header::operator=(const header& other)
{
    version_ = other.version_;
    previous_block_hash_ = other.previous_block_hash_;
    merkle_root_ = other.merkle_root_;
    timestamp_ = other.timestamp_;
    bits_ = other.bits_;
    nonce_ = other.nonce_;
    valid_ = other.valid_;
    return *this;
}

bool header::operator==(const header& other) const
{
    return (version_ == other.version_)
        && (previous_block_hash_ == other.previous_block_hash_)
        && (merkle_root_ == other.merkle_root_)
        && (timestamp_ == other.timestamp_)
        && (bits_ == other.bits_)
        && (nonce_ == other.nonce_);
}

bool header::operator!=(const header& other) const
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

bool header::from_data(const data_chunk& data)
{
    read::bytes::copy reader(data);
    return from_data(reader);
}

bool header::from_data(std::istream& stream)
{
    read::bytes::istream reader(stream);
    return from_data(reader);
}

bool header::from_data(reader& source)
{
    reset();

    version_ = source.read_4_bytes_little_endian();
    previous_block_hash_ = source.read_hash();
    merkle_root_ = source.read_hash();
    timestamp_ = source.read_4_bytes_little_endian();
    bits_ = source.read_4_bytes_little_endian();
    nonce_ = source.read_4_bytes_little_endian();

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// protected
void header::reset()
{
    version_ = 0;
    previous_block_hash_.fill(0);
    merkle_root_.fill(0);
    timestamp_ = 0;
    bits_ = 0;
    nonce_ = 0;
    valid_ = false;
}

bool header::is_valid() const
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk header::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void header::to_data(std::ostream& stream) const
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void header::to_data(writer& sink) const
{
    DEBUG_ONLY(const auto size = serialized_size();)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_4_bytes_little_endian(version_);
    sink.write_bytes(previous_block_hash_);
    sink.write_bytes(merkle_root_);
    sink.write_4_bytes_little_endian(timestamp_);
    sink.write_4_bytes_little_endian(bits_);
    sink.write_4_bytes_little_endian(nonce_);

    BITCOIN_ASSERT(sink && sink.get_position() - start == size);
}

// static
size_t header::serialized_size()
{
    return sizeof(version_)
        + hash_size
        + hash_size
        + sizeof(timestamp_)
        + sizeof(bits_)
        + sizeof(nonce_);
}

// Properties.
// ----------------------------------------------------------------------------

uint32_t header::version() const
{
    return version_;
}

const hash_digest& header::previous_block_hash() const
{
    return previous_block_hash_;
}

const hash_digest& header::merkle_root() const
{
    return merkle_root_;
}

uint32_t header::timestamp() const
{
    return timestamp_;
}

uint32_t header::bits() const
{
    return bits_;
}

uint32_t header::nonce() const
{
    return nonce_;
}

// computed
hash_digest header::hash() const
{
    return bitcoin_hash(to_data());
}

// static/private
uint256_t header::difficulty(uint32_t bits)
{
    const auto header_bits = compact(bits);

    if (header_bits.is_overflowed())
        return zero;

    uint256_t target(header_bits);

    //*************************************************************************
    // CONSENSUS: satoshi will throw division by zero in the case where the
    // target is (2^256)-1 as the overflow will result in a zero divisor.
    // While actually achieving this work is improbable, this method operates
    // on a public method and therefore must be guarded.
    //*************************************************************************
    const auto divisor = add1(target);

    // We need to compute 2**256 / (target + 1), but we can't represent 2**256
    // as it's too large for uint256. However as 2**256 is at least as large as
    // target + 1, it is equal to ((2**256 - target - 1) / (target + 1)) + 1, or
    // (~target / (target + 1)) + 1.
    return is_zero(divisor) ? zero : add1(~target / divisor);
}

// computed
uint256_t header::difficulty() const
{
    return difficulty(bits_);
}

// Check.
// ----------------------------------------------------------------------------

bool header::is_invalid_proof_of_work(uint32_t proof_of_work_limit,
    bool scrypt) const
{
    const auto bits = compact(bits_);
    static const uint256_t pow_limit(compact{ proof_of_work_limit });

    if (bits.is_overflowed())
        return true;

    uint256_t target(bits);

    // Ensure claimed work is within limits.
    if (target < one || target > pow_limit)
        return true;

    // Conditionally use scrypt proof of work (e.g. Litecoin).
    // Ensure actual work is at least claimed amount (smaller is more work).
    return to_uint256(scrypt ? scrypt_hash(to_data()) : hash()) > target;
}

// ****************************************************************************
/// CONSENSUS: bitcoin 32bit unix time: en.wikipedia.org/wiki/Year_2038_problem
// ****************************************************************************
bool header::is_invalid_timestamp(uint32_t timestamp_limit_seconds) const
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
    uint32_t proof_of_work_limit, bool scrypt) const
{
    if (is_invalid_proof_of_work(proof_of_work_limit, scrypt))
        return error::invalid_proof_of_work;

    if (is_invalid_timestamp(timestamp_limit_seconds))
        return error::futuristic_timestamp;

    return error::success;
}

code header::accept(const chain_state& state) const
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

} // namespace chain
} // namespace system
} // namespace libbitcoin
