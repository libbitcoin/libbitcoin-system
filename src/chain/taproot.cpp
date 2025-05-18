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
#include <bitcoin/system/chain/taproot.hpp>

#include <algorithm>
#include <iterator>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// protected
// ----------------------------------------------------------------------------

hash_digest taproot::merkle_root(const data_chunk& control,
    const hash_digest& tapleaf_hash) NOEXCEPT
{
    BC_ASSERT(is_valid_control_block(control));
    constexpr auto start = add1(ec_xonly_size);
    const auto bytes = floored_subtract(control.size(), start);
    const auto count = floored_divide(bytes, ec_xonly_size);
    const auto begin = std::next(control.data(), start);
    const auto nodes = unsafe_array_cast<ec_xonly, taproot_max_keys>(begin);

    hash_digest hash{ tapleaf_hash };
    for (size_t node{}; node < count; ++node)
        hash = branch_hash(hash, nodes.at(node));

    return hash;
}

// TapBranch
hash_digest taproot::branch_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapBranch"> sink{ stream };

    if (std::lexicographical_compare(left.begin(), left.end(),
        right.begin(), right.end()))
    {
        sink.write_bytes(left);
        sink.write_bytes(right);
    }
    else
    {
        sink.write_bytes(right);
        sink.write_bytes(left);
    }

    sink.flush();
    return out;
}

// TapTweak
hash_digest taproot::tweak_hash(const ec_xonly& key,
    const hash_digest& merkle) NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapTweak"> sink{ stream };
    sink.write_bytes(key);
    sink.write_bytes(merkle);
    sink.flush();
    return out;
}

// public
// ----------------------------------------------------------------------------

// TapLeaf
hash_digest taproot::leaf_hash(uint8_t version,
    const script& script) NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapLeaf"> sink{ stream };
    sink.write_byte(version);
    script.to_data(sink, true);
    sink.flush();
    return out;
}

taproot::tap taproot::parse(const data_chunk& control) NOEXCEPT
{
    BC_ASSERT(!control.empty());
    const auto byte = control.front();
    return
    {
        bit_and(byte, tapscript_mask),
        to_bool(bit_and(byte, bit_not(tapscript_mask)))
    };
}

bool taproot::is_valid_control_block(const data_chunk& control) NOEXCEPT
{
    const auto size = control.size();
    constexpr auto max = add1(ec_xonly_size) + ec_xonly_size * taproot_max_keys;

    // Control block must be add1(32) + 32m, for integer m [0..128] [bip341].
    return !is_limited(size, add1(ec_xonly_size), max) && is_zero(
        floored_modulo(size - add1(ec_xonly_size), ec_xonly_size));
}

bool taproot::verify_commitment(const data_chunk& control,
    const data_chunk& program, const hash_digest& hash,
    bool parity) NOEXCEPT
{
    BC_ASSERT(is_valid_control_block(control));
    const auto out = program.data();
    const auto& out_key = unsafe_array_cast<uint8_t, ec_xonly_size>(out);
    const auto in = std::next(control.data());
    const auto& in_key = unsafe_array_cast<uint8_t, ec_xonly_size>(in);
    const auto root = merkle_root(control, hash);
    const auto tweak = tweak_hash(out_key, root);
    return schnorr::verify_commitment(in_key, tweak, out_key, parity);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
