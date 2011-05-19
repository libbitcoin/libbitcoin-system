#ifndef LIBBITCOIN_NET_DIALECT_H
#define LIBBITCOIN_NET_DIALECT_H

#include <string>

#include "net/serializer.h"
#include "net/messages.h"

namespace libbitcoin {
namespace net {

class dialect
{
public:
    virtual const serializer::stream translate(message::version version) const = 0;
};

} // net
} // libbitcoin

#endif
