#include "block.hpp"

#include <boost/detail/endian.hpp>

#include "util/sha256.hpp"

namespace libbitcoin {

void block::calculate_hash()
{
    sha256 sha_ctx;
    //TODO handle errors
#ifdef BOOST_LITTLE_ENDIAN
    sha_ctx.update(version);
    sha_ctx.update(prev_hash);
    sha_ctx.update(merkle_root);
    sha_ctx.update(timestamp);
    sha_ctx.update(bits);
    sha_ctx.update(nonce);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
    sha_ctx.final(hash);
}

} // libbitcoin

