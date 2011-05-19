#ifndef LIBBITCOIN_NET_DIALECTS_ORIGINAL_H
#define LIBBITCOIN_NET_DIALECTS_ORIGINAL_H

#include "net/dialect.h"

namespace libbitcoin {
namespace net {

class original_dialect : public dialect
{
public:
    struct var_int
    {
        uint64_t get();
        int len;
        uint16_t nums[4];
    };
    
    // Equivalent is called untranslate
    const serializer::stream translate(message::version version) const;
};

} // net
} // libbitcoin

#endif
