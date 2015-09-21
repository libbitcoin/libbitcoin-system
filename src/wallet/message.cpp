/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/message.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/wallet/wif_keys.hpp>

namespace libbitcoin {
namespace wallet {

constexpr uint8_t max_recovery_id = 3;
constexpr uint8_t magic_compressed = 31;
constexpr uint8_t magic_uncompressed = 27;
constexpr uint8_t magic_differential = magic_compressed - magic_uncompressed;
static_assert(magic_differential > max_recovery_id, "oops!");
static_assert(MAX_UINT8 - max_recovery_id >= magic_uncompressed, "oops!");

static hash_digest hash_message(data_slice message)
{
    // This is a specified magic prefix.
    static const std::string prefix("Bitcoin Signed Message:\n");

    data_chunk data;
    data_sink ostream(data);
    ostream_writer sink(ostream);
    sink.write_string(prefix);
    sink.write_variable_uint_little_endian(message.size());
    sink.write_data(message);
    ostream.flush();
    return bitcoin_hash(data);
}

static bool recover(short_hash& out_hash, bool compressed,
    const compact_signature& compact, uint8_t recovery_id,
    const hash_digest& message_digest)
{
    if (compressed)
    {
        ec_compressed point;
        if (!recover_public(point, compact, recovery_id, message_digest))
            return false;

        out_hash = bitcoin_short_hash(point);
        return true;
    }

    ec_uncompressed point;
    if (!recover_public(point, compact, recovery_id, message_digest))
        return false;

    out_hash = bitcoin_short_hash(point);
    return true;
}

bool recovery_id_to_magic(uint8_t& out_magic, uint8_t recovery_id,
    bool compressed)
{
    if (recovery_id > max_recovery_id)
        return false;

    // Offset the recovery id with sentinels to indication compression state.
    const auto increment = compressed ? magic_compressed : magic_uncompressed;
    out_magic = recovery_id + increment;
    return true;
}

bool magic_to_recovery_id(uint8_t& out_recovery_id, bool& out_compressed,
    uint8_t magic)
{
    // Magic less offsets cannot exceed recovery id range [0, max_recovery_id].
    if (magic < magic_uncompressed ||
        magic > magic_compressed + max_recovery_id)
        return false;

    // Subtract smaller sentinel (guarded above).
    auto recovery_id = magic - magic_uncompressed;

    // Obtain compression state (differential exceeds the recovery id range).
    out_compressed = recovery_id >= magic_differential;

    // If compression is indicated subtract differential (guarded above).
    if (out_compressed)
        recovery_id -= magic_differential;

    out_recovery_id = static_cast<uint8_t>(recovery_id);
    return true;
}

bool sign_message(message_signature& signature, data_slice message,
    const std::string& wif)
{
    bool compressed;
    uint8_t version;
    ec_secret secret;
    if (!decode_wif(secret, version, compressed, wif))
        return false;

    return sign_message(signature, message, secret, compressed);
}

bool sign_message(message_signature& signature, data_slice message,
    const ec_secret& secret, bool compressed)
{
    uint8_t recovery_id;
    compact_signature compact;
    if (!sign_compact(compact, recovery_id, secret, hash_message(message)))
        return false;

    uint8_t magic;
    if (!recovery_id_to_magic(magic, recovery_id, compressed))
        return false;

    signature = splice(to_array(magic), compact);
    return true;
}

bool verify_message(data_slice message, const payment_address& address,
    const message_signature& signature)
{
    const auto magic = signature.front();
    const auto compact = slice<1, message_signature_size>(signature);

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
} // namespace libbitcoin
