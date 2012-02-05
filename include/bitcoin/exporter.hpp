#ifndef LIBBITCOIN_EXPORTER_H
#define LIBBITCOIN_EXPORTER_H

#include <string>
#include <memory>

#include <bitcoin/constants.hpp>
#include <bitcoin/data_helpers.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

class exporter
{
public:
    // To network
    // version      --
    // verack       --
    // addr         sub
    // inv          sub
    // getdata      send
    // getblocks    send
    // tx           send
    // block        sub
    // getaddr      send
    // ping

    virtual const char* command_name(const message::version&) const = 0;
    virtual const char* command_name(const message::verack&) const = 0;
    virtual const char* command_name(const message::get_address&) const = 0;
    virtual const char* command_name(const message::get_data&) const = 0;
    virtual const char* command_name(const message::get_blocks&) const = 0;
    virtual const char* command_name(const message::block&) const = 0;
    virtual const char* command_name(const message::transaction&) const = 0;

    // Will ignore checksum when checksum == 0
    virtual data_chunk save(const message::header& packet_header) const = 0;

    virtual data_chunk save(const message::version& version) const = 0;
    virtual data_chunk save(const message::verack& verack) const = 0;
    virtual data_chunk save(const message::get_address& getaddr) const = 0;
    virtual data_chunk save(const message::get_data& getdata) const = 0;
    virtual data_chunk save(const message::get_blocks& getblocks) const = 0;
    virtual data_chunk save(const message::block& block) const = 0;
    virtual data_chunk save(const message::transaction& tx) const = 0;

    // Utilities
    virtual message::header load_header(const data_chunk& stream) const = 0;
    virtual bool verify_header(const message::header& header_msg) const = 0;

    virtual bool is_checksum_used(
        const message::header& header_msg) const = 0;
    virtual uint32_t load_checksum(const data_chunk& stream) const = 0;
    virtual bool verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const = 0;

    // From network
    virtual message::version load_version(
        const data_chunk& stream) const = 0;

    virtual message::verack load_verack(
        const data_chunk& stream) const = 0;

    virtual message::address load_address(
        const data_chunk& stream) const = 0;

    virtual message::inventory load_inventory(
        const data_chunk& stream) const = 0;

    virtual message::transaction load_transaction(
        const data_chunk& stream) const = 0;

    virtual message::block load_block(
        const data_chunk& stream) const = 0;
};

class satoshi_exporter 
  : public exporter
{
public:
    // Create stream from message
    const char* command_name(const message::version&) const;
    const char* command_name(const message::verack&) const;
    const char* command_name(const message::get_address&) const;
    const char* command_name(const message::get_data&) const;
    const char* command_name(const message::get_blocks&) const;
    const char* command_name(const message::block&) const;
    const char* command_name(const message::transaction&) const;

    // Will ignore checksum when checksum == 0
    data_chunk save(const message::header& packet_header) const;

    data_chunk save(const message::version& version) const;
    data_chunk save(const message::verack& verack) const;
    data_chunk save(const message::get_address& getaddr) const;
    data_chunk save(const message::get_data& getdata) const;
    data_chunk save(const message::get_blocks& getblocks) const;
    data_chunk save(const message::block& block) const;
    data_chunk save(const message::transaction& tx) const;

    // Create header/messages from stream
    message::header load_header(const data_chunk& stream) const;
    bool verify_header(const message::header& header_msg) const;

    bool is_checksum_used(const message::header& header_msg) const;
    uint32_t load_checksum(const data_chunk& stream) const;
    bool verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const;

    message::version load_version(const data_chunk& stream) const;
    message::verack load_verack(const data_chunk& stream) const;
    message::address load_address(const data_chunk& stream) const;
    message::inventory load_inventory(const data_chunk& stream) const;
    message::transaction load_transaction(
        const data_chunk& stream) const;
    message::block load_block(const data_chunk& stream) const;
};

template <typename Message>
data_chunk create_raw_message(exporter_ptr saver, const Message& packet)
{
    data_chunk payload = saver->save(packet);
    // Make the header packet and serialise it
    message::header packet_header;
    packet_header.magic = magic_value;
    packet_header.command = saver->command_name(packet);
    packet_header.payload_length = payload.size();
    if (saver->is_checksum_used(packet_header))
        packet_header.checksum = generate_sha256_checksum(payload);
    data_chunk raw_header = saver->save(packet_header);
    // Construct completed packet with header + payload
    data_chunk whole_message = raw_header;
    extend_data(whole_message, payload);
    // Probably not the right place for this
    // Networking output in an exporter
    log_info(log_domain::network) << "s: " << packet_header.command
        << " (" << payload.size() << " bytes)";
    return whole_message;
}

} // libbitcoin

#endif

