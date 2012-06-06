#include <bitcoin/exporter.hpp>

#include <boost/assert.hpp>

#include <bitcoin/messages.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/satoshi_serialize.hpp>

namespace libbitcoin {

const char* satoshi_exporter::command_name(const message::version&) const
{
    return "version";
}
const char* satoshi_exporter::command_name(const message::verack&) const
{
    return "verack";
}
const char* satoshi_exporter::command_name(const message::address&) const
{
    return "addr";
}
const char* satoshi_exporter::command_name(const message::get_address&) const
{
    return "getaddr";
}
const char* satoshi_exporter::command_name(const message::inventory&) const
{
    return "inv";
}
const char* satoshi_exporter::command_name(const message::get_data&) const
{
    return "getdata";
}
const char* satoshi_exporter::command_name(const message::get_blocks&) const
{
    return "getblocks";
}
const char* satoshi_exporter::command_name(const message::block&) const
{
    return "block";
}
const char* satoshi_exporter::command_name(const message::transaction&) const
{
    return "tx";
}
const char* satoshi_exporter::command_name(const message::ping&) const
{
    return "ping";
}

data_chunk satoshi_exporter::save(
    const message::header& packet_header) const
{
    data_chunk result(satoshi_raw_size(packet_header));
    message::satoshi_save(packet_header, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::version& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::verack& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::address& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::get_data& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::get_blocks& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::get_address& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::inventory& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::transaction& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::block& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

data_chunk satoshi_exporter::save(const message::ping& packet) const
{
    data_chunk result(satoshi_raw_size(packet));
    message::satoshi_save(packet, result.begin());
    return result;
}

message::header satoshi_exporter::load_header(const data_chunk& stream)  const
{
    message::header header;
    satoshi_load(stream.begin(), stream.end(), header);
    return header;
}

message::version satoshi_exporter::load_version(const data_chunk& stream) const
{
    message::version packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::verack satoshi_exporter::load_verack(const data_chunk& stream) const
{
    message::verack packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::address satoshi_exporter::load_address(const data_chunk& stream) const
{
    message::address packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::get_address satoshi_exporter::load_get_address(
    const data_chunk& stream) const
{
    message::get_address packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::inventory satoshi_exporter::load_inventory(
    const data_chunk& stream) const
{
    message::inventory packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::get_data satoshi_exporter::load_get_data(
    const data_chunk& stream) const
{
    message::get_data packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::get_blocks satoshi_exporter::load_get_blocks(
    const data_chunk& stream) const
{
    message::get_blocks packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::transaction satoshi_exporter::load_transaction(
    const data_chunk& stream) const
{
    message::transaction packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::block satoshi_exporter::load_block(const data_chunk& stream) const
{
    message::block packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

message::ping satoshi_exporter::load_ping(const data_chunk& stream) const
{
    message::ping packet;
    satoshi_load(stream.begin(), stream.end(), packet);
    return packet;
}

bool satoshi_exporter::verify_header(const message::header& header_msg) const
{
    if (header_msg.magic != magic_value)
        return false;
    if (header_msg.command == "version")
    {
        if (header_msg.payload_length < 85)
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
        // Also then add ASSERTS to handlers above.
    }
    else
    {
        // Unknown header
        return false;
    }
    return true;
}

} // namespace libbitcoin

