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
#include <bitcoin/system/wallet/psbt/output.hpp>

#include <algorithm>
#include <utility>
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

bool output::from_data(reader& source, uint32_t version) NOEXCEPT
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
        switch (static_cast<output_key>(entry.type()))
        {
            case output_key::embedded_script:
            {
                embedded_script = to_shared<chain::script>(entry.value, false);
                if (!typed || !embedded_script->is_valid())
                    return false;
                break;
            }
            case output_key::witness_script:
            {
                witness_script = to_shared<chain::script>(entry.value, false);
                if (!typed || !witness_script->is_valid())
                    return false;
                break;
            }
            case output_key::bip32_derivation:
            {
                derivation derivation{ std::move(data), {} };
                if (!is_public_key(derivation.point) ||
                    !derivation.origin.from_value(entry.value))
                    return false;
                derivations.push_back(std::move(derivation));
                break;
            }
            case output_key::amount:
            {
                if (version0 || !typed ||
                    entry.value.size() != sizeof(uint64_t))
                    return false;
                amount = from_little_endian<uint64_t>(entry.value);
                break;
            }
            case output_key::script:
            {
                script = to_shared<chain::script>(entry.value, false);
                if (version0 || !typed || !script->is_valid())
                    return false;
                break;
            }
            default:
            {
                others.push_back(std::move(entry));
                break;
            }
        }
    }

    // BIP370 requires the amount and script on every output.
    if (!version0 && (!amount.has_value() || !script))
        return false;

    // A valid source implies the map terminator was read.
    return !!source;
}

void output::to_data(writer& sink, uint32_t) const NOEXCEPT
{
    const auto write = [&sink](output_key type,
        const data_chunk& data, const data_chunk& value) NOEXCEPT
    {
        sink.write_variable(add1(data.size()));
        sink.write_byte(static_cast<uint8_t>(type));
        sink.write_bytes(data);
        sink.write_variable(value.size());
        sink.write_bytes(value);
    };

    if (embedded_script)
        write(output_key::embedded_script, {},
            embedded_script->to_data(false));

    if (witness_script)
        write(output_key::witness_script, {},
            witness_script->to_data(false));

    for (const auto& derived: sorted(derivations))
        write(output_key::bip32_derivation, derived.point,
            derived.origin.to_value());

    if (amount.has_value())
        write(output_key::amount, {},
            to_chunk(to_little_endian(amount.value())));

    if (script)
        write(output_key::script, {}, script->to_data(false));

    for (const auto& field: sorted(others))
        field.to_data(sink);

    // The map terminator.
    sink.write_byte(0);
}

void output::combine(const output& other) NOEXCEPT
{
    if (!embedded_script)
        embedded_script = other.embedded_script;

    if (!witness_script)
        witness_script = other.witness_script;

    if (!amount.has_value())
        amount = other.amount;

    if (!script)
        script = other.script;

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
