/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_HEADER_HPP
#define LIBBITCOIN_MESSAGE_HEADER_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

// List of bitcoin messages
// ------------------------
// version
// verack
// getaddr
// addr
// inv
// getdata
// getblocks
// block
// tx
// ping
// pong
// notfound
// reject       [not yet supported]
// getheaders   [not yet supported]
// headers      [not yet supported]
// mempool      [BIP35: not yet supported]
// filterload   [BIP37: no support intended]
// filteradd    [BIP37: no support intended]
// filterclear  [BIP37: no support intended]
// merkleblock  [BIP37: no support intended]
// checkorder   [deprecated in protocol]
// submitorder  [deprecated in protocol]
// reply        [deprecated in protocol]
// alert        [no support intended]

namespace libbitcoin {
namespace message {

class BC_API header
{
public:
    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static BC_CONSTEXPR size_t header_size = 20;
    static BC_CONSTEXPR size_t checksum_size = 4;

    // boost1.54/linux/clang/libstdc++-4.8 error if std::array
    // could not match 'boost::array' against 'std::array'
    typedef std::array<uint8_t, header_size> header_bytes;
    typedef std::array<uint8_t, checksum_size> checksum_bytes;

    static header factory_from_data(const data_chunk& data);
    static header factory_from_data(std::istream& stream);
    static header factory_from_data(reader& source);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t satoshi_size() const;

    uint32_t magic;
    std::string command;
    uint32_t payload_length;
    uint32_t checksum;
};

BC_API bool operator==(const header& left, const header& right);
BC_API bool operator!=(const header& left, const header& right);

template <typename Message>
data_chunk create_raw_message(const Message& packet)
{
    // Serialize the payload (required for header size).
    auto payload = packet.to_data();

    // Construct the header.
    header payload_header;
    payload_header.magic = bc::magic_value;
    payload_header.command = Message::satoshi_command;
    payload_header.payload_length = static_cast<uint32_t>(payload.size());
    payload_header.checksum = bitcoin_checksum(payload);

    // Serialize header and copy the payload into a single message buffer.
    auto message = payload_header.to_data();
    extend_data(message, payload);
    return message;
}

} // namspace message
} // namspace libbitcoin

#endif
