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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_ECDSA_SIGNATURES_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_ECDSA_SIGNATURES_HPP

#include <span>
#include <bitcoin/system/chain/batch/multisig.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_USE_OF_SPAN)

/// Accumulator of captured ecdsa verification groups (variable-width AoS).
/// Each record is one group: a single sigop (1 of 1) or a multisig band
/// (m of n). The group id is the record ordinal, capped at max_uint16. The
/// store expands each group to its banded rows upon commit.
class ecdsa_signatures
{
public:
    /// Append one single sigop (1 of 1) group record, false implies group
    /// capacity (decline).
    inline bool append(const hash_digest& digest, const ec_compressed& key,
        const ec_signature& signature) NOEXCEPT
    {
        if (!put(digest, { &key, one }, { &signature, one }))
            return false;

        ++singles_;
        return true;
    }

    /// Append one multisig group record (m of n band, including 1 of 1),
    /// false implies group capacity (decline).
    inline bool append(const hash_digest& digest,
        const std::span<const ec_compressed>& keys,
        const std::span<const ec_signature>& sigs) NOEXCEPT
    {
        if (!put(digest, keys, sigs))
            return false;

        keys_ += keys.size();
        return true;
    }

    /// Visit each group in capture order (group id is the visit ordinal).
    /// Handler: void(const hash_digest&, std::span<const ec_compressed>,
    /// std::span<const ec_signature>).
    template <typename Handler>
    inline void for_each(Handler&& handler) const NOEXCEPT
    {
        BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
        auto it = log_.data();
        BC_POP_WARNING()

        const auto end = std::next(it, to_signed(log_.size()));
        while (it != end)
        {
            BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
            const size_t sigs_count = *it++;
            const size_t keys_count = *it++;
            BC_POP_WARNING()

            const auto& digest = unsafe_array_cast<uint8_t, hash_size>(it);
            std::advance(it, to_signed(hash_size));
            const std::span<const ec_compressed> keys
            {
                pointer_cast<const ec_compressed>(it), keys_count
            };
            std::advance(it, to_signed(keys_count * ec_compressed_size));
            const std::span<const ec_signature> sigs
            {
                pointer_cast<const ec_signature>(it), sigs_count
            };
            std::advance(it, to_signed(sigs_count * ec_signature_size));
            handler(digest, keys, sigs);
        }
    }

    /// Group (record) count, the group id domain.
    inline size_t groups() const NOEXCEPT
    {
        return groups_;
    }

    /// Total banded row expansion across all groups (store rows).
    inline size_t rows() const NOEXCEPT
    {
        return rows_;
    }

    /// Single sigop capture count (reporting only).
    inline size_t singles() const NOEXCEPT
    {
        return singles_;
    }

    /// Total keys across multisig captures (reporting only).
    inline size_t multisig_keys() const NOEXCEPT
    {
        return keys_;
    }

    /// True if no groups are captured.
    inline bool empty() const NOEXCEPT
    {
        return is_zero(groups_);
    }

    /// Verify all groups in place (store commit failure fallback). Each group
    /// applies the sequential op_checkmultisig trial (1 of 1 degenerates to a
    /// single verify), exactly the evaluation its capture fabricated.
    bool verify() const NOEXCEPT
    {
        auto success = true;
        for_each([&](const hash_digest& digest,
            const std::span<const ec_compressed>& keys,
            const std::span<const ec_signature>& sigs) NOEXCEPT
        {
            success = verify_group(digest, keys, sigs) && success;
        });

        return success;
    }

    /// Reset for next block, retaining capacity.
    inline void clear() NOEXCEPT
    {
        log_.clear();
        groups_ = zero;
        rows_ = zero;
        singles_ = zero;
        keys_ = zero;
    }

    /// Reset, releasing capacity (see signatures::purge).
    inline void purge() NOEXCEPT
    {
        clear();
        log_.shrink_to_fit();
    }

protected:
    inline bool put(const hash_digest& digest,
        const std::span<const ec_compressed>& keys,
        const std::span<const ec_signature>& sigs) NOEXCEPT
    {
        const auto sigs_count = sigs.size();
        const auto keys_count = keys.size();
        BC_ASSERT(multisig::check(sigs_count, keys_count));

        if (groups_ > max_uint16)
            return false;

        log_.push_back(possible_narrow_cast<uint8_t>(sigs_count));
        log_.push_back(possible_narrow_cast<uint8_t>(keys_count));
        append(digest);
        for (const auto& key: keys) append(key);
        for (const auto& sig: sigs) append(sig);

        ++groups_;
        rows_ += multisig::rows(sigs_count, keys_count);
        return true;
    }

    template <size_t Size>
    inline void append(const data_array<Size>& bytes) NOEXCEPT
    {
        log_.insert(log_.end(), bytes.begin(), bytes.end());
    }

    static bool verify_group(const hash_digest& digest,
        const std::span<const ec_compressed>& keys,
        const std::span<const ec_signature>& sigs) NOEXCEPT
    {
        auto key = keys.begin();
        auto sig = sigs.begin();
        while ((sig != sigs.end()) &&
            (std::distance(key, keys.end()) >= std::distance(sig, sigs.end())))
        {
            if (ecdsa::verify_signature(*key, digest, *sig)) ++sig;
            ++key;
        }

        return sig == sigs.end();
    }

private:
    data_chunk log_{};
    size_t groups_{};
    size_t rows_{};
    size_t singles_{};
    size_t keys_{};
};

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
