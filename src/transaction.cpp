#include "transaction.hpp"

#include <boost/detail/endian.hpp>

#include "util/sha256.hpp"

namespace libbitcoin {

void transaction::calculate_hash()
{
    sha256 sha_ctx;
#ifdef BOOST_LITTLE_ENDIAN
    sha_ctx.update(version);

    sha_ctx.var_uint(inputs.size());
    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        sha_ctx.update(it->hash);
        sha_ctx.update(it->index);
        sha_ctx.var_uint(it->script.length());
        sha_ctx.str(it->script);
        sha_ctx.update(it->sequence);
    }
    
    sha_ctx.var_uint(outputs.size());
    for(auto it = outputs.begin(); it != outputs.end(); ++it)
    {
        sha_ctx.update(it->value);
        sha_ctx.var_uint(it->script.length());
        sha_ctx.str(it->script);
    }

    sha_ctx.update(locktime);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
    sha_ctx.final(hash);
}

} // libbitcoin

