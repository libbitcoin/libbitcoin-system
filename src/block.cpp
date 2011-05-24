#include "block.hpp"

#include <boost/detail/endian.hpp>

#include "util/sha256.hpp"

namespace libbitcoin {

void block::calculate_hash()
{
    sha256 sha_ctx;
    //TODO handle errors
#ifdef BOOST_LITTLE_ENDIAN
    sha_ctx << version << prev_hash << merkle_root << timestamp << bits
            << nonce;
    sha_ctx.final(hash);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
}

} // libbitcoin

