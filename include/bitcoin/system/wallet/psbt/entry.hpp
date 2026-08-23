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
#ifndef LIBBITCOIN_SYSTEM_WALLET_PSBT_ENTRY_HPP
#define LIBBITCOIN_SYSTEM_WALLET_PSBT_ENTRY_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

/// Global map key types, as defined by BIP174/BIP370.
enum class global_key : uint8_t
{
    unsigned_tx = 0x00,
    xpub = 0x01,
    tx_version = 0x02,
    fallback_locktime = 0x03,
    input_count = 0x04,
    output_count = 0x05,
    tx_modifiable = 0x06,
    version = 0xfb,
    proprietary = 0xfc
};

/// Input map key types, as defined by BIP174/BIP370/BIP371.
enum class input_key : uint8_t
{
    non_witness_utxo = 0x00,
    witness_utxo = 0x01,
    partial_sig = 0x02,
    sighash_type = 0x03,
    redeem_script = 0x04,
    witness_script = 0x05,
    bip32_derivation = 0x06,
    final_script_sig = 0x07,
    final_script_witness = 0x08,
    por_commitment = 0x09,
    ripemd160 = 0x0a,
    sha256 = 0x0b,
    hash160 = 0x0c,
    hash256 = 0x0d,
    previous_txid = 0x0e,
    output_index = 0x0f,
    sequence = 0x10,
    required_time_locktime = 0x11,
    required_height_locktime = 0x12,
    tap_key_sig = 0x13,
    tap_script_sig = 0x14,
    tap_leaf_script = 0x15,
    tap_bip32_derivation = 0x16,
    tap_internal_key = 0x17,
    tap_merkle_root = 0x18,
    proprietary = 0xfc
};

/// Output map key types, as defined by BIP174/BIP370/BIP371.
enum class output_key : uint8_t
{
    redeem_script = 0x00,
    witness_script = 0x01,
    bip32_derivation = 0x02,
    amount = 0x03,
    script = 0x04,
    tap_internal_key = 0x05,
    tap_tree = 0x06,
    tap_bip32_derivation = 0x07,
    proprietary = 0xfc
};

/// A raw key-value pair, preserving non-structural and unknown fields.
struct BC_API entry
{
    typedef std_vector<entry> list;

    /// Parse one pair, false on the map terminator or exhaustion.
    bool from_data(reader& source) NOEXCEPT;

    /// Serialize the pair.
    void to_data(writer& sink) const NOEXCEPT;

    /// The key type (leading compact size).
    uint64_t type() const NOEXCEPT;

    /// The key excluding its type prefix.
    data_chunk keydata() const NOEXCEPT;

    data_chunk key;
    data_chunk value;

    bool operator==(const entry& other) const NOEXCEPT
    {
        return key == other.key && value == other.value;
    }

    bool operator!=(const entry& other) const NOEXCEPT
    {
        return !(*this == other);
    }
};

/// A BIP32 key origin (master fingerprint and derivation path).
struct BC_API key_origin
{
    /// Parse the BIP174 value encoding, false on malformed.
    bool from_value(const data_chunk& value) NOEXCEPT;

    /// Serialize to the BIP174 value encoding.
    data_chunk to_value() const NOEXCEPT;

    uint32_t fingerprint;
    std_vector<uint32_t> path;

    bool operator==(const key_origin& other) const NOEXCEPT
    {
        return fingerprint == other.fingerprint && path == other.path;
    }

    bool operator!=(const key_origin& other) const NOEXCEPT
    {
        return !(*this == other);
    }
};

/// A public key origin (input/output bip32_derivation).
struct BC_API derivation
{
    typedef std_vector<derivation> list;

    data_chunk point;
    key_origin origin;

    bool operator==(const derivation& other) const NOEXCEPT
    {
        return point == other.point && origin == other.origin;
    }

    bool operator!=(const derivation& other) const NOEXCEPT
    {
        return !(*this == other);
    }
};

/// An extended public key origin (global xpub).
struct BC_API xpub
{
    typedef std_vector<xpub> list;

    data_chunk key;
    key_origin origin;

    bool operator==(const xpub& other) const NOEXCEPT
    {
        return key == other.key && origin == other.origin;
    }

    bool operator!=(const xpub& other) const NOEXCEPT
    {
        return !(*this == other);
    }
};

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
