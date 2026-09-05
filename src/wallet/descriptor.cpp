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
#include <bitcoin/system/wallet/descriptor.hpp>

#include <unordered_map>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/wallet/addresses/payment_address.hpp>
#include <bitcoin/system/wallet/addresses/witness_address.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

using namespace system::chain;

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

constexpr size_t checksum_length = 8;
constexpr auto hardened_bit = 0x80000000_u32;

// Checksum (bip380).
// ----------------------------------------------------------------------------

// The bip380 checksum generator (bch code over GF(32), as bech32).
static uint64_t poly_mod(uint64_t check, uint64_t value) NOEXCEPT
{
    auto out = bit_xor(shift_left(bit_and(check, 0x00000007ffffffff_u64), 5),
        value);

    if (get_right(check, 35)) out = bit_xor(out, 0xf5dee51989_u64);
    if (get_right(check, 36)) out = bit_xor(out, 0xa9fdca3312_u64);
    if (get_right(check, 37)) out = bit_xor(out, 0x1bab10e32d_u64);
    if (get_right(check, 38)) out = bit_xor(out, 0x3706b1677a_u64);
    if (get_right(check, 39)) out = bit_xor(out, 0x644d626ffd_u64);
    return out;
}

std::string descriptor::to_checksum(const std::string& body) NOEXCEPT
{
    // Grouped so that common case and keypath errors cost a single symbol.
    static const std::string input_charset
    {
        "0123456789()[],'/*abcdefgh@:$%{}"
        "IJKLMNOPQRSTUVWXYZ&+-.;<=>?!^_|~"
        "ijklmnopqrstuvwxyzABCDEFGH`#\"\\ "
    };

    // The checksum character set (as bech32).
    static const std::string checksum_charset
    {
        "qpzry9x8gf2tvdw0s3jn54khce6mua7l"
    };

    uint64_t check = 1;
    uint64_t group{};
    size_t grouped{};
    for (const auto character: body)
    {
        const auto position = input_charset.find(character);
        if (position == std::string::npos)
            return {};

        check = poly_mod(check, bit_and<uint64_t>(position, 31));
        group = 3 * group + shift_right<uint64_t>(position, 5);
        if (++grouped == 3)
        {
            check = poly_mod(check, group);
            group = 0;
            grouped = 0;
        }
    }

    if (is_nonzero(grouped))
        check = poly_mod(check, group);

    for (size_t pad{}; pad < checksum_length; ++pad)
        check = poly_mod(check, 0);

    check = bit_xor<uint64_t>(check, 1);

    std::string out{};
    for (size_t index{}; index < checksum_length; ++index)
        out += checksum_charset.at(bit_and<uint64_t>(
            shift_right(check, 5 * (7 - index)), 31));

    return out;
}

// Key expressions.
// ----------------------------------------------------------------------------

static bool parse_path(std_vector<uint32_t>& path, bool& wildcard,
    bool& hardened, const std::string& text) NOEXCEPT
{
    if (text.empty())
        return true;

    if (text.front() != '/')
        return false;

    for (const auto& element: split(text.substr(1), "/"))
    {
        if (wildcard)
            return false;

        auto value = element;
        auto hard = false;
        if (!value.empty() && (value.back() == '\'' || value.back() == 'h' ||
            value.back() == 'H'))
        {
            hard = true;
            value.pop_back();
        }

        if (value == "*")
        {
            wildcard = true;
            hardened = hard;
            continue;
        }

        uint32_t index{};
        if (value.empty() || !deserialize(index, value) ||
            index >= hardened_bit)
            return false;

        path.push_back(hard ? bit_or(index, hardened_bit) : index);
    }

    return true;
}

bool descriptor::parse_key(key_expression& out,
    const std::string& text) NOEXCEPT
{
    auto rest = text;

    // The optional key origin is bracketed.
    if (!rest.empty() && rest.front() == '[')
    {
        const auto close = rest.find(']');
        constexpr size_t fingerprint_length = 8;
        if (close == std::string::npos || close < add1(fingerprint_length))
            return false;

        const auto inner = rest.substr(1, sub1(close));
        data_chunk fingerprint{};
        if (!decode_base16(fingerprint,
            inner.substr(0, fingerprint_length)) ||
            fingerprint.size() != sizeof(uint32_t))
            return false;

        psbt::key_origin origin{};
        origin.fingerprint = from_little_endian<uint32_t>(fingerprint);
        auto wildcard = false;
        auto hardened = false;
        if (!parse_path(origin.path, wildcard, hardened,
            inner.substr(fingerprint_length)) || wildcard)
            return false;

        out.origin = std::move(origin);
        rest = rest.substr(add1(close));
    }

    // A hexadecimal public key, x-only under tr (no path).
    constexpr auto xonly_length = ec_xonly_size * two;
    if (!rest.empty() && (rest.front() == '0' || rest.front() == '4' ||
        rest.size() == xonly_length))
    {
        data_chunk point{};
        if (!decode_base16(point, rest) ||
            !(is_public_key(point) || point.size() == ec_xonly_size))
            return false;

        out.form = key_form::point;
        out.point = std::move(point);
        return true;
    }

    // An extended key with an optional path and wildcard.
    const auto slash = rest.find('/');
    const auto prefix = rest.substr(0, slash);
    const auto path = (slash == std::string::npos) ? "" : rest.substr(slash);

    const hd_private extended_private{ prefix };
    if (extended_private)
    {
        out.form = key_form::extended_private;
        out.extended_private = extended_private;
        return parse_path(out.path, out.wildcard, out.hardened, path);
    }

    const hd_public extended_public{ prefix };
    if (extended_public)
    {
        out.form = key_form::extended_public;
        out.extended_public = extended_public;
        return parse_path(out.path, out.wildcard, out.hardened, path);
    }

    // A wallet import format secret (no path).
    const ec_private secret{ prefix };
    if (secret && slash == std::string::npos)
    {
        data_chunk point{};
        if (!secret.to_public().to_data(point))
            return false;

        out.form = key_form::secret;
        out.point = std::move(point);
        return true;
    }

    return false;
}

bool descriptor::key_expression::derive(data_chunk& out,
    uint32_t index) const NOEXCEPT
{
    switch (form)
    {
        case key_form::point:
        case key_form::secret:
        {
            out = point;
            return !out.empty();
        }
        case key_form::extended_public:
        {
            // Hardened elements cannot be derived from a public key.
            if (hardened)
                return false;

            auto key = extended_public;
            for (const auto& element: path)
            {
                if (element >= hardened_bit)
                    return false;

                key = key.derive_public(element);
            }

            if (wildcard)
                key = key.derive_public(index);

            if (!key)
                return false;

            out = to_chunk(key.point());
            return true;
        }
        case key_form::extended_private:
        {
            auto key = extended_private;
            for (const auto& element: path)
                key = key.derive_private(element);

            if (wildcard)
                key = key.derive_private(hardened ?
                    bit_or(index, hardened_bit) : index);

            if (!key)
                return false;

            out = to_chunk(key.to_public().point());
            return true;
        }
        default:
            return false;
    }
}

bool descriptor::key_expression::derive(psbt::derivation& out,
    uint32_t index) const NOEXCEPT
{
    if (!derive(out.point, index))
        return false;

    // An origin provides the fingerprint and leading path elements. Without
    // one an extended key is its own origin and a bare key its fingerprint.
    auto& to = out.origin;
    to.path.clear();
    if (origin.has_value())
    {
        to.fingerprint = origin->fingerprint;
        to.path = origin->path;
    }
    else if (form == key_form::point || form == key_form::secret)
    {
        to.fingerprint = from_little_endian<uint32_t>(
            bitcoin_short_hash(out.point));
    }
    else
    {
        const auto& key = (form == key_form::extended_private) ?
            extended_private.to_public() : extended_public;
        to.fingerprint = from_little_endian<uint32_t>(
            bitcoin_short_hash(key.point()));
    }

    to.path.insert(to.path.end(), path.begin(), path.end());
    if (wildcard)
        to.path.push_back(hardened ? bit_or(index, hardened_bit) : index);

    return true;
}

// Parse.
// ----------------------------------------------------------------------------

bool descriptor::parse(node& out, const std::string& body) NOEXCEPT
{
    const auto open = body.find('(');
    if (open == std::string::npos || body.back() != ')')
        return false;

    const auto name = body.substr(0, open);
    const auto inner = body.substr(add1(open),
        sub1(body.size() - add1(open)));

    static const std::unordered_map<std::string, function> functions
    {
        { "pk", function::pk },
        { "pkh", function::pkh },
        { "wpkh", function::wpkh },
        { "sh", function::sh },
        { "wsh", function::wsh },
        { "multi", function::multi },
        { "sortedmulti", function::sortedmulti },
        { "combo", function::combo },
        { "tr", function::tr },
        { "addr", function::addr },
        { "raw", function::raw }
    };

    const auto entry = functions.find(name);
    if (entry == functions.end())
        return false;

    out.type = entry->second;
    switch (out.type)
    {
        case function::sh:
        case function::wsh:
        {
            out.child = std::make_shared<node>();
            return parse(*out.child, inner);
        }
        case function::multi:
        case function::sortedmulti:
        {
            const auto parts = split(inner, ",");
            if (parts.size() < two)
                return false;

            uint64_t required{};
            if (!deserialize(required, parts.front()) ||
                is_zero(required) || required >= parts.size() ||
                required > max_uint8)
                return false;

            out.required = possible_narrow_cast<uint8_t>(required);
            for (auto part = std::next(parts.begin());
                part != parts.end(); ++part)
            {
                key_expression key{};
                if (!parse_key(key, *part))
                    return false;

                out.keys.push_back(std::move(key));
            }

            return true;
        }
        case function::addr:
        {
            out.address = inner;
            return !inner.empty();
        }
        case function::raw:
        {
            return decode_base16(out.raw, inner);
        }
        default:
        {
            key_expression key{};
            if (!parse_key(key, inner))
                return false;

            out.keys.push_back(std::move(key));
            return true;
        }
    }
}

// Derive.
// ----------------------------------------------------------------------------

bool descriptor::derive_signings(signing::list& out, const node& tree,
    uint32_t index, bool top) NOEXCEPT
{
    switch (tree.type)
    {
        case function::pk:
        {
            psbt::derivation key{};
            if (!tree.keys.front().derive(key, index) ||
                !is_public_key(key.point))
                return false;

            signing item{};
            item.script = { script::to_pay_public_key_pattern(key.point) };
            item.derivations.push_back(std::move(key));
            out.push_back(std::move(item));
            return true;
        }
        case function::pkh:
        {
            psbt::derivation key{};
            if (!tree.keys.front().derive(key, index) ||
                !is_public_key(key.point))
                return false;

            signing item{};
            item.script = { script::to_pay_key_hash_pattern(
                bitcoin_short_hash(key.point)) };
            item.derivations.push_back(std::move(key));
            out.push_back(std::move(item));
            return true;
        }
        case function::wpkh:
        {
            psbt::derivation key{};
            if (!tree.keys.front().derive(key, index) ||
                !is_compressed_key(key.point))
                return false;

            signing item{};
            item.script = { script::to_pay_witness_key_hash_pattern(
                bitcoin_short_hash(key.point)) };
            item.derivations.push_back(std::move(key));
            out.push_back(std::move(item));
            return true;
        }
        case function::sh:
        {
            signing::list inner{};
            if (top && derive_signings(inner, *tree.child, index, false) &&
                is_one(inner.size()))
            {
                auto& item = inner.front();
                item.embedded = to_shared<chain::script>(
                    std::move(item.script));
                item.script = { script::to_pay_script_hash_pattern(
                    bitcoin_short_hash(item.embedded->to_data(false))) };
                out.push_back(std::move(item));
                return true;
            }

            return false;
        }
        case function::wsh:
        {
            signing::list inner{};
            if (tree.child->type != function::wpkh &&
                tree.child->type != function::wsh &&
                derive_signings(inner, *tree.child, index, false) &&
                is_one(inner.size()))
            {
                auto& item = inner.front();
                item.witness = to_shared<chain::script>(
                    std::move(item.script));
                item.script = { script::to_pay_witness_script_hash_pattern(
                    sha256_hash(item.witness->to_data(false))) };
                out.push_back(std::move(item));
                return true;
            }

            return false;
        }
        case function::multi:
        case function::sortedmulti:
        {
            signing item{};
            data_stack points{};
            for (const auto& key: tree.keys)
            {
                psbt::derivation derived{};
                if (!key.derive(derived, index) ||
                    !is_public_key(derived.point))
                    return false;

                points.push_back(derived.point);
                item.derivations.push_back(std::move(derived));
            }

            if (tree.type == function::sortedmulti)
                std::sort(points.begin(), points.end());

            item.script = { script::to_pay_multisig_pattern(tree.required,
                points) };
            out.push_back(std::move(item));
            return true;
        }
        case function::combo:
        {
            psbt::derivation key{};
            if (!top || !tree.keys.front().derive(key, index))
                return false;

            const auto& point = key.point;
            out.push_back({ { script::to_pay_public_key_pattern(point) },
                {}, {}, { key } });
            const auto hash = bitcoin_short_hash(point);
            out.push_back({ { script::to_pay_key_hash_pattern(hash) },
                {}, {}, { key } });
            if (is_compressed_key(point))
            {
                const auto witness = to_shared<chain::script>(
                    script::to_pay_witness_key_hash_pattern(hash));
                out.push_back({ *witness, {}, {}, { key } });
                out.push_back({ { script::to_pay_script_hash_pattern(
                    bitcoin_short_hash(witness->to_data(false))) },
                    witness, {}, { key } });
            }

            return true;
        }
        case function::tr:
        {
            data_chunk point{};
            if (!top || !tree.keys.front().derive(point, index))
                return false;

            // The internal key is x-only (a compressed key drops its sign).
            ec_compressed lifted{};
            lifted.front() = ec_even_sign;
            if (point.size() == ec_xonly_size)
                std::copy(point.begin(), point.end(),
                    std::next(lifted.begin()));
            else if (is_compressed_key(point))
                std::copy(std::next(point.begin()), point.end(),
                    std::next(lifted.begin()));
            else
                return false;

            // bip341 key path spend commits to no script tree.
            const auto internal = unsafe_array_cast<uint8_t, ec_xonly_size>(
                std::next(lifted.data()));
            if (!ec_add(lifted, tagged_hash("TapTweak", internal)))
                return false;

            const auto output = unsafe_array_cast<uint8_t, ec_xonly_size>(
                std::next(lifted.data()));
            out.push_back(
                { { script::to_pay_witness_taproot_pattern(output) },
                {}, {}, {} });
            return true;
        }
        case function::addr:
        {
            const payment_address base58{ tree.address };
            if (base58)
            {
                out.push_back({ base58.output_script(), {}, {}, {} });
                return true;
            }

            const witness_address witness{ tree.address };
            if (witness)
            {
                out.push_back({ witness.script(), {}, {}, {} });
                return true;
            }

            return false;
        }
        case function::raw:
        {
            const script script{ tree.raw, false };
            if (!script.is_valid())
                return false;

            out.push_back({ script, {}, {}, {} });
            return true;
        }
        default:
            return false;
    }
}

// Constructors.
// ----------------------------------------------------------------------------

descriptor::descriptor() NOEXCEPT
{
}

descriptor::descriptor(const std::string& expression) NOEXCEPT
  : descriptor(from_string(expression))
{
}

descriptor::descriptor(bool valid, std::string&& body,
    node&& tree) NOEXCEPT
  : valid_(valid), body_(std::move(body)), tree_(std::move(tree))
{
}

descriptor descriptor::from_string(const std::string& expression) NOEXCEPT
{
    auto body = expression;
    const auto hash = expression.find('#');
    if (hash != std::string::npos)
    {
        body = expression.substr(0, hash);
        if (expression.substr(add1(hash)) != to_checksum(body))
            return {};
    }

    node tree{};
    if (!parse(tree, body))
        return {};

    return { true, std::move(body), std::move(tree) };
}

// Operators.
// ----------------------------------------------------------------------------

descriptor::operator bool() const NOEXCEPT
{
    return valid_;
}

bool descriptor::operator==(const descriptor& other) const NOEXCEPT
{
    return valid_ == other.valid_ && body_ == other.body_;
}

bool descriptor::operator!=(const descriptor& other) const NOEXCEPT
{
    return !(*this == other);
}

// Serializer.
// ----------------------------------------------------------------------------

std::string descriptor::encoded() const NOEXCEPT
{
    return body_ + "#" + to_checksum(body_);
}

// Accessors.
// ----------------------------------------------------------------------------

std::string descriptor::checksum() const NOEXCEPT
{
    return to_checksum(body_);
}

bool descriptor::ranged() const NOEXCEPT
{
    for (auto tree = &tree_; tree != nullptr; tree = tree->child.get())
        for (const auto& key: tree->keys)
            if (key.wildcard)
                return true;

    return false;
}

bool descriptor::has_private_keys() const NOEXCEPT
{
    for (auto tree = &tree_; tree != nullptr; tree = tree->child.get())
        for (const auto& key: tree->keys)
            if (key.form == key_form::extended_private ||
                key.form == key_form::secret)
                return true;

    return false;
}

bool descriptor::solvable() const NOEXCEPT
{
    if (!valid_)
        return false;

    for (auto tree = &tree_; tree != nullptr; tree = tree->child.get())
        if (tree->type == function::addr || tree->type == function::raw)
            return false;

    return true;
}

// Methods.
// ----------------------------------------------------------------------------

chain::scripts descriptor::scripts(uint32_t index) const NOEXCEPT
{
    chain::scripts out{};
    for (auto& item: signings(index))
        out.push_back(std::move(item.script));

    return out;
}

descriptor::signing::list descriptor::signings(uint32_t index) const NOEXCEPT
{
    signing::list out{};
    if (!valid_ || !derive_signings(out, tree_, index, true))
        return {};

    return out;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace wallet
} // namespace system
} // namespace libbitcoin
