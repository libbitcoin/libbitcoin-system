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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/header.hpp>

#include <chrono>
#include <utility>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

header header::factory_from_data(const data_chunk& data,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(data, with_transaction_count);
    return instance;
}

header header::factory_from_data(std::istream& stream,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(stream, with_transaction_count);
    return instance;
}

header header::factory_from_data(reader& source,
    bool with_transaction_count)
{
    header instance;
    instance.from_data(source, with_transaction_count);
    return instance;
}

uint64_t header::satoshi_fixed_size_without_transaction_count()
{
    return 80;
}

header::header()
  : header(0, null_hash, null_hash, 0, 0, 0, 0)
{
}

header::header(uint32_t version, const hash_digest& previous_block_hash,
    const hash_digest& merkle, uint32_t timestamp, uint32_t bits,
    uint32_t nonce, uint64_t transaction_count)
  : version_(version), previous_block_hash_(previous_block_hash),
    merkle_(merkle), timestamp_(timestamp), bits_(bits), nonce_(nonce),
    transaction_count_(transaction_count), hash_(nullptr)
{
}

header::header(uint32_t version, hash_digest&& previous_block_hash,
    hash_digest&& merkle, uint32_t timestamp, uint32_t bits, uint32_t nonce,
    uint64_t transaction_count)
  : version_(version), previous_block_hash_(std::move(previous_block_hash)),
    merkle_(std::move(merkle)), timestamp_(timestamp), bits_(bits),
    nonce_(nonce), transaction_count_(transaction_count), hash_(nullptr)
{
}

header::header(const header& other)
  : header(other.version_, other.previous_block_hash_, other.merkle_,
        other.timestamp_, other.bits_, other.nonce_, other.transaction_count_)
{
}

header::header(header&& other)
  : header(other.version_, std::move(other.previous_block_hash_),
      std::move(other.merkle_), other.timestamp_, other.bits_, other.nonce_,
      other.transaction_count_)
{
}

bool header::is_valid() const
{
    return (version_ != 0) ||
        (previous_block_hash_ != null_hash) ||
        (merkle_ != null_hash) ||
        (timestamp_ != 0) ||
        (bits_ != 0) ||
        (nonce_ != 0);
}

void header::reset()
{
    version_ = 0;
    previous_block_hash_.fill(0);
    merkle_.fill(0);
    timestamp_ = 0;
    bits_ = 0;
    nonce_ = 0;
    transaction_count_ = 0;

    mutex_.lock();
    hash_.reset();
    mutex_.unlock();
}

bool header::from_data(const data_chunk& data,
    bool with_transaction_count)
{
    data_source istream(data);
    return from_data(istream, with_transaction_count);
}

bool header::from_data(std::istream& stream, bool with_transaction_count)
{
    istream_reader source(stream);
    return from_data(source, with_transaction_count);
}

bool header::from_data(reader& source, bool with_transaction_count)
{
    reset();

    version_ = source.read_4_bytes_little_endian();
    previous_block_hash_ = source.read_hash();
    merkle_ = source.read_hash();
    timestamp_ = source.read_4_bytes_little_endian();
    bits_ = source.read_4_bytes_little_endian();
    nonce_ = source.read_4_bytes_little_endian();
    if (with_transaction_count)
        transaction_count_ = source.read_variable_uint_little_endian();

    const auto result = static_cast<bool>(source);

    if (!result)
        reset();

    return result;
}

data_chunk header::to_data(bool with_transaction_count) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream, with_transaction_count);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(with_transaction_count));
    return data;
}

void header::to_data(std::ostream& stream,
    bool with_transaction_count) const
{
    ostream_writer sink(stream);
    to_data(sink, with_transaction_count);
}

void header::to_data(writer& sink, bool with_transaction_count) const
{
    sink.write_4_bytes_little_endian(version_);
    sink.write_hash(previous_block_hash_);
    sink.write_hash(merkle_);
    sink.write_4_bytes_little_endian(timestamp_);
    sink.write_4_bytes_little_endian(bits_);
    sink.write_4_bytes_little_endian(nonce_);

    if (with_transaction_count)
        sink.write_variable_uint_little_endian(transaction_count_);
}

uint64_t header::serialized_size(bool with_transaction_count) const
{
    auto size = satoshi_fixed_size_without_transaction_count();

    if (with_transaction_count)
        size += variable_uint_size(transaction_count_);

    return size;
}

uint32_t header::version() const
{
    return version_;
}

void header::set_version(uint32_t value)
{
    version_ = value;
}

hash_digest& header::previous_block_hash()
{
    return previous_block_hash_;
}

const hash_digest& header::previous_block_hash() const
{
    return previous_block_hash_;
}

void header::set_previous_block_hash(const hash_digest& value)
{
    previous_block_hash_ = value;
}

void header::set_previous_block_hash(hash_digest&& value)
{
    previous_block_hash_ = std::move(value);
}

hash_digest& header::merkle()
{
    return merkle_;
}

const hash_digest& header::merkle() const
{
    return merkle_;
}

void header::set_merkle(const hash_digest& value)
{
    merkle_ = value;
}

void header::set_merkle(hash_digest&& value)
{
    merkle_ = std::move(value);
}

uint32_t header::timestamp() const
{
    return timestamp_;
}

void header::set_timestamp(uint32_t value)
{
    timestamp_ = value;
}

uint32_t header::bits() const
{
    return bits_;
}

void header::set_bits(uint32_t value)
{
    bits_ = value;
}

uint32_t header::nonce() const
{
    return nonce_;
}

void header::set_nonce(uint32_t value)
{
    nonce_ = value;
}

uint64_t header::transaction_count() const
{
    return transaction_count_;
}

void header::set_transaction_count(uint64_t value)
{
    transaction_count_ = value;
}

hash_digest header::hash() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (!hash_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        hash_.reset(new hash_digest(bitcoin_hash(to_data(false))));
        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    const auto hash = *hash_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
}

bool header::is_valid_time_stamp() const
{
    // Use system clock because we require accurate time of day.
    typedef std::chrono::system_clock wall_clock;
    static const auto two_hours = std::chrono::hours(time_stamp_future_hours);
    const auto time = wall_clock::from_time_t(timestamp_);
    const auto future = wall_clock::now() + two_hours;
    return time <= future;
}

bool header::is_valid_proof_of_work() const
{
    // TODO: This should be statically-initialized.
    hash_number maximum;
    if (!maximum.set_compact(max_work_bits))
        return false;

    hash_number target;
    if (!target.set_compact(bits_) || target > maximum)
        return false;

    hash_number value(hash());
    return value <= target;
}

code header::check() const
{
    if (!is_valid_proof_of_work())
        return error::invalid_proof_of_work;

    else if (!is_valid_time_stamp())
        return error::futuristic_timestamp;

    else
        return error::success;
}

code header::accept(const chain_state& state) const
{
    if (state.is_checkpoint_failure(hash()))
        return error::checkpoints_failed;

    else if (version_ < state.minimum_version())
        return error::old_version_block;

    else if (bits_ != state.work_required())
        return error::incorrect_proof_of_work;

    else if (timestamp_ <= state.median_time_past())
        return error::timestamp_too_early;

    else
        return error::success;
}

header& header::operator=(header&& other)
{
    version_ = other.version_;
    previous_block_hash_ = std::move(other.previous_block_hash_);
    merkle_ = std::move(other.merkle_);
    timestamp_ = other.timestamp_;
    bits_ = other.bits_;
    nonce_ = other.nonce_;
    transaction_count_ = other.transaction_count_;
    return *this;
}

// TODO: eliminate header copies and then delete this.
header& header::operator=(const header& other)
{
    version_ = other.version_;
    previous_block_hash_ = other.previous_block_hash_;
    merkle_ = other.merkle_;
    timestamp_ = other.timestamp_;
    bits_ = other.bits_;
    nonce_ = other.nonce_;
    transaction_count_ = other.transaction_count_;
    return *this;
}

bool header::operator==(const header& other) const
{
    return (version_ == other.version_)
        && (previous_block_hash_ == other.previous_block_hash_)
        && (merkle_ == other.merkle_)
        && (timestamp_ == other.timestamp_)
        && (bits_ == other.bits_)
        && (nonce_ == other.nonce_)
        && (transaction_count_ == other.transaction_count_);
}

bool header::operator!=(const header& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
