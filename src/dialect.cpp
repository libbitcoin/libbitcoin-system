#include <bitcoin/dialect.hpp>

#include <boost/assert.hpp>

#include <bitcoin/messages.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/sha256.hpp>

namespace libbitcoin {

data_chunk construct_header_from(std::string command,
    const data_chunk& payload)
{
    log_info(log_domain::network) << "s: " << command
            << " (" << payload.size() << " bytes)";
    serializer header;
    // magic
    header.write_4_bytes(magic_value);
    // command
    header.write_command(command);
    // payload length
    uint32_t length = payload.size();
    header.write_4_bytes(length);
    // checksum is not in verson or verack
    if (command != "version" && command != "verack")
    {
        uint32_t checksum = generate_sha256_checksum(payload);
        header.write_4_bytes(checksum);
    }
    return header.get_data();
}

data_chunk assemble_message(std::string command, const serializer& payload,
    bool include_header)
{
    data_chunk msg_body = payload.get_data();
    if (!include_header)
        return msg_body;
    data_chunk message = construct_header_from(command, msg_body);
    // Extend message with actual payload
    message.reserve(
        message.size() + distance(msg_body.begin(), msg_body.end()));
    message.insert(message.end(), msg_body.begin(), msg_body.end());
    return message;
}

data_chunk header_only_message(std::string command)
{
    serializer payload;
    data_chunk msg_body = payload.get_data();
    // No data
    data_chunk header = construct_header_from(command, msg_body);
    return header;
}

data_chunk original_dialect::to_network(const message::version& version) const
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
    return assemble_message("version", payload, true);
}

data_chunk original_dialect::to_network(const message::verack&) const
{
    return header_only_message("verack");
}

data_chunk original_dialect::to_network(const message::getaddr&) const
{
    return header_only_message("getaddr");
}

data_chunk original_dialect::to_network(
    const message::getblocks& getblocks) const
{
    serializer payload;
    payload.write_4_bytes(31900);
    payload.write_var_uint(getblocks.locator_start_hashes.size());
    for (hash_digest start_hash: getblocks.locator_start_hashes)
        payload.write_hash(start_hash);
    payload.write_hash(getblocks.hash_stop);
    return assemble_message("getblocks", payload, true);
}

data_chunk original_dialect::to_network(const message::block& block, 
        bool include_header) const
{
    serializer payload;
    return assemble_message("block", payload, include_header);
}

data_chunk original_dialect::to_network(const message::transaction& tx, 
        bool include_header) const
{
    serializer payload;
    return assemble_message("tx", payload, include_header);
}

data_chunk original_dialect::to_network(const message::getdata& getdata) const
{
    serializer payload;
    payload.write_var_uint(getdata.invs.size());
    for (const message::inv_vect inv: getdata.invs)
    {
        switch (inv.type)
        {
            case message::inv_type::transaction:
                payload.write_4_bytes(1);
                break;
            case message::inv_type::block:
                payload.write_4_bytes(2);
                break;
            case message::inv_type::error:
            case message::inv_type::none:
            default:
                BITCOIN_ASSERT(0);
                break;
        }
        payload.write_hash(inv.hash);
    }
    return assemble_message("getdata", payload, true);
}

message::header original_dialect::header_from_network(
        const data_chunk& stream)  const
{
    deserializer deserial(stream);
    message::header header;
    header.magic = deserial.read_4_bytes();
    header.command = deserial.read_fixed_len_str(12);
    header.payload_length = deserial.read_4_bytes();
    header.checksum = 0;
    return header;
}

uint32_t original_dialect::checksum_from_network(const data_chunk& chunk) const
{
    deserializer deserial(chunk);
    return deserial.read_4_bytes();
}

message::version original_dialect::version_from_network(
    const data_chunk& stream, bool& ec) const
{
    ec = false;
    deserializer deserial(stream);
    message::version payload;
    payload.version = deserial.read_4_bytes();
    payload.services = deserial.read_8_bytes();
    payload.timestamp = deserial.read_8_bytes();
    payload.addr_me = deserial.read_net_addr();
    // Ignored field
    payload.addr_me.timestamp = 0;
    if (payload.version < 106) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26);
        return payload;
    }
    payload.addr_you = deserial.read_net_addr();
    // Ignored field
    payload.addr_you.timestamp = 0;
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

message::addr original_dialect::addr_from_network(
    const data_chunk& stream, bool& ec) const
{
    ec = false;
    message::addr payload;
    deserializer deserial(stream);
    uint64_t count = deserial.read_var_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        message::net_addr addr = deserial.read_net_addr();
        addr.timestamp = timestamp;
        payload.addr_list.push_back(addr);
    }
    return payload;
}

message::inv_type inv_type_from_number(uint32_t raw_type)
{
    switch (raw_type)
    {
        case 0:
            return message::inv_type::error;
        case 1:
            return message::inv_type::transaction;
        case 2:
            return message::inv_type::block;
        default:
            return message::inv_type::none;
    }
}

message::inv original_dialect::inv_from_network(
    const data_chunk& stream, bool& ec) const
{
    ec = false;
    deserializer deserial(stream);
    message::inv payload;
    uint64_t count = deserial.read_var_uint();
    for (size_t i = 0; i < count; ++i)
    {
        message::inv_vect inv_vect;
        uint32_t raw_type = deserial.read_4_bytes();
        inv_vect.type = inv_type_from_number(raw_type);
        inv_vect.hash = deserial.read_hash();
        payload.invs.push_back(inv_vect);
    }
    return payload;
}

data_chunk read_raw_script(deserializer& deserial)
{
    data_chunk raw_script;
    uint64_t script_length = deserial.read_var_uint();
    return deserial.read_data(script_length);
}

script read_script(deserializer& deserial)
{
    data_chunk raw_script = read_raw_script(deserial);
    BITCOIN_ASSERT(raw_script == save_script(parse_script(raw_script)));
    // Eventually plan to move parse_script to inside here
    return parse_script(raw_script);
}

message::transaction read_transaction(deserializer& deserial)
{
    message::transaction txn;
    txn.version = deserial.read_4_bytes();
    uint64_t txn_in_count = deserial.read_var_uint();
    for (size_t txn_in_i = 0; txn_in_i < txn_in_count; ++txn_in_i)
    {
        message::transaction_input input;
        input.previous_output.hash = deserial.read_hash();
        input.previous_output.index = deserial.read_4_bytes();
        if (previous_output_is_null(input.previous_output))
            input.input_script = coinbase_script(read_raw_script(deserial));
        else
            input.input_script = read_script(deserial);
        input.sequence = deserial.read_4_bytes();
        txn.inputs.push_back(input);
    }
    uint64_t txn_out_count = deserial.read_var_uint();
    for (size_t txn_out_i = 0; txn_out_i < txn_out_count; ++txn_out_i)
    {
        message::transaction_output output;
        output.value = deserial.read_8_bytes();
        output.output_script = read_script(deserial);
        txn.outputs.push_back(output);
    }
    txn.locktime = deserial.read_4_bytes();
    return txn;
}

message::transaction original_dialect::transaction_from_network(
    const data_chunk& stream, bool& ec) const
{
    ec = false;
    deserializer deserial(stream);
    return read_transaction(deserial);
}

message::block original_dialect::block_from_network(
    const data_chunk& stream, bool& ec) const
{
    ec = false;
    deserializer deserial(stream);
    message::block payload;
    payload.version = deserial.read_4_bytes();
    payload.prev_block = deserial.read_hash();
    payload.merkle_root = deserial.read_hash();
    payload.timestamp = deserial.read_4_bytes();
    payload.bits = deserial.read_4_bytes();
    payload.nonce = deserial.read_4_bytes();
    uint64_t txn_count = deserial.read_var_uint();
    for (size_t txn_i = 0; txn_i < txn_count; ++txn_i)
    {
        const message::transaction txn = read_transaction(deserial);
        payload.transactions.push_back(txn);
    }
    return payload;
}

bool original_dialect::verify_header(const message::header& header_msg) const
{
    if (header_msg.magic != magic_value)
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
        // Also then add ASSERTS to handlers above.
    }
    else
    {
        // Unknown header
        return false;
    }
    return true;
}

bool original_dialect::checksum_used(const message::header& header_msg) const
{
    return header_msg.command != "version" && header_msg.command != "verack";
}

bool original_dialect::verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const
{
    if (!checksum_used(header_msg))
        return true;
    return header_msg.checksum == generate_sha256_checksum(stream);
}

} // libbitcoin

