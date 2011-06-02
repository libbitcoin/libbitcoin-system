#include "dialect.hpp"

#include <boost/assert.hpp>

#include "bitcoin/net/messages.hpp"
#include "bitcoin/util/assert.hpp"

namespace libbitcoin {
namespace net {

serializer::stream construct_header_from(serializer::stream payload, 
        std::string command)
{
    serializer header;
    // magic
    header.write_4_bytes(0xd9b4bef9);
    // command
    header.write_command(command);
    // payload length
    uint32_t length = payload.size();
    header.write_4_bytes(length);
    // checksum is not in verson or verack
    if (command != "version" && command != "verack") 
    {
        //uint32_t checksum = 0;
        //write_to_stream(header, checksum);
    }
    return header.get_data();
}

serializer::stream header_only_message(std::string command)
{
    serializer payload;
    serializer::stream msg_body = payload.get_data();
    // No data
    serializer::stream header = construct_header_from(msg_body, "verack");
    return header;
}

serializer::stream original_dialect::to_network(
        message::version version) const
{
    serializer payload;
    payload.write_4_bytes(version.version);
    payload.write_8_bytes(version.services);
    payload.write_8_bytes(version.timestamp);
    payload.write_net_addr(version.addr_me);
    payload.write_net_addr(version.addr_you);
    payload.write_8_bytes(version.nonce);
    // do sub_version_num
    payload.write_byte(0);
    payload.write_4_bytes(version.start_height);

    serializer::stream msg_body = payload.get_data();
    serializer::stream message = 
            construct_header_from(msg_body, "version");
    // Extend message with actual payload
    message.reserve(message.size() + distance(msg_body.begin(), msg_body.end()));
    message.insert(message.end(), msg_body.begin(), msg_body.end());
    return message;
}

serializer::stream original_dialect::to_network(message::verack verack) const
{
    return header_only_message("verack");
}

message::header original_dialect::header_from_network(
        const serializer::stream& stream)  const
{
    deserializer deserial(stream);
    message::header header;
    header.magic = deserial.read_4_bytes();
    header.command = deserial.read_fixed_len_str(12);
    header.payload_length = deserial.read_4_bytes();
    return header;
}

message::version original_dialect::version_from_network(
        const serializer::stream& stream) const
{
    deserializer deserial(stream);
    message::version payload;
    payload.version = deserial.read_4_bytes();
    payload.services = deserial.read_8_bytes();
    payload.timestamp = deserial.read_8_bytes();
    payload.addr_me = deserial.read_net_addr();
    if (payload.version < 106) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26);
        return payload;
    }
    payload.addr_you = deserial.read_net_addr();
    payload.nonce = deserial.read_8_bytes();
    // sub_version_num
    payload.sub_version_num = deserial.read_byte();
    if (payload.version < 209) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1);
        return payload;
    }
    payload.start_height = deserial.read_4_bytes();
    BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4);
    return payload;
}

bool original_dialect::verify_header(net::message::header header_msg) const
{
    if (header_msg.magic != 0xd9b4bef9)
        return false;
    if (header_msg.command == "version")
    {
        if (header_msg.payload_length != 85)
            return false;
    }
    else if (header_msg.command == "verack"
            || header_msg.command == "getaddr"
            || header_msg.command == "ping")
    {
        if (header_msg.payload_length != 0)
            return false;
    }
    else if (header_msg.command == "inv"
            || header_msg.command == "addr"
            || header_msg.command == "getdata"
            || header_msg.command == "getblocks"
            || header_msg.command == "getheaders"
            || header_msg.command == "tx"
            || header_msg.command == "block"
            || header_msg.command == "headers"
            || header_msg.command == "alert")
    {
        // Should check if sizes make sense
        // i.e for addr should be multiple of 30x + 1 byte
    }
    return true;
}

} // net
} // libbitcoin

