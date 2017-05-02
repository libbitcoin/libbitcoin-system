/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/transaction.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <type_traits>
#include <sstream>
#include <utility>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/input.hpp>
#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/machine/rule_fork.hpp>
#include <bitcoin/bitcoin/message/messages.hpp>
#include <bitcoin/bitcoin/utility/collection.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/timer.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::machine;

// Read a length-prefixed collection of inputs or outputs from the source.
template<class Source, class Put>
bool read(Source& source, std::vector<Put>& puts, bool wire)
{
    auto result = true;
    const auto count = source.read_size_little_endian();

    // Guard against potential for arbitary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        puts.resize(count);

    const auto deserialize = [&result, &source, wire](Put& put)
    {
        result = result && put.from_data(source, wire);
    };

    std::for_each(puts.begin(), puts.end(), deserialize);
    return result;
}

// Write a length-prefixed collection of inputs or outputs to the sink.
template<class Sink, class Put>
void write(Sink& sink, const std::vector<Put>& puts, bool wire)
{
    sink.write_variable_little_endian(puts.size());

    const auto serialize = [&sink, wire](const Put& put)
    {
        put.to_data(sink, wire);
    };

    std::for_each(puts.begin(), puts.end(), serialize);
}

// Constructors.
//-----------------------------------------------------------------------------

transaction::transaction()
  : version_(0), locktime_(0), validation{}
{
}

transaction::transaction(transaction&& other)
  : transaction(other.version_, other.locktime_, std::move(other.inputs_),
      std::move(other.outputs_))
{
    // TODO: implement safe private accessor for conditional cache transfer.
    validation = std::move(other.validation);
}

transaction::transaction(const transaction& other)
  : transaction(other.version_, other.locktime_, other.inputs_, other.outputs_)
{
    // TODO: implement safe private accessor for conditional cache transfer.
    validation = other.validation;
}

transaction::transaction(transaction&& other, hash_digest&& hash)
  : transaction(other.version_, other.locktime_, std::move(other.inputs_),
        std::move(other.outputs_))
{
    hash_ = std::make_shared<hash_digest>(std::move(hash));
    validation = std::move(other.validation);
}

transaction::transaction(const transaction& other, const hash_digest& hash)
  : transaction(other.version_, other.locktime_, other.inputs_, other.outputs_)
{
    hash_ = std::make_shared<hash_digest>(hash);
    validation = other.validation;
}

transaction::transaction(uint32_t version, uint32_t locktime,
    const input::list& inputs, const output::list& outputs)
  : version_(version),
    locktime_(locktime),
    inputs_(inputs),
    outputs_(outputs),
    validation{}
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    input::list&& inputs, output::list&& outputs)
  : version_(version),
    locktime_(locktime),
    inputs_(std::move(inputs)),
    outputs_(std::move(outputs)),
    validation{}
{
}

// Operators.
//-----------------------------------------------------------------------------

transaction& transaction::operator=(transaction&& other)
{
    // TODO: implement safe private accessor for conditional cache transfer.
    version_ = other.version_;
    locktime_ = other.locktime_;
    inputs_ = std::move(other.inputs_);
    outputs_ = std::move(other.outputs_);
    validation = std::move(other.validation);
    return *this;
}

// TODO: eliminate blockchain transaction copies and then delete this.
transaction& transaction::operator=(const transaction& other)
{
    version_ = other.version_;
    locktime_ = other.locktime_;
    inputs_ = other.inputs_;
    outputs_ = other.outputs_;
    validation = other.validation;
    return *this;
}

bool transaction::operator==(const transaction& other) const
{
    return (version_ == other.version_)
        && (locktime_ == other.locktime_)
        && (inputs_ == other.inputs_)
        && (outputs_ == other.outputs_);
}

bool transaction::operator!=(const transaction& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
transaction transaction::factory_from_data(const data_chunk& data, bool wire)
{
    transaction instance;
    instance.from_data(data, wire);
    return instance;
}

// static
transaction transaction::factory_from_data(std::istream& stream, bool wire)
{
    transaction instance;
    instance.from_data(stream, wire);
    return instance;
}

// static
transaction transaction::factory_from_data(reader& source, bool wire)
{
    transaction instance;
    instance.from_data(source, wire);
    return instance;
}

bool transaction::from_data(const data_chunk& data, bool wire)
{
    data_source istream(data);
    return from_data(istream, wire);
}

bool transaction::from_data(std::istream& stream, bool wire)
{
    istream_reader source(stream);
    return from_data(source, wire);
}

bool transaction::from_data(reader& source, bool wire)
{
    reset();

    if (wire)
    {
        // Wire (satoshi protocol) deserialization.
        version_ = source.read_4_bytes_little_endian();
        read(source, inputs_, wire) && read(source, outputs_, wire);
        locktime_ = source.read_4_bytes_little_endian();
    }
    else
    {
        // Database (outputs forward) serialization.
        read(source, outputs_, wire) && read(source, inputs_, wire);
        const auto locktime = source.read_variable_little_endian();
        const auto version = source.read_variable_little_endian();

        if (locktime > max_uint32 || version > max_uint32)
            source.invalidate();

        locktime_ = static_cast<uint32_t>(locktime);
        version_ = static_cast<uint32_t>(version);
    }

    if (!source)
        reset();

    return source;
}

// protected
void transaction::reset()
{
    version_ = 0;
    locktime_ = 0;
    inputs_.clear();
    inputs_.shrink_to_fit();
    outputs_.clear();
    outputs_.shrink_to_fit();
    invalidate_cache();
    total_input_value_ = boost::none;
    total_output_value_ = boost::none;
}

bool transaction::is_valid() const
{
    return (version_ != 0) || (locktime_ != 0) || !inputs_.empty() ||
        !outputs_.empty();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk transaction::to_data(bool wire) const
{
    data_chunk data;
    const auto size = serialized_size(wire);

    // Reserve an extra byte to prevent full reallocation in the case of
    // generate_signature_hash extension by addition of the sighash_type.
    data.reserve(size + sizeof(uint8_t));

    data_sink ostream(data);
    to_data(ostream, wire);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void transaction::to_data(std::ostream& stream, bool wire) const
{
    ostream_writer sink(stream);
    to_data(sink, wire);
}

void transaction::to_data(writer& sink, bool wire) const
{
    if (wire)
    {
        // Wire (satoshi protocol) serialization.
        sink.write_4_bytes_little_endian(version_);
        write(sink, inputs_, wire);
        write(sink, outputs_, wire);
        sink.write_4_bytes_little_endian(locktime_);
    }
    else
    {
        // Database (outputs forward) serialization.
        write(sink, outputs_, wire);
        write(sink, inputs_, wire);
        sink.write_variable_little_endian(locktime_);
        sink.write_variable_little_endian(version_);
    }
}

// Size.
//-----------------------------------------------------------------------------

size_t transaction::serialized_size(bool wire) const
{
    const auto ins = [wire](size_t size, const input& input)
    {
        return size + input.serialized_size(wire);
    };

    const auto outs = [wire](size_t size, const output& output)
    {
        return size + output.serialized_size(wire);
    };

    return (wire ? sizeof(version_) : message::variable_uint_size(version_))
        + (wire ? sizeof(locktime_) : message::variable_uint_size(locktime_))
        + message::variable_uint_size(inputs_.size())
        + message::variable_uint_size(outputs_.size())
        + std::accumulate(inputs_.begin(), inputs_.end(), size_t{0}, ins)
        + std::accumulate(outputs_.begin(), outputs_.end(), size_t{0}, outs);
}

// Accessors.
//-----------------------------------------------------------------------------

uint32_t transaction::version() const
{
    return version_;
}

void transaction::set_version(uint32_t value)
{
    version_ = value;
    invalidate_cache();
}

uint32_t transaction::locktime() const
{
    return locktime_;
}

void transaction::set_locktime(uint32_t value)
{
    locktime_ = value;
    invalidate_cache();
}

input::list& transaction::inputs()
{
    return inputs_;
}

const input::list& transaction::inputs() const
{
    return inputs_;
}

void transaction::set_inputs(const input::list& value)
{
    inputs_ = value;
    invalidate_cache();
    total_input_value_ = boost::none;
}

void transaction::set_inputs(input::list&& value)
{
    inputs_ = std::move(value);
    invalidate_cache();
    total_input_value_ = boost::none;
}

output::list& transaction::outputs()
{
    return outputs_;
}

const output::list& transaction::outputs() const
{
    return outputs_;
}

void transaction::set_outputs(const output::list& value)
{
    outputs_ = value;
    invalidate_cache();
    total_output_value_ = boost::none;
}

void transaction::set_outputs(output::list&& value)
{
    outputs_ = std::move(value);
    invalidate_cache();
    total_output_value_ = boost::none;
}

// Cache.
//-----------------------------------------------------------------------------

void transaction::invalidate_cache() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (hash_)
    {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_.reset();
        //---------------------------------------------------------------------
        mutex_.unlock_and_lock_upgrade();
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

hash_digest transaction::hash() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (!hash_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        hash_ = std::make_shared<hash_digest>(bitcoin_hash(to_data(true)));
        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    const auto hash = *hash_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
}

hash_digest transaction::hash(uint32_t sighash_type) const
{
    auto serialized = to_data(true);
    extend_data(serialized, to_little_endian(sighash_type));
    return bitcoin_hash(serialized);
}

// Validation helpers.
//-----------------------------------------------------------------------------

bool transaction::is_coinbase() const
{
    if (inputs_.empty())
        return false;

    const auto& prevout = inputs_.front().previous_output();
    return (inputs_.size() == 1) && prevout.is_null();
}

// True if coinbase and has invalid input[0] script size.
bool transaction::is_oversized_coinbase() const
{
    if (!is_coinbase())
        return false;

    const auto script_size = inputs_.front().script().serialized_size(false);
    return script_size < min_coinbase_size || script_size > max_coinbase_size;
}

// True if not coinbase but has null previous_output(s).
bool transaction::is_null_non_coinbase() const
{
    if (is_coinbase())
        return false;

    const auto invalid = [](const input& input)
    {
        return input.previous_output().is_null();
    };

    return std::any_of(inputs_.begin(), inputs_.end(), invalid);
}

// private
bool transaction::all_inputs_final() const
{
    const auto finalized = [](const input& input)
    {
        return input.is_final();
    };

    return std::all_of(inputs_.begin(), inputs_.end(), finalized);
}

bool transaction::is_final(size_t block_height) const
{
    const auto next_block_time = static_cast<uint32_t>(zulu_time());
    return is_final(block_height, next_block_time);
}

bool transaction::is_final(size_t block_height, uint32_t block_time) const
{
    const auto max_locktime = [=]()
    {
        return locktime_ < locktime_threshold ?
            safe_unsigned<uint32_t>(block_height) : block_time;
    };

    return locktime_ == 0 || locktime_ < max_locktime() || all_inputs_final();
}

// This is not a consensus rule, just detection of an irrational use.
bool transaction::is_locktime_conflict() const
{
    return locktime_ != 0 && all_inputs_final();
}

// Returns max_uint64 in case of overflow.
uint64_t transaction::total_input_value() const
{
    uint64_t value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (total_input_value_ != boost::none)
    {
        value = total_input_value_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    ////static_assert(max_money() < max_uint64, "overflow sentinel invalid");
    const auto sum = [](uint64_t total, const input& input)
    {
        const auto& prevout = input.previous_output().validation.cache;
        const auto missing = !prevout.is_valid();

        // Treat missing previous outputs as zero-valued, no math on sentinel.
        return ceiling_add(total, missing ? 0 : prevout.value());
    };

    value = std::accumulate(inputs_.begin(), inputs_.end(), uint64_t(0), sum);
    total_input_value_ = value;
    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

// Returns max_uint64 in case of overflow.
uint64_t transaction::total_output_value() const
{
    uint64_t value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (total_output_value_ != boost::none)
    {
        value = total_output_value_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    ////static_assert(max_money() < max_uint64, "overflow sentinel invalid");
    const auto sum = [](uint64_t total, const output& output)
    {
        return ceiling_add(total, output.value());
    };

    value = std::accumulate(outputs_.begin(), outputs_.end(), uint64_t(0), sum);
    total_output_value_ = value;
    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

uint64_t transaction::fees() const
{
    return floor_subtract(total_input_value(), total_output_value());
}

bool transaction::is_overspent() const
{
    return !is_coinbase() && total_output_value() > total_input_value();
}

// Returns max_size_t in case of overflow.
size_t transaction::signature_operations() const
{
    const auto state = validation.state;
    return state ? signature_operations(
        state->is_enabled(rule_fork::bip16_rule)) : max_size_t;
}

// Returns max_size_t in case of overflow.
size_t transaction::signature_operations(bool bip16_active) const
{
    const auto in = [bip16_active](size_t total, const input& input)
    {
        // This includes BIP16 p2sh additional sigops if prevout is cached.
        return ceiling_add(total, input.signature_operations(bip16_active));
    };

    const auto out = [](size_t total, const output& output)
    {
        return ceiling_add(total, output.signature_operations());
    };

    return std::accumulate(inputs_.begin(), inputs_.end(), size_t{0}, in) +
        std::accumulate(outputs_.begin(), outputs_.end(), size_t{0}, out);
}

bool transaction::is_missing_previous_outputs() const
{
    const auto missing = [](const input& input)
    {
        const auto& prevout = input.previous_output();
        const auto coinbase = prevout.is_null();
        const auto missing = !prevout.validation.cache.is_valid();
        return missing && !coinbase;
    };

    // This is an optimization of !missing_inputs().empty();
    return std::any_of(inputs_.begin(), inputs_.end(), missing);
}

point::list transaction::previous_outputs() const
{
    point::list prevouts(inputs_.size());

    const auto pointer = [](const input& input)
    {
        return input.previous_output();
    };

    const auto& ins = inputs_;
    std::transform(ins.begin(), ins.end(), prevouts.begin(), pointer);
    return prevouts;
}

point::list transaction::missing_previous_outputs() const
{
    point::list prevouts;

    for (auto& input: inputs_)
    {
        const auto& prevout = input.previous_output();
        const auto missing = !prevout.validation.cache.is_valid();

        if (missing && !prevout.is_null())
            prevouts.push_back(prevout);
    }

    return prevouts;
}

hash_list transaction::missing_previous_transactions() const
{
    const auto points = missing_previous_outputs();
    hash_list hashes(points.size());
    const auto hasher = [](const output_point& point) { return point.hash(); };
    std::transform(points.begin(), points.end(), hashes.begin(), hasher);
    return distinct(hashes);
}

bool transaction::is_internal_double_spend() const
{
    auto prevouts = previous_outputs();
    std::sort(prevouts.begin(), prevouts.end());
    const auto distinct_end = std::unique(prevouts.begin(), prevouts.end());
    const auto distinct = (distinct_end == prevouts.end());
    return !distinct;
}

bool transaction::is_double_spend(bool include_unconfirmed) const
{
    const auto spent = [include_unconfirmed](const input& input)
    {
        const auto& prevout = input.previous_output().validation;
        return prevout.spent && (include_unconfirmed || prevout.confirmed);
    };

    return std::any_of(inputs_.begin(), inputs_.end(), spent);
}

bool transaction::is_dusty(uint64_t minimum_output_value) const
{
    const auto dust = [minimum_output_value](const output& output)
    {
        return output.is_dust(minimum_output_value);
    };

    return std::any_of(outputs_.begin(), outputs_.end(), dust);
}

bool transaction::is_mature(size_t height) const
{
    const auto mature = [height](const input& input)
    {
        return input.previous_output().is_mature(height);
    };

    return std::all_of(inputs_.begin(), inputs_.end(), mature);
}

// Coinbase transactions return success, to simplify iteration.
code transaction::connect_input(const chain_state& state,
    size_t input_index) const
{
    if (input_index >= inputs_.size())
        return error::operation_failed;

    if (is_coinbase())
        return error::success;

    const auto& prevout = inputs_[input_index].previous_output().validation;

    // Verify that the previous output cache has been populated.
    if (!prevout.cache.is_valid())
        return error::missing_previous_output;

    const auto forks = state.enabled_forks();
    const auto index32 = static_cast<uint32_t>(input_index);

    // Verify the transaction input script against the previous output.
    return script::verify(*this, index32, forks);
}

// Validation.
//-----------------------------------------------------------------------------

// These checks are self-contained; blockchain (and so version) independent.
code transaction::check(bool transaction_pool) const
{
    if (inputs_.empty() || outputs_.empty())
        return error::empty_transaction;

    else if (is_null_non_coinbase())
        return error::previous_output_null;

    else if (total_output_value() > max_money())
        return error::spend_overflow;

    else if (!transaction_pool && is_oversized_coinbase())
        return error::invalid_coinbase_script_size;

    else if (transaction_pool && is_coinbase())
        return error::coinbase_transaction;

    else if (transaction_pool && is_internal_double_spend())
        return error::transaction_internal_double_spend;

    else if (transaction_pool && serialized_size(true) >= max_block_size)
        return error::transaction_size_limit;

    // We cannot know if bip16 is enabled at this point so we disable it.
    // This will not make a difference unless prevouts are populated, in which
    // case they are ignored. This means that p2sh sigops are not counted here.
    // This is a preliminary check, the final count must come from accept().
    // Reenable once sigop caching is implemented, otherwise is deoptimization.
    ////else if (transaction_pool && signature_operations(false) > max_block_sigops)
    ////    return error::transaction_legacy_sigop_limit;

    else
        return error::success;
}

code transaction::accept(bool transaction_pool) const
{
    const auto state = validation.state;
    return state ? accept(*state, transaction_pool) : error::operation_failed;
}

// These checks assume that prevout caching is completed on all tx.inputs.
// Flags for tx pool calls should be based on the current blockchain height.
code transaction::accept(const chain_state& state, bool transaction_pool) const
{
    const auto bip16 = state.is_enabled(rule_fork::bip16_rule);
    const auto bip30 = state.is_enabled(rule_fork::bip30_rule);

    //*************************************************************************
    // CONSENSUS:
    // We don't need to allow tx pool acceptance of an unspent duplicate
    // because tx pool validation is not strinctly a matter of consensus.
    //*************************************************************************
    const auto duplicates = state.is_enabled(rule_fork::allow_collisions) &&
        !transaction_pool;

    if (transaction_pool && state.is_under_checkpoint())
        return error::premature_validation;

    if (transaction_pool && !is_final(state.height()))
        return error::transaction_non_final;

    if (transaction_pool && version() > state.maximum_transaction_version())
        return error::transaction_version;

    //*************************************************************************
    // CONSENSUS:
    // A transaction hash that exists in the chain is not acceptable even if
    // the original is spent in the new block. This is not necessary nor is it
    // described by BIP30, but it is in the code referenced by BIP30. As such
    // the tx pool need only test against the chain, skipping the pool.
    //*************************************************************************
    else if (!duplicates && bip30 && validation.duplicate)
        return error::unspent_duplicate;

    else if (is_missing_previous_outputs())
        return error::missing_previous_output;

    else if (is_double_spend(transaction_pool))
        return error::double_spend;

    // This relates height to maturity of spent coinbase. Since reorg is the
    // only way to decrease height and reorg invalidates, this is cache safe.
    else if (!is_mature(state.height()))
        return error::coinbase_maturity;

    else if (is_overspent())
        return error::spend_exceeds_value;

    // This recomputes sigops to include p2sh from prevouts if bip16 is true.
    else if (transaction_pool && signature_operations(bip16) > max_block_sigops)
        return error::transaction_embedded_sigop_limit;

    else
        return error::success;
}

code transaction::connect() const
{
    const auto state = validation.state;
    return state ? connect(*state) : error::operation_failed;
}

code transaction::connect(const chain_state& state) const
{
    code ec;

    for (size_t input = 0; input < inputs_.size(); ++input)
        if ((ec = connect_input(state, input)))
            return ec;

    return error::success;
}

} // namespace chain
} // namespace libbitcoin
