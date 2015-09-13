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

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/wallet/wif_keys.hpp>

namespace libbitcoin {
namespace wallet {

hash_digest hash_message(data_slice message)
{
    std::string prefix = "Bitcoin Signed Message:\n";

    size_t size = variable_uint_size(prefix.size()) + prefix.size() +
        variable_uint_size(message.size()) + message.size();
    data_chunk data(size);

    auto ser = make_serializer(data.begin());
    ser.write_string(prefix);
    ser.write_variable_uint_little_endian(message.size());
    ser.write_data(message);

    return bitcoin_hash(data);
}

// This is convenient thought not as efficient as the other override.
message_signature sign_message(data_slice message, const std::string& wif)
{
    auto secret = wif_to_secret(wif);
    if (secret == ec_secret())
        return message_signature();

    bool compressed = is_wif_compressed(wif);

    return sign_message(message, secret, compressed);
}

message_signature sign_message(data_slice message,
    const ec_secret& secret, bool compressed)
{
    auto hash = hash_message(message);

    auto cs = sign_compact(secret, hash);

    int magic = 27 + cs.recid;
    if (compressed)
        magic += 4;

    message_signature out;
    out[0] = magic;
    std::copy(cs.signature.begin(), cs.signature.end(), out.begin() + 1);
    return out;
}

bool verify_message(data_slice message,
    const payment_address& address, const message_signature& signature)
{
    auto hash = hash_message(message);

    bool compressed = false;
    int magic = signature[0] - 27;
    if (magic < 0 || 8 <= magic)
    {
        return false;
    }
    if (4 <= magic)
    {
        compressed = true;
        magic -= 4;
    }

    compact_signature cs;
    std::copy(signature.begin() + 1, signature.end(), cs.signature.begin());
    cs.recid = magic;
    ec_point pubkey = recover_compact(cs, hash, compressed);

    return address.hash() == bitcoin_short_hash(pubkey);
}

} // namespace wallet
} // namespace libbitcoin

