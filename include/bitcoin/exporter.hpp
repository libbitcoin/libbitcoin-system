#ifndef LIBBITCOIN_EXPORTER_H
#define LIBBITCOIN_EXPORTER_H

#include <string>
#include <memory>

#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

class exporter
{
public:
    // To network
    virtual const char* command_name(const message::version&) const = 0;
    virtual const char* command_name(const message::verack&) const = 0;
    virtual const char* command_name(const message::address&) const = 0;
    virtual const char* command_name(const message::get_address&) const = 0;
    virtual const char* command_name(const message::inventory&) const = 0;
    virtual const char* command_name(const message::get_data&) const = 0;
    virtual const char* command_name(const message::get_blocks&) const = 0;
    virtual const char* command_name(const message::block&) const = 0;
    virtual const char* command_name(const message::transaction&) const = 0;
    virtual const char* command_name(const message::ping&) const = 0;

    // Will ignore checksum when checksum == 0
    virtual data_chunk save(const message::header& packet_header) const = 0;

    virtual data_chunk save(const message::version& packet) const = 0;
    virtual data_chunk save(const message::verack& packet) const = 0;
    virtual data_chunk save(const message::address& packet) const = 0;
    virtual data_chunk save(const message::get_address& packet) const = 0;
    virtual data_chunk save(const message::inventory& packet) const = 0;
    virtual data_chunk save(const message::get_data& packet) const = 0;
    virtual data_chunk save(const message::get_blocks& packet) const = 0;
    virtual data_chunk save(const message::block& packet) const = 0;
    virtual data_chunk save(const message::transaction& packet) const = 0;
    virtual data_chunk save(const message::ping& packet) const = 0;

    // From network
    virtual message::header load_header(
        const data_chunk& stream) const = 0;

    virtual message::version load_version(
        const data_chunk& stream) const = 0;

    virtual message::verack load_verack(
        const data_chunk& stream) const = 0;

    virtual message::address load_address(
        const data_chunk& stream) const = 0;

    virtual message::get_address load_get_address(
        const data_chunk& stream) const = 0;

    virtual message::inventory load_inventory(
        const data_chunk& stream) const = 0;

    virtual message::get_data load_get_data(
        const data_chunk& stream) const = 0;

    virtual message::get_blocks load_get_blocks(
        const data_chunk& stream) const = 0;

    virtual message::transaction load_transaction(
        const data_chunk& stream) const = 0;

    virtual message::block load_block(
        const data_chunk& stream) const = 0;

    virtual message::ping load_ping(
        const data_chunk& stream) const = 0;

    // Utilities
    virtual bool verify_header(const message::header& header_msg) const = 0;
};

class satoshi_exporter 
  : public exporter
{
public:
    // Create stream from message
    const char* command_name(const message::version&) const;
    const char* command_name(const message::verack&) const;
    const char* command_name(const message::address&) const;
    const char* command_name(const message::get_address&) const;
    const char* command_name(const message::inventory&) const;
    const char* command_name(const message::get_data&) const;
    const char* command_name(const message::get_blocks&) const;
    const char* command_name(const message::block&) const;
    const char* command_name(const message::transaction&) const;
    const char* command_name(const message::ping&) const;

    // Will ignore checksum when checksum == 0
    data_chunk save(const message::header& packet_header) const;

    data_chunk save(const message::version& packet) const;
    data_chunk save(const message::verack& packet) const;
    data_chunk save(const message::address& packet) const;
    data_chunk save(const message::get_address& packet) const;
    data_chunk save(const message::inventory& packet) const;
    data_chunk save(const message::get_data& packet) const;
    data_chunk save(const message::get_blocks& packet) const;
    data_chunk save(const message::block& packet) const;
    data_chunk save(const message::transaction& packet) const;
    data_chunk save(const message::ping& packet) const;

    // Create header/messages from stream
    message::header load_header(const data_chunk& stream) const;

    message::version load_version(const data_chunk& stream) const;
    message::verack load_verack(const data_chunk& stream) const;
    message::address load_address(const data_chunk& stream) const;
    message::get_address load_get_address(const data_chunk& stream) const;
    message::inventory load_inventory(const data_chunk& stream) const;
    message::get_data load_get_data(const data_chunk& stream) const;
    message::get_blocks load_get_blocks(const data_chunk& stream) const;
    message::transaction load_transaction(const data_chunk& stream) const;
    message::block load_block(const data_chunk& stream) const;
    message::ping load_ping(const data_chunk& stream) const;

    bool verify_header(const message::header& header_msg) const;
};

} // namespace libbitcoin

#endif

