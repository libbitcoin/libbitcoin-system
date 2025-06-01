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
#include <bitcoin/system/chain/annex.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/tapscript.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// protected
// ----------------------------------------------------------------------------

hash_digest taproot::merkle_root(const tapscript::keys_t& keys, size_t count,
    const hash_digest& tapleaf_hash) NOEXCEPT
{
    hash_digest hash{ tapleaf_hash };
    for (size_t key{}; key < count; ++key)
        hash = sorted_branch_hash(hash, keys.at(key));

    return hash;
}

hash_digest taproot::sorted_branch_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    return std::lexicographical_compare(left.begin(), left.end(),
        right.begin(), right.end()) ? branch_hash(left, right) :
        branch_hash(right, left);
}

// TapBranch
hash_digest taproot::branch_hash(const hash_digest& first,
    const hash_digest& second) NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapBranch"> sink{ stream };
    sink.write_bytes(first);
    sink.write_bytes(second);
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

// static
bool taproot::drop_annex(chunk_cptrs& stack) NOEXCEPT
{
    if (!annex::is_annex_pattern(stack))
        return false;

    stack.pop_back();
    return true;
}

bool taproot::verify_commit(const tapscript& control, const ec_xonly& out_key,
    const hash_digest& leaf) NOEXCEPT
{
    using namespace schnorr;
    const auto root = merkle_root(control.keys(), control.count(), leaf);
    const auto tweak = tweak_hash(control.key(), root);
    return verify_commitment(control.key(), tweak, out_key, control.parity());
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
