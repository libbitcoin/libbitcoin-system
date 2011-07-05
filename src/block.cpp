#include <bitcoin/block.hpp>

#include <boost/detail/endian.hpp>

#include <bitcoin/util/sha256.hpp>

namespace libbitcoin {

void block::calculate_hash()
{
    sha256 sha_ctx;
    //TODO handle errors
#ifdef BOOST_LITTLE_ENDIAN
    sha_ctx.push_4_bytes(version);
    for (size_t i = 0; i < sha256::length; i++)
        sha_ctx.push_byte(prev_hash[i]);
    for (size_t i = 0; i < sha256::length; i++)
        sha_ctx.push_byte(merkle_root[i]);
    sha_ctx.push_4_bytes(timestamp);
    sha_ctx.push_4_bytes(bits);
    for (size_t i = 0; i < 4; i++)
        sha_ctx.push_byte(nonce[i]);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
    sha_ctx.finalize(hash);
}

} // libbitcoin

