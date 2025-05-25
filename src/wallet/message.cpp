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
#include <bitcoin/system/wallet/message.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

static constexpr uint8_t max_recovery_id = 3;
static constexpr uint8_t magic_compressed = 31;
static constexpr uint8_t magic_uncompressed = 27;
static constexpr uint8_t magic_differential = magic_compressed - magic_uncompressed;
static_assert(magic_differential > max_recovery_id);
static_assert(max_uint8 - max_recovery_id >= magic_uncompressed);

hash_digest hash_message(const data_slice& message) NOEXCEPT
{
    // This is a specified magic prefix.
    static const std::string prefix("Bitcoin Signed Message:\n");

    hash_digest hash{};
    stream::out::fast stream{ hash };
    hash::sha256x2::fast sink(stream);
    sink.write_string(prefix);
    sink.write_variable(message.size());
    sink.write_bytes(message);
    sink.flush();
    return hash;
}

static bool recover(short_hash& out_hash, bool compressed,
    const ec_signature& compact, uint8_t recovery_id,
    const hash_digest& message_digest) NOEXCEPT
{
    const recoverable_signature recoverable
    {
        compact,
        recovery_id
    };

    if (compressed)
    {
        ec_compressed point;
        if (!ecdsa::recover_public(point, recoverable, message_digest))
            return false;

        out_hash = bitcoin_short_hash(point);
        return true;
    }

    ec_uncompressed point;
    if (!ecdsa::recover_public(point, recoverable, message_digest))
        return false;

    out_hash = bitcoin_short_hash(point);
    return true;
}

bool recovery_id_to_magic(uint8_t& out_magic, uint8_t recovery_id,
    bool compressed) NOEXCEPT
{
    if (recovery_id > max_recovery_id)
        return false;

    // Offset the recovery id with sentinels to indication compression state.
    const auto increment = compressed ? magic_compressed : magic_uncompressed;
    out_magic = recovery_id + increment;
    return true;
}

bool magic_to_recovery_id(uint8_t& out_recovery_id, bool& out_compressed,
    uint8_t magic) NOEXCEPT
{
    // Magic less offsets cannot exceed recovery id range [0, max_recovery_id].
    if (magic < magic_uncompressed ||
        magic > magic_compressed + max_recovery_id)
        return false;

    // Subtract smaller sentinel (guarded above).
    out_recovery_id = magic - magic_uncompressed;

    // Obtain compression state (differential exceeds the recovery id range).
    out_compressed = out_recovery_id >= magic_differential;

    // If compression is indicated subtract differential (guarded above).
    if (out_compressed)
        out_recovery_id -= magic_differential;

    return true;
}

bool sign_message(message_signature& out_signature, const data_slice& message,
    const ec_private& secret) NOEXCEPT
{
    return sign_message(out_signature, message, secret, secret.compressed());
}

bool sign_message(message_signature& out_signature, const data_slice& message,
    const std::string& wif) NOEXCEPT
{
    ec_private secret(wif);
    return (secret &&
        sign_message(out_signature, message, secret, secret.compressed()));
}

bool sign_message(message_signature& out_signature, const data_slice& message,
    const ec_secret& secret, bool compressed) NOEXCEPT
{
    recoverable_signature recoverable;
    if (!ecdsa::sign_recoverable(recoverable, secret, hash_message(message)))
        return false;

    uint8_t magic;
    if (!recovery_id_to_magic(magic, recoverable.recovery_id, compressed))
        return false;

    out_signature = splice(to_array(magic), recoverable.signature);
    return true;
}

bool verify_message(const data_slice& message, const payment_address& address,
    const message_signature& signature) NOEXCEPT
{
    const auto magic = signature.front();
    const auto compact = slice<one, message_signature_size>(signature);

    bool compressed;
    uint8_t recovery_id;
    if (!magic_to_recovery_id(recovery_id, compressed, magic))
        return false;

    short_hash hash;
    const auto message_digest = hash_message(message);
    return recover(hash, compressed, compact, recovery_id, message_digest) &&
        (hash == address.hash());
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
