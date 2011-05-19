#include "net/dialects/original.h"

#include "net/serializer.h"
#include "net/messages.h"

namespace libbitcoin {
namespace net {

static serializer::stream construct_header_from(serializer::stream payload, 
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
    if (command != "version" && command != "verack") {
        //uint32_t checksum = 0;
        //write_to_stream(header, checksum);
    }
    return header.get_data();
}

const serializer::stream original_dialect::translate(message::version version) const
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

} // net
} // libbitcoin

