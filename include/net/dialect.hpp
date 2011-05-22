#ifndef LIBBITCOIN_NET_DIALECT_H
#define LIBBITCOIN_NET_DIALECT_H

#include <string>

#include "net/serializer.hpp"
#include "net/messages.hpp"

namespace libbitcoin {
namespace net {

class dialect
{
public:
    virtual const serializer::stream to_network(
            message::version version) const = 0;
    virtual const message::header header_from_network(
            const serializer::stream& stream) const = 0;
};

class original_dialect : public dialect
{
public:
    // Create stream from message
    const serializer::stream to_network(message::version version) const;

    // Create header/messages from stream
    const message::header header_from_network(
            const serializer::stream& stream) const;
};

} // net
} // libbitcoin

#endif
