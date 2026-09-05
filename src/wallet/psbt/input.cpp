/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/wallet/psbt/input.hpp>

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/wallet/psbt/transaction.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

using namespace system::chain;

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

// The full key of each pair must be unique within the map.
static bool is_duplicate(std_vector<data_chunk>& keys,
    const data_chunk& key) NOEXCEPT
{
    if (std::find(keys.begin(), keys.end(), key) != keys.end())
        return true;

    keys.push_back(key);
    return false;
}

static bool to_uint32(uint32_t& out, const data_chunk& value) NOEXCEPT
{
    if (value.size() != sizeof(uint32_t))
        return false;

    out = from_little_endian<uint32_t>(value);
    return true;
}


// Serialization emits repeated-type entries in key order (deterministic).
static entry::list sorted(const entry::list& entries) NOEXCEPT
{
    auto copy = entries;
    std::sort(copy.begin(), copy.end(),
        [](const entry& left, const entry& right) NOEXCEPT
        {
            return left.key < right.key;
        });

    return copy;
}

static derivation::list sorted(const derivation::list& entries) NOEXCEPT
{
    auto copy = entries;
    std::sort(copy.begin(), copy.end(),
        [](const derivation& left, const derivation& right) NOEXCEPT
        {
            return left.point < right.point;
        });

    return copy;
}

bool input::from_data(reader& source, uint32_t version) NOEXCEPT
{
    const auto version0 = (version == transaction::version_0);
    std_vector<data_chunk> keys{};
    entry entry{};

    while (entry.from_data(source))
    {
        if (is_duplicate(keys, entry.key))
            return false;

        auto data = entry.keydata();
        const auto typed = data.empty();
        switch (static_cast<input_key>(entry.type()))
        {
            case input_key::non_witness_utxo:
            {
                stream::in::fast stream(entry.value);
                read::bytes::fast tx(stream);
                non_witness_utxo = to_shared<chain::transaction>(tx, true);
                if (!typed || !non_witness_utxo->is_valid() ||
                    !tx.is_exhausted())
                    return false;
                break;
            }
            case input_key::witness_utxo:
            {
                stream::in::fast stream(entry.value);
                read::bytes::fast out(stream);
                witness_utxo = to_shared<chain::output>(out);
                if (!typed || !witness_utxo->is_valid() ||
                    !out.is_exhausted())
                    return false;
                break;
            }
            case input_key::partial_sig:
            {
                if (!is_public_key(data))
                    return false;
                partial_signatures.push_back(std::move(entry));
                break;
            }
            case input_key::sighash_type:
            {
                uint32_t sighash{};
                if (!typed || !to_uint32(sighash, entry.value))
                    return false;
                sighash_type = sighash;
                break;
            }
            case input_key::embedded_script:
            {
                embedded_script = to_shared<chain::script>(entry.value, false);
                if (!typed || !embedded_script->is_valid())
                    return false;
                break;
            }
            case input_key::witness_script:
            {
                witness_script = to_shared<chain::script>(entry.value, false);
                if (!typed || !witness_script->is_valid())
                    return false;
                break;
            }
            case input_key::bip32_derivation:
            {
                derivation derivation{ std::move(data), {} };
                if (!is_public_key(derivation.point) ||
                    !derivation.origin.from_value(entry.value))
                    return false;
                derivations.push_back(std::move(derivation));
                break;
            }
            case input_key::final_script_sig:
            {
                final_script_sig = to_shared<chain::script>(entry.value, false);
                if (!typed || !final_script_sig->is_valid())
                    return false;
                break;
            }
            case input_key::final_script_witness:
            {
                final_script_witness = to_shared<chain::witness>(entry.value, true);
                if (!typed || !final_script_witness->is_valid())
                    return false;
                break;
            }
            case input_key::previous_txid:
            {
                if (version0 || !typed || entry.value.size() != hash_size)
                    return false;
                stream::in::fast stream(entry.value);
                read::bytes::fast hash(stream);
                previous_txid = hash.read_hash();
                break;
            }
            case input_key::output_index:
            {
                uint32_t index{};
                if (version0 || !typed || !to_uint32(index, entry.value))
                    return false;
                output_index = index;
                break;
            }
            case input_key::sequence:
            {
                uint32_t out{};
                if (version0 || !typed || !to_uint32(out, entry.value))
                    return false;
                sequence = out;
                break;
            }
            case input_key::required_time_locktime:
            {
                uint32_t time{};
                if (version0 || !typed || !to_uint32(time, entry.value) ||
                    time < locktime_threshold)
                    return false;
                required_time_locktime = time;
                break;
            }
            case input_key::required_height_locktime:
            {
                uint32_t height{};
                if (version0 || !typed || !to_uint32(height, entry.value) ||
                    is_zero(height) || height >= locktime_threshold)
                    return false;
                required_height_locktime = height;
                break;
            }
            default:
            {
                others.push_back(std::move(entry));
                break;
            }
        }
    }

    // BIP370 requires the prevout point on every input.
    if (!version0 && (!previous_txid.has_value() ||
        !output_index.has_value()))
        return false;

    // A valid source implies the map terminator was read.
    return !!source;
}

void input::to_data(writer& sink, uint32_t) const NOEXCEPT
{
    const auto write = [&sink](input_key type,
        const data_chunk& data, const data_chunk& value) NOEXCEPT
    {
        sink.write_variable(add1(data.size()));
        sink.write_byte(static_cast<uint8_t>(type));
        sink.write_bytes(data);
        sink.write_variable(value.size());
        sink.write_bytes(value);
    };

    if (non_witness_utxo)
        write(input_key::non_witness_utxo, {},
            non_witness_utxo->to_data(true));

    if (witness_utxo)
        write(input_key::witness_utxo, {}, witness_utxo->to_data());

    for (const auto& signature: sorted(partial_signatures))
        signature.to_data(sink);

    if (sighash_type.has_value())
        write(input_key::sighash_type, {},
            to_chunk(to_little_endian(sighash_type.value())));

    if (embedded_script)
        write(input_key::embedded_script, {},
            embedded_script->to_data(false));

    if (witness_script)
        write(input_key::witness_script, {},
            witness_script->to_data(false));

    for (const auto& derived: sorted(derivations))
        write(input_key::bip32_derivation, derived.point,
            derived.origin.to_value());

    if (final_script_sig)
        write(input_key::final_script_sig, {},
            final_script_sig->to_data(false));

    if (final_script_witness)
        write(input_key::final_script_witness, {},
            final_script_witness->to_data(true));

    if (previous_txid.has_value())
        write(input_key::previous_txid, {},
            to_chunk(previous_txid.value()));

    if (output_index.has_value())
        write(input_key::output_index, {},
            to_chunk(to_little_endian(output_index.value())));

    if (sequence.has_value())
        write(input_key::sequence, {},
            to_chunk(to_little_endian(sequence.value())));

    if (required_time_locktime.has_value())
        write(input_key::required_time_locktime, {},
            to_chunk(to_little_endian(required_time_locktime.value())));

    if (required_height_locktime.has_value())
        write(input_key::required_height_locktime, {},
            to_chunk(to_little_endian(required_height_locktime.value())));

    for (const auto& field: sorted(others))
        field.to_data(sink);

    // The map terminator.
    sink.write_byte(0);
}

bool input::is_final() const NOEXCEPT
{
    return final_script_sig || final_script_witness;
}

chain::output::cptr input::prevout(uint32_t index) const NOEXCEPT
{
    if (witness_utxo)
        return witness_utxo;

    if (non_witness_utxo &&
        index < non_witness_utxo->outputs_ptr()->size())
        return non_witness_utxo->outputs_ptr()->at(index);

    return {};
}

void input::combine(const input& other) NOEXCEPT
{
    if (!non_witness_utxo)
        non_witness_utxo = other.non_witness_utxo;

    if (!witness_utxo)
        witness_utxo = other.witness_utxo;

    if (!sighash_type.has_value())
        sighash_type = other.sighash_type;

    if (!embedded_script)
        embedded_script = other.embedded_script;

    if (!witness_script)
        witness_script = other.witness_script;

    if (!final_script_sig)
        final_script_sig = other.final_script_sig;

    if (!final_script_witness)
        final_script_witness = other.final_script_witness;

    if (!sequence.has_value())
        sequence = other.sequence;

    if (!required_time_locktime.has_value())
        required_time_locktime = other.required_time_locktime;

    if (!required_height_locktime.has_value())
        required_height_locktime = other.required_height_locktime;

    for (const auto& signature: other.partial_signatures)
        if (std::find(partial_signatures.begin(), partial_signatures.end(),
            signature) == partial_signatures.end())
            partial_signatures.push_back(signature);

    for (const auto& derivation: other.derivations)
        if (std::find(derivations.begin(), derivations.end(), derivation) ==
            derivations.end())
            derivations.push_back(derivation);

    for (const auto& entry: other.others)
        if (std::find(others.begin(), others.end(), entry) == others.end())
            others.push_back(entry);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin
