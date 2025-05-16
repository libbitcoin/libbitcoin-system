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

#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Signature hashing (version 0 - segwit).
// ----------------------------------------------------------------------------

//*****************************************************************************
// CONSENSUS: if index exceeds outputs in signature hash, return null_hash.
//*****************************************************************************
hash_digest transaction::output_hash_v0(
    const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);
    if (output_overflow(index))
        return null_hash;

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    outputs_->at(index)->to_data(sink);
    sink.flush();
    return digest;
}

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************
bool transaction::version0_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript, uint64_t value,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Mask anyone_can_pay and unused bits, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);
    const auto anyone = is_anyone_can_pay(sighash_flags);
    const auto single = (flag == coverage::hash_single);
    const auto all = (flag == coverage::hash_all);

    // Create hash writer.
    stream::out::fast stream{ out };
    hash::sha256x2::fast sink{ stream };

    ///////////////////////////////////////////////////////////////////////////
    // TODO: update cache calls.
    ///////////////////////////////////////////////////////////////////////////

    sink.write_4_bytes_little_endian(version_);
    sink.write_bytes(!anyone ? hash_points() : null_hash);
    sink.write_bytes(!anyone && all ? hash_sequences() : null_hash);

    (*input)->point().to_data(sink);
    subscript.to_data(sink, true);
    sink.write_8_bytes_little_endian(value);
    sink.write_4_bytes_little_endian((*input)->sequence());

    if (single)
        sink.write_bytes(output_hash_v0(input));
    else
        sink.write_bytes(all ? hash_outputs() : null_hash);

    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);

    sink.flush();
    return true;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
