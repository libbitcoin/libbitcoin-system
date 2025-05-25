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
#include <bitcoin/system/chain/transaction.hpp>

#include <iterator>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Identity hashing.
// ----------------------------------------------------------------------------

// static
// Compute the desegregated hash from a segregated buffer (used by network).
hash_digest transaction::desegregated_hash(size_t witnessed,
    size_t unwitnessed, const uint8_t* data) NOEXCEPT
{
    if (is_null(data))
        return null_hash;

    constexpr auto preamble = sizeof(uint32_t) + two * sizeof(uint8_t);
    const auto puts = floored_subtract(unwitnessed, two * sizeof(uint32_t));
    const auto locktime = floored_subtract(witnessed, sizeof(uint32_t));

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    sink.write_bytes(data, sizeof(uint32_t));
    sink.write_bytes(std::next(data, preamble), puts);
    sink.write_bytes(std::next(data, locktime), sizeof(uint32_t));
    sink.flush();
    return digest;
}

// Canonical hash, recomputes if not cached, returns copy if cached.
hash_digest transaction::hash(bool witness) const NOEXCEPT
{
    if (segregated_)
    {
        if (witness)
        {
            // Witness coinbase tx hash is assumed to be null_hash [bip141].
            if (witness_hash_) return *witness_hash_;
            if (is_coinbase()) return null_hash;
        }
        else
        {
            if (nominal_hash_) return *nominal_hash_;
        }
    }
    else
    {
        if (nominal_hash_) return *nominal_hash_;
    }

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    to_data(sink, witness);
    sink.flush();
    return digest;
}

// Cached identity hashing (not thead safe).
// ----------------------------------------------------------------------------

// Used to populate nominal hash after wire deserialization and store read.
void transaction::set_nominal_hash(const hash_digest& hash) const NOEXCEPT
{
    nominal_hash_ = hash;
}

// Used to populate witness hash after wire deserialization (not stored).
void transaction::set_witness_hash(const hash_digest& hash) const NOEXCEPT
{
    witness_hash_ = hash;
}

// Efficient because always returns a reference and never recomputes.
const hash_digest& transaction::get_hash(bool witness) const NOEXCEPT
{
    if (witness)
    {
        if (!witness_hash_) set_witness_hash(hash(witness));
        return *witness_hash_;
    }
    else
    {
        if (!nominal_hash_) set_nominal_hash(hash(witness));
        return *nominal_hash_;
    }
}

// Cached signature hashing (not thead safe).
// ----------------------------------------------------------------------------

hash_digest transaction::x1_base_hash_points() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (const auto& input: *inputs_)
        input->point().to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::x1_base_hash_sequences() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (const auto& input: *inputs_)
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return digest;
}

hash_digest transaction::x1_base_hash_outputs() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (const auto& output : *outputs_)
        output->to_data(sink);

    sink.flush();
    return digest;
}

// This requires ALL prevouts of the tx are populated (new in taproot).
hash_digest transaction::v1_only_hash_amounts() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (const auto& input : *inputs_)
        sink.write_8_bytes_little_endian(input->prevout->value());

    sink.flush();
    return digest;
}

// This requires ALL prevouts of the tx are populated (new in taproot).
hash_digest transaction::v1_only_hash_scripts() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (const auto& input : *inputs_)
        input->prevout->script().to_data(sink, true);

    sink.flush();
    return digest;
}

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

void transaction::set_x1_base_hash() const NOEXCEPT
{
    if (!x1_base_cache_)
        x1_base_cache_ = std::make_shared<base_cache>
        (
            x1_base_hash_points(),
            x1_base_hash_sequences(),
            x1_base_hash_outputs()
        );
}

void transaction::set_x2_base_hash() const NOEXCEPT
{
    if (!x2_base_cache_)
        x2_base_cache_ = std::make_shared<base_cache>
        (
            sha256_hash(single_hash_points()),
            sha256_hash(single_hash_sequences()),
            sha256_hash(single_hash_outputs())
        );
}

void transaction::set_v1_only_hash() const NOEXCEPT
{
    if (!v1_only_cache_)
        v1_only_cache_ = std::make_shared<only_cache>
        (
            v1_only_hash_amounts(),
            v1_only_hash_scripts()
        );
}

BC_POP_WARNING()

// sha256x1 (script verson 1)
// ----------------------------------------------------------------------------

const hash_digest& transaction::single_hash_points() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->points;
}

const hash_digest& transaction::single_hash_sequences() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->sequences;
}

const hash_digest& transaction::single_hash_outputs() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->outputs;
}

const hash_digest& transaction::single_hash_amounts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->amounts;
}

const hash_digest& transaction::single_hash_scripts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->scripts;
}

// sha256x2 (script verson 0)
// ----------------------------------------------------------------------------

const hash_digest& transaction::double_hash_points() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->points;
}

const hash_digest& transaction::double_hash_sequences() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->sequences;
}

const hash_digest& transaction::double_hash_outputs() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->outputs;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
