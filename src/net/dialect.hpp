#ifndef LIBBITCOIN_NET_DIALECT_H
#define LIBBITCOIN_NET_DIALECT_H

#include <string>
#include <memory>

#include <bitcoin/net/messages.hpp>

#include "serializer.hpp"

namespace libbitcoin {
namespace net {

class dialect
{
public:
    virtual data_chunk to_network(
            message::version version) const = 0;

    virtual data_chunk to_network(
            message::verack verack) const = 0;

    virtual data_chunk to_network(
            message::getaddr getaddr) const = 0;

    virtual message::header header_from_network(
            const data_chunk& stream) const = 0;

    virtual uint32_t checksum_from_network(
            const data_chunk& stream) const = 0;

    virtual message::version version_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const = 0;

    virtual message::addr addr_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const = 0;

    virtual message::inv inv_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const = 0;

    virtual bool verify_header(net::message::header header_msg) const = 0;
};

class original_dialect : public dialect
{
public:
    // Create stream from message
    data_chunk to_network(message::version version) const;
    data_chunk to_network(message::verack verack) const;
    data_chunk to_network(message::getaddr getaddr) const;

    // Create header/messages from stream
    message::header header_from_network(const data_chunk& stream) const;

    virtual uint32_t checksum_from_network(const data_chunk& stream) const;

    message::version version_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const;

    message::inv inv_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const;

    message::addr addr_from_network(
            const message::header header_msg,
            const data_chunk& stream, bool& ec) const;

    bool verify_header(net::message::header header_msg) const;
};

} // net
} // libbitcoin

#endif
