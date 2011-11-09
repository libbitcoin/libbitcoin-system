#ifndef LIBBITCOIN_DIALECT_H
#define LIBBITCOIN_DIALECT_H

#include <string>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/util/serializer.hpp>

namespace libbitcoin {

class dialect
{
public:
    // To network
    virtual data_chunk create_header(const message::version& version,
        const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::verack& verack,
        const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::getaddr& getaddr,
        const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::getdata& getdata,
        const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::getblocks& getblocks,
         const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::block& block,
        const data_chunk& payload) const = 0;
    virtual data_chunk create_header(const message::transaction& tx,
        const data_chunk& payload) const = 0;

    virtual data_chunk to_network(const message::version& version) const = 0;
    virtual data_chunk to_network(const message::verack& verack) const = 0;
    virtual data_chunk to_network(const message::getaddr& getaddr) const = 0;
    virtual data_chunk to_network(const message::getdata& getdata) const = 0;
    virtual data_chunk to_network(
        const message::getblocks& getblocks) const = 0;
    virtual data_chunk to_network(const message::block& block) const = 0;
    virtual data_chunk to_network(const message::transaction& tx) const = 0;

    // Utilities
    virtual message::header header_from_network(
            const data_chunk& stream) const = 0;

    virtual bool verify_header(const message::header& header_msg) const = 0;

    virtual bool checksum_used(const message::header& header_msg) const = 0;

    virtual uint32_t checksum_from_network(const data_chunk& stream) const = 0;

    virtual bool verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const = 0;

    // From network
    virtual message::version version_from_network(
        const data_chunk& stream) const = 0;

    virtual message::addr addr_from_network(
        const data_chunk& stream) const = 0;

    virtual message::inv inv_from_network(
        const data_chunk& stream) const = 0;

    virtual message::transaction transaction_from_network(
        const data_chunk& stream) const = 0;

    virtual message::block block_from_network(
        const data_chunk& stream) const = 0;
};

class original_dialect 
 : public dialect
{
public:
    // Create stream from message
    data_chunk create_header(const message::version& version,
        const data_chunk& payload) const;
    data_chunk create_header(const message::verack& verack,
        const data_chunk& payload) const;
    data_chunk create_header(const message::getaddr& getaddr,
        const data_chunk& payload) const;
    data_chunk create_header(const message::getdata& getdata,
        const data_chunk& payload) const;
    data_chunk create_header(const message::getblocks& getblocks,
         const data_chunk& payload) const;
    data_chunk create_header(const message::block& block,
        const data_chunk& payload) const;
    data_chunk create_header(const message::transaction& tx,
        const data_chunk& payload) const;

    data_chunk to_network(const message::version& version) const;
    data_chunk to_network(const message::verack& verack) const;
    data_chunk to_network(const message::getaddr& getaddr) const;
    data_chunk to_network(const message::getdata& getdata) const;
    data_chunk to_network(const message::getblocks& getblocks) const;
    data_chunk to_network(const message::block& block) const;
    data_chunk to_network(const message::transaction& tx) const;

    // Create header/messages from stream
    message::header header_from_network(const data_chunk& stream) const;
    bool verify_header(const message::header& header_msg) const;

    bool checksum_used(const message::header& header_msg) const;
    virtual uint32_t checksum_from_network(const data_chunk& stream) const;

    bool verify_checksum(const message::header& header_msg,
            const data_chunk& stream) const;

    message::version version_from_network(const data_chunk& stream) const;

    message::addr addr_from_network(const data_chunk& stream) const;

    message::inv inv_from_network(const data_chunk& stream) const;

    message::transaction transaction_from_network(
        const data_chunk& stream) const;

    message::block block_from_network(const data_chunk& stream) const;
};

} // libbitcoin

#endif
