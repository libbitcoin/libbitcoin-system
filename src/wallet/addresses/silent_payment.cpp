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
#include <bitcoin/system/wallet/addresses/silent_payment.hpp>

#include <algorithm>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Sorting.
// ----------------------------------------------------------------------------

bool silent_payment::scan_match::operator<(
    const scan_match& other) const NOEXCEPT
{
    return index < other.index;
}

// Static.
// ----------------------------------------------------------------------------

// The prevouts summary is input_hash * A, where A is the sum of the eligible
// input keys and input_hash commits to the smallest outpoint and A [bip352].
bool silent_payment::summarize(ec_compressed& out,
    const chain::transaction& tx) NOEXCEPT
{
    if (tx.is_coinbase())
        return false;

    const auto& inputs = *tx.inputs_ptr();
    const auto& outputs = *tx.outputs_ptr();
    if (std::none_of(outputs.begin(), outputs.end(), [](const auto& output)
    {
        return chain::script::is_pay_witness_taproot_pattern(
            output->script().ops());
    }))
        return false;

    ec_compresseds keys{};
    keys.reserve(inputs.size());
    for (const auto& input: inputs)
    {
        // Spends of future segwit versions exclude the transaction.
        if (!input->prevout || input->prevout->script().version() ==
            chain::script_version::reserved)
            return false;

        ec_compressed key{};
        if (get_input_key(key, *input))
            keys.push_back(key);
    }

    const auto smallest = std::min_element(inputs.begin(), inputs.end(),
        [](const auto& left, const auto& right) NOEXCEPT
    {
        return is_lesser(left->point(), right->point());
    });

    ec_secret hash{};
    return !keys.empty() && ec_sum(out, keys) &&
        input_hash(hash, (*smallest)->point(), out) &&
        ec_multiply(out, hash);
}

bool silent_payment::get_outputs(scan_outputs& out,
    const chain::transaction& tx) NOEXCEPT
{
    out.clear();
    uint32_t index{};
    for (const auto& output: *tx.outputs_ptr())
    {
        const auto& script = output->script();
        if (chain::script::is_pay_witness_taproot_pattern(script.ops()))
            out.push_back({ index, unsafe_array_cast<uint8_t, ec_xonly_size>(
                script.witness_program()->data()) });

        ++index;
    }

    return !out.empty();
}

// Construction.
// ----------------------------------------------------------------------------

silent_payment::silent_payment(const ec_secret& scan_secret,
    const ec_compressed& spend_key,
    const std_vector<uint32_t>& labels) NOEXCEPT
  : scan_secret_(scan_secret), labels_(labels)
{
    if (!verify_secret(scan_secret_) || !decompress(spend_key_, spend_key))
        return;

    ec_secret tweak{};
    ec_uncompressed key{};
    label_tweaks_.reserve(labels_.size());
    label_keys_.reserve(labels_.size());
    for (const auto label: labels_)
    {
        if (!label_tweak(tweak, scan_secret_, label) ||
            !secret_to_public(key, tweak))
            return;

        label_tweaks_.push_back(tweak);
        label_keys_.push_back(key);
    }

    valid_ = true;
}

silent_payment::operator bool() const NOEXCEPT
{
    return valid_;
}

// Scanning.
// ----------------------------------------------------------------------------

// Each k is satisfied by an unmatched output keyed by B_spend + t_k * G, or by
// that sum plus one label key, and scanning stops at the first k with neither.
bool silent_payment::scan(scan_matches& out, const ec_compressed& summary,
    const scan_outputs& outputs) const NOEXCEPT
{
    out.clear();
    ec_compressed shared{};
    if (!valid_ || !shared_secret(shared, summary))
        return false;

    ec_secret tweak{};
    ec_uncompressed key{};
    ec_uncompressed labeled{};
    for (uint32_t k{}; out.size() < outputs.size() && k < maximum_outputs;
        ++k)
    {
        key = spend_key_;
        if (!shared_tweak(tweak, shared, k) || !ec_add(key, tweak))
            return false;

        uint32_t index{};
        if (find_output(index, outputs, array_cast<uint8_t, ec_xonly_size,
            one>(key), out))
        {
            out.push_back({ index, tweak, unlabeled });
            continue;
        }

        auto found = false;
        for (size_t label{}; !found && label < labels_.size(); ++label)
        {
            labeled = key;
            if (!ec_add(labeled, label_keys_.at(label)))
                return false;

            if (find_output(index, outputs, array_cast<uint8_t,
                ec_xonly_size, one>(labeled), out))
            {
                auto combined = tweak;
                if (!ec_add(combined, label_tweaks_.at(label)))
                    return false;

                out.push_back({ index, combined, labels_.at(label) });
                found = true;
            }
        }

        if (!found)
            break;
    }

    std::sort(out.begin(), out.end());
    return true;
}

// A receiver's first output (k = 0) is sufficient to discover a transaction.
bool silent_payment::match(bool& out, const ec_compressed& summary,
    const scan_outputs& outputs) const NOEXCEPT
{
    out = false;
    ec_compressed shared{};
    ec_secret tweak{};
    auto key = spend_key_;
    if (!valid_ || !shared_secret(shared, summary) ||
        !shared_tweak(tweak, shared, zero) || !ec_add(key, tweak))
        return false;

    const auto contains = [&](const ec_uncompressed& point) NOEXCEPT
    {
        const auto& xonly = array_cast<uint8_t, ec_xonly_size, one>(point);
        return std::any_of(outputs.begin(), outputs.end(),
            [&](const scan_output& output) NOEXCEPT
            {
                return output.key == xonly;
            });
    };

    if (contains(key))
    {
        out = true;
        return true;
    }

    ec_uncompressed labeled{};
    for (const auto& label_key: label_keys_)
    {
        labeled = key;
        if (!ec_add(labeled, label_key))
            return false;

        if (contains(labeled))
        {
            out = true;
            return true;
        }
    }

    return true;
}

// protected
// ----------------------------------------------------------------------------

// Outpoints order by serialization: hash bytes, then little-endian index.
bool silent_payment::is_lesser(const chain::point& left,
    const chain::point& right) NOEXCEPT
{
    if (left.hash() != right.hash())
        return left.hash() < right.hash();

    return to_little_endian(left.index()) < to_little_endian(right.index());
}

bool silent_payment::input_hash(ec_secret& out, const chain::point& smallest,
    const ec_compressed& sum) NOEXCEPT
{
    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/Inputs"> sink{ stream };
    smallest.to_data(sink);
    sink.write_bytes(sum);
    sink.flush();
    return verify_secret(out);
}

bool silent_payment::shared_tweak(ec_secret& out, const ec_compressed& shared,
    uint32_t k) NOEXCEPT
{
    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/SharedSecret"> sink{ stream };
    sink.write_bytes(shared);
    sink.write_4_bytes_big_endian(k);
    sink.flush();
    return verify_secret(out);
}

bool silent_payment::label_tweak(ec_secret& out, const ec_secret& scan_secret,
    uint32_t label) NOEXCEPT
{
    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/Label"> sink{ stream };
    sink.write_bytes(scan_secret);
    sink.write_4_bytes_big_endian(label);
    sink.flush();
    return verify_secret(out);
}

bool silent_payment::get_input_key(ec_compressed& out,
    const chain::input& input) NOEXCEPT
{
    switch (input.prevout->script().output_pattern())
    {
        case chain::script_pattern::pay_witness_v1_taproot:
            return get_taproot_key(out, input);
        case chain::script_pattern::pay_witness_key_hash:
            return get_witness_key(out, input);
        case chain::script_pattern::pay_script_hash:
            return chain::script::is_sign_witness_key_hash_pattern(
                input.script().ops()) && get_witness_key(out, input);
        case chain::script_pattern::pay_key_hash:
            return get_key_hash_key(out, input);
        default:
            return false;
    }
}

// The key is the last witness element, and only compressed keys contribute.
bool silent_payment::get_witness_key(ec_compressed& out,
    const chain::input& input) NOEXCEPT
{
    const auto& stack = input.witness().stack();
    if (stack.empty() || !stack.back() || !is_compressed_key(*stack.back()))
        return false;

    out = unsafe_array_cast<uint8_t, ec_compressed_size>(
        stack.back()->data());
    return true;
}

// Input scripts are malleable, so the last compressed key that hashes to the
// prevout's key hash is taken from the serialized input script [bip352].
bool silent_payment::get_key_hash_key(ec_compressed& out,
    const chain::input& input) NOEXCEPT
{
    const auto& hash = input.prevout->script().ops().at(2).data();
    const auto bytes = input.script().to_data(false);
    if (bytes.size() < ec_compressed_size)
        return false;

    for (auto end = bytes.size(); end >= ec_compressed_size; --end)
    {
        const auto start = std::next(bytes.begin(), end - ec_compressed_size);
        const auto sign = *start;
        if (sign != ec_even_sign && sign != ec_odd_sign)
            continue;

        std::copy_n(start, ec_compressed_size, out.begin());
        if (bitcoin_short_hash(out) ==
            unsafe_array_cast<uint8_t, short_hash_size>(hash.data()))
            return true;
    }

    return false;
}

// The key is the output program, with even y, unless the input is a script
// path spend with the NUMS internal key [bip352].
bool silent_payment::get_taproot_key(ec_compressed& out,
    const chain::input& input) NOEXCEPT
{
    const auto& program = input.prevout->script().witness_program();
    if (is_nums_spend(input) || !program || program->size() != ec_xonly_size)
        return false;

    out.front() = ec_even_sign;
    std::copy(program->begin(), program->end(), std::next(out.begin()));
    return verify_point(out);
}

bool silent_payment::is_nums_spend(const chain::input& input) NOEXCEPT
{
    const auto& witness = input.witness();
    const auto& stack = witness.stack();
    const auto items = witness.annex() ? sub1(stack.size()) : stack.size();
    if (items < two)
        return false;

    const chain::tapscript control{ stack.at(sub1(items)) };
    return control.is_valid() && control.key() == nums_key;
}

bool silent_payment::find_output(uint32_t& out, const scan_outputs& outputs,
    const ec_xonly& key, const scan_matches& matches) NOEXCEPT
{
    for (const auto& output: outputs)
    {
        if (output.key != key || std::any_of(matches.begin(), matches.end(),
            [&](const scan_match& match) NOEXCEPT
            {
                return match.index == output.index;
            }))
            continue;

        out = output.index;
        return true;
    }

    return false;
}

// private
// ----------------------------------------------------------------------------

bool silent_payment::shared_secret(ec_compressed& out,
    const ec_compressed& summary) const NOEXCEPT
{
    out = summary;
    return ec_multiply(out, scan_secret_);
}

BC_POP_WARNING()

} // namespace wallet
} // namespace system
} // namespace libbitcoin
