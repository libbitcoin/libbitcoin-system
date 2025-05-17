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
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/extension.hpp>
#include <bitcoin/system/chain/enums/key_version.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

void transaction::initialize_sighash_cache() const NOEXCEPT
{
    if (!segregated_)
        return;

    // This overconstructs the cache (anyone or !all), however it is simple.
    sighash_cache_ =
    {
        hash_points(),
        hash_sequences(),
        hash_outputs()
    };
}

hash_digest transaction::hash_points() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->points;

    static hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& input: *inputs_)
        input->point().to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::hash_sequences() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->sequences;

    static hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& input: *inputs_)
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return digest;
}

hash_digest transaction::hash_outputs() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->outputs;

    static hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& output : *outputs_)
        output->to_data(sink);

    sink.flush();
    return digest;
}

// Cached identity hashing.
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

// More efficient than hash(bool) but not thead safe unless cached.
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

// Canonical form (thread safe).
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

// Cached for signature hashing (not thead safe unless cached).
// ----------------------------------------------------------------------------
// These rarely overconstruct the cache (anyone or !all) and are simple.

////void transaction::initialize_sighash_cache() const NOEXCEPT
////{
////    initialize_v0_cache();
////    initialize_v1_cache();
////}

void transaction::initialize_v0_cache() const NOEXCEPT
{
    ////v0_cache_ = to_shared<v0_cache>(
    ////    double_hash_points(),
    ////    double_hash_sequences(),
    ////    double_hash_outputs());
}

void transaction::initialize_v1_cache() const NOEXCEPT
{
    ////v1_cache_ = to_shared<v1_cache>(
    ////    single_hash_amounts(),
    ////    single_hash_scripts(),
    ////    single_hash_points(),
    ////    single_hash_sequences(),
    ////    single_hash_outputs());
}

// sha256x1

const hash_digest& transaction::single_hash_points() const NOEXCEPT
{
    // TODO: if cache not computed, compute and then return from cache.
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::single_hash_amounts() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::single_hash_scripts() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::single_hash_sequences() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::single_hash_outputs() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

// sha256x2

const hash_digest& transaction::double_hash_points() const NOEXCEPT
{
    // TODO: if cache not computed, compute and then return from cache.
    // TODO: compute single hashes if not computed.
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::double_hash_sequences() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

const hash_digest& transaction::double_hash_outputs() const NOEXCEPT
{
    static constexpr hash_digest hash{};
    return hash;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
