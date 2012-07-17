#include <bitcoin/utility/wif.hpp>

#include <bitcoin/format.hpp>
#include <bitcoin/utility/base58.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

std::string private_key_to_wif(private_data unencoded_data)
{
    unencoded_data.insert(unencoded_data.begin(), 0x80);
    uint32_t checksum = generate_sha256_checksum(unencoded_data);
    extend_data(unencoded_data, uncast_type(checksum));
    return encode_base58(unencoded_data);
}

private_data wif_to_private_key(const std::string& wif)
{
    private_data decoded = decode_base58(wif);
    if (decoded.size() < 5)
        return private_data();
    // Check first byte is valid and remove it
    if (decoded[0] != 0x80)
        return private_data();
    // Proceed to verify the checksum
    private_data checksum_bytes(decoded.end() - 4, decoded.end());
    BITCOIN_ASSERT(checksum_bytes.size() == 4);
    decoded.erase(decoded.end() - 4, decoded.end());
    if (cast_chunk<uint32_t>(checksum_bytes) != generate_sha256_checksum(decoded))
        return private_data();
    // Checks passed. Drop the 0x80 start byte.
    decoded.erase(decoded.begin());
    return decoded;
}

} // libbitcoin

