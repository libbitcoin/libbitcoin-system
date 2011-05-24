#include "transaction.hpp"

#include <boost/detail/endian.hpp>

#include "util/sha256.hpp"

namespace libbitcoin {

void transaction::calculate_hash()
{
    sha256 sha_ctx;
#ifdef BOOST_LITTLE_ENDIAN
    sha_ctx << version;

    sha_ctx.push_var_uint(inputs.size());
    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        sha_ctx << it->hash << it->index;
        sha_ctx.push_var_uint(it->script.length());
        sha_ctx.push_str(it->script);
        sha_ctx << it->sequence;
    }
    
    sha_ctx.push_var_uint(outputs.size());
    for(auto it = outputs.begin(); it != outputs.end(); ++it)
    {
        sha_ctx << it->value;
        sha_ctx.push_var_uint(it->script.length());
        sha_ctx.push_str(it->script);
    }

    sha_ctx << locktime;
    sha_ctx.final(hash);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
}

} // libbitcoin

