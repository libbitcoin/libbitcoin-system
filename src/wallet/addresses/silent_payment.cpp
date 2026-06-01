/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <iterator>
#include <utility>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/tapscript.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/radix/base_16.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace silent_payment {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

using chain::input;
using chain::point;
using chain::script;
using chain::script_pattern;
using chain::script_version;
using chain::tapscript;
using chain::transaction;

static constexpr auto bip341_nums_key = base16_array(
    "50929b74c1a04954b78b4b6035e97a5e078a5a0f28ec96d547bfee9ace803ac0");
static constexpr auto ec_odd_sign = add1(ec_even_sign);
static constexpr size_t maximum_outputs = 2323;

typedef data_array<point::serialized_size()> outpoint_data;

static outpoint_data to_outpoint_data(const point& value) NOEXCEPT
{
    outpoint_data out{};
    stream::out::fast stream{ out };
    write::bytes::fast writer{ stream };
    value.to_data(writer);
    return out;
}

static bool outpoint_data_less(const outpoint_data& left,
    const outpoint_data& right) NOEXCEPT
{
    return std::lexicographical_compare(left.begin(), left.end(),
        right.begin(), right.end());
}

static bool is_compressed_key_sign(uint8_t value) NOEXCEPT
{
    return value == ec_even_sign || value == ec_odd_sign;
}

static bool copy_xonly(ec_xonly& out, const data_chunk& value) NOEXCEPT
{
    if (value.size() != ec_xonly_size)
        return false;

    std::copy_n(value.begin(), ec_xonly_size, out.begin());
    return true;
}

static bool copy_compressed(ec_compressed& out,
    const data_chunk& value) NOEXCEPT
{
    if (!is_compressed_key(value))
        return false;

    std::copy_n(value.begin(), ec_compressed_size, out.begin());
    return true;
}

static ec_xonly to_xonly(const ec_uncompressed& point) NOEXCEPT
{
    ec_xonly out{};
    std::copy_n(std::next(point.begin()), ec_xonly_size, out.begin());
    return out;
}

static bool shared_secret_tweak(ec_secret& out,
    const ec_compressed& shared_secret, uint32_t index) NOEXCEPT
{
    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/SharedSecret"> sink{ stream };
    sink.write_bytes(shared_secret);
    sink.write_4_bytes_big_endian(index);
    sink.flush();
    return verify(out);
}

static bool contains_match(const scan_matches& matches, uint32_t index) NOEXCEPT
{
    for (const auto& match: matches)
        if (match.index == index)
            return true;

    return false;
}

static bool find_unmatched_output(uint32_t& out_index,
    const std_vector<pay_witness_taproot_output>& outputs, const ec_xonly& key,
    const scan_matches& matches) NOEXCEPT
{
    for (const auto& output: outputs)
    {
        if (output.key != key || contains_match(matches, output.index))
            continue;

        out_index = output.index;
        return true;
    }

    return false;
}

static bool contains_output(
    const std_vector<pay_witness_taproot_output>& outputs,
    const ec_xonly& key) NOEXCEPT
{
    for (const auto& output: outputs)
        if (output.key == key)
            return true;

    return false;
}

static bool extract_last_witness_key(ec_compressed& out,
    const input& in) NOEXCEPT
{
    const auto& stack = in.witness().stack();
    if (stack.empty() || !stack.back())
        return false;

    return copy_compressed(out, *stack.back());
}

static bool extract_p2pkh_key(ec_compressed& out, const input& in) NOEXCEPT
{
    const auto& ops = in.prevout->script().ops();
    if (!script::is_pay_key_hash_pattern(ops))
        return false;

    const auto bytes = in.script().to_data(false);
    if (bytes.size() < ec_compressed_size)
        return false;

    // P2PKH input scripts are malleable, so scan backward for the compressed
    // key whose HASH160 matches the spent output rather than trusting the last
    // push.
    for (auto offset = bytes.size() - ec_compressed_size + one; offset > zero;
        --offset)
    {
        const auto start = std::next(bytes.begin(), offset - one);
        if (!is_compressed_key_sign(*start))
            continue;

        ec_compressed key{};
        std::copy_n(start, ec_compressed_size, key.begin());
        const auto digest = bitcoin_short_hash(key);
        const auto& hash = unsafe_array_cast<uint8_t, short_hash_size>(
            ops.at(2).data().data());
        if (digest == hash)
        {
            out = key;
            return true;
        }
    }

    return false;
}

static bool to_even_compressed(ec_compressed& out,
    const ec_xonly& point) NOEXCEPT
{
    out.front() = ec_even_sign;
    std::copy(point.begin(), point.end(), std::next(out.begin()));
    return verify(out);
}

static bool is_nums_internal_key(const input& in) NOEXCEPT
{
    const auto& stack = in.witness().stack();

    // BIP352 only skips taproot script-path spends whose control block internal
    // key is the BIP341 NUMS point H.
    if (stack.size() <= one)
        return false;

    const auto control = in.witness().annex() ?
        *std::prev(stack.end(), 2) : stack.back();
    if (!control)
        return false;

    const tapscript script{ control };
    return script.is_valid()
        && script.is_tapscript()
        && script.key() == bip341_nums_key;
}

static bool extract_taproot_key(ec_compressed& out, const input& in) NOEXCEPT
{
    if (is_nums_internal_key(in))
        return false;

    const auto& program = in.prevout->script().witness_program();
    if (!program || program->size() != ec_xonly_size)
        return false;

    ec_xonly xonly{};
    if (!copy_xonly(xonly, *program))
        return false;

    return to_even_compressed(out, xonly);
}

static bool extract_pay_witness_taproot_output(pay_witness_taproot_output& out,
    const chain::output& output,
    uint32_t index) NOEXCEPT
{
    if (!script::is_pay_witness_taproot_pattern(output.script().ops()))
        return false;

    const auto& program = output.script().witness_program();
    if (!program || !copy_xonly(out.key, *program))
        return false;

    out.index = index;
    return true;
}

static bool extract_pay_witness_taproot_outputs(
    std_vector<pay_witness_taproot_output>& out,
    const transaction& tx) NOEXCEPT
{
    const auto& outputs = *tx.outputs_ptr();
    out.reserve(outputs.size());

    uint32_t index{};
    for (const auto& output: outputs)
    {
        pay_witness_taproot_output entry{};
        if (extract_pay_witness_taproot_output(entry, *output, index))
            out.push_back(entry);

        ++index;
    }

    return !out.empty();
}

static bool label_tweak(ec_secret& out, const ec_secret& scan_secret,
    uint32_t label) NOEXCEPT
{
    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/Label"> sink{ stream };
    sink.write_bytes(scan_secret);
    sink.write_4_bytes_big_endian(label);
    sink.flush();
    return verify(out);
}

static bool is_excluded_segwit_future(const input& in) NOEXCEPT
{
    return in.prevout
        && in.prevout->script().version() == script_version::reserved;
}

static bool extract_shared_secret_key(ec_compressed& out,
    const input& in) NOEXCEPT
{
    if (!in.prevout)
        return false;

    switch (in.prevout->script().output_pattern())
    {
        case script_pattern::pay_witness_v1_taproot:
            return extract_taproot_key(out, in);
        case script_pattern::pay_witness_key_hash:
            return extract_last_witness_key(out, in);
        case script_pattern::pay_script_hash:
            return script::is_sign_witness_key_hash_pattern(in.script().ops())
                && extract_last_witness_key(out, in);
        case script_pattern::pay_key_hash:
            return extract_p2pkh_key(out, in);
        default:
            return false;
    }
}

static bool compute_input_hash(ec_secret& out, const transaction& tx,
    const ec_compressed& sum) NOEXCEPT
{
    const auto& inputs = *tx.inputs_ptr();
    if (inputs.empty())
        return false;

    // BIP352 orders by serialized outpoints, not point's uniqueness ordering.
    auto minimum = to_outpoint_data(inputs.front()->point());
    for (auto it = std::next(inputs.begin()); it != inputs.end(); ++it)
    {
        const auto outpoint = to_outpoint_data((*it)->point());
        if (outpoint_data_less(outpoint, minimum))
            minimum = outpoint;
    }

    stream::out::fast stream{ out };
    hash::sha256t::fast<"BIP0352/Inputs"> sink{ stream };
    sink.write_bytes(minimum);
    sink.write_bytes(sum);
    sink.flush();

    return verify(out);
}

bool compute_scan_record(scan_record& out, const transaction& tx) NOEXCEPT
{
    scan_record record{};
    if (tx.is_coinbase() ||
        !extract_pay_witness_taproot_outputs(record.outputs, tx))
        return false;

    const auto& inputs = *tx.inputs_ptr();
    compressed_list keys{};
    keys.reserve(inputs.size());

    for (const auto& in: inputs)
    {
        if (!in->prevout || is_excluded_segwit_future(*in))
            return false;

        ec_compressed key{};
        if (extract_shared_secret_key(key, *in))
            keys.push_back(key);
    }

    if (keys.empty())
        return false;

    ec_compressed sum{};
    ec_secret input_hash{};
    if (!ec_sum(sum, keys) || !compute_input_hash(input_hash, tx, sum))
        return false;

    record.prevouts_summary = sum;
    if (!ec_multiply(record.prevouts_summary, input_hash))
        return false;

    out = std::move(record);
    return true;
}

bool scan_match::operator<(const scan_match& other) const NOEXCEPT
{
    return index < other.index;
}

scanner::scanner(const ec_secret& scan_secret,
    const ec_compressed& spend_public, const std_vector<uint32_t>& labels)
    NOEXCEPT
  : scan_secret_(scan_secret), labels_(labels)
{
    if (!verify(scan_secret_) || !decompress(spend_point_, spend_public))
        return;

    label_tweaks_.reserve(labels_.size());
    label_points_uncompressed_.reserve(labels_.size());

    for (const auto label: labels_)
    {
        ec_secret tweak{};
        ec_uncompressed point{};
        if (!label_tweak(tweak, scan_secret_, label) ||
            !secret_to_public(point, tweak))
        {
            return;
        }

        label_tweaks_.push_back(tweak);
        label_points_uncompressed_.push_back(point);
    }

    valid_ = true;
}

scanner::operator bool() const NOEXCEPT
{
    return valid();
}

bool scanner::valid() const NOEXCEPT
{
    return valid_
        && labels_.size() == label_tweaks_.size()
        && labels_.size() == label_points_uncompressed_.size();
}

bool scanner::to_shared_secret(ec_compressed& out,
    const ec_compressed& prevouts_summary) const NOEXCEPT
{
    out = prevouts_summary;
    return ec_multiply(out, scan_secret_);
}

bool scanner::to_shared_secret(ec_compressed& out,
    const ec_uncompressed& prevouts_summary) const NOEXCEPT
{
    ec_uncompressed point{ prevouts_summary };
    return ec_multiply(point, scan_secret_) && compress(out, point);
}

bool scanner::scan_shared(scan_matches& out,
    const ec_compressed& shared_secret,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    out.clear();
    out.reserve(outputs.size());

    if (outputs.empty())
        return true;

    uint32_t index{};
    while (out.size() < outputs.size() && index < maximum_outputs)
    {
        ec_secret tweak{};
        if (!shared_secret_tweak(tweak, shared_secret, index))
            return false;

        ec_uncompressed point{ spend_point_ };
        if (!ec_add(point, tweak))
            return false;

        uint32_t output_index{};
        scan_match found{};
        found.tweak = tweak;
        if (find_unmatched_output(output_index, outputs, to_xonly(point), out))
        {
            found.index = output_index;
            out.push_back(found);
            ++index;
            continue;
        }

        bool matched{};
        const auto label_points = label_points_uncompressed_.size();
        for (size_t label_index{}; label_index < label_points; ++label_index)
        {
            ec_uncompressed labelled{ point };
            if (!ec_add(labelled, label_points_uncompressed_[label_index]))
                return false;

            if (find_unmatched_output(output_index, outputs, to_xonly(labelled),
                out))
            {
                auto combined = tweak;
                if (!ec_add(combined, label_tweaks_[label_index]))
                    return false;

                found.index = output_index;
                found.tweak = combined;
                found.label = labels_[label_index];
                out.push_back(found);
                ++index;
                matched = true;
                break;
            }
        }

        if (!matched)
            break;
    }

    std::sort(out.begin(), out.end());
    return true;
}

bool scanner::match_shared(bool& out,
    const ec_compressed& shared_secret,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    if (outputs.empty())
    {
        out = false;
        return true;
    }

    ec_secret tweak{};
    if (!shared_secret_tweak(tweak, shared_secret, zero))
        return false;

    // Transaction discovery is satisfied by the receiver's initial output.
    ec_uncompressed point{ spend_point_ };
    if (!ec_add(point, tweak))
        return false;

    if (contains_output(outputs, to_xonly(point)))
    {
        out = true;
        return true;
    }

    for (const auto& label_point: label_points_uncompressed_)
    {
        ec_uncompressed labelled{ point };
        if (!ec_add(labelled, label_point))
            return false;

        if (contains_output(outputs, to_xonly(labelled)))
        {
            out = true;
            return true;
        }
    }

    out = false;
    return true;
}

bool scanner::scan(scan_matches& out, const ec_compressed& prevouts_summary,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    ec_compressed shared_secret{};
    return valid()
        && to_shared_secret(shared_secret, prevouts_summary)
        && scan_shared(out, shared_secret, outputs);
}

bool scanner::scan(scan_matches& out, const ec_uncompressed& prevouts_summary,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    ec_compressed shared_secret{};
    return valid()
        && to_shared_secret(shared_secret, prevouts_summary)
        && scan_shared(out, shared_secret, outputs);
}

bool scanner::match(bool& out, const ec_compressed& prevouts_summary,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    ec_compressed shared_secret{};
    return valid()
        && to_shared_secret(shared_secret, prevouts_summary)
        && match_shared(out, shared_secret, outputs);
}

bool scanner::match(bool& out, const ec_uncompressed& prevouts_summary,
    const std_vector<pay_witness_taproot_output>& outputs) const NOEXCEPT
{
    ec_compressed shared_secret{};
    return valid()
        && to_shared_secret(shared_secret, prevouts_summary)
        && match_shared(out, shared_secret, outputs);
}

BC_POP_WARNING()

} // namespace silent_payment
} // namespace wallet
} // namespace system
} // namespace libbitcoin
