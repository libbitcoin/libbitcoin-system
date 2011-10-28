#include <bitcoin/address.hpp>

#include <bitcoin/util/base58.hpp>
#include <bitcoin/util/ripemd.hpp>
#include <bitcoin/util/sha256.hpp>

namespace libbitcoin {

std::string pubkey_to_address(const data_chunk& public_key)
{
    data_chunk unhashed_address;
    unhashed_address.push_back(0);
    extend_data(unhashed_address, generate_ripemd_hash(public_key));
    uint32_t checksum = generate_sha256_checksum(unhashed_address);
    extend_data(unhashed_address, uncast_type(checksum));
    return encode_base58(unhashed_address);
}

} // libbitcoin

