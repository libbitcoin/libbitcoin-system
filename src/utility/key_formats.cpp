#include <bitcoin/utility/key_formats.hpp>

#include <bitcoin/format.hpp>
#include <bitcoin/utility/base58.hpp>
#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/address.hpp>

namespace libbitcoin {

std::string secret_to_wif(const secret_parameter& secret)
{
    private_data unencoded_data(secret.begin(), secret.end());
    unencoded_data.insert(unencoded_data.begin(), payment_address::wif_version);
    uint32_t checksum = generate_sha256_checksum(unencoded_data);
    extend_data(unencoded_data, uncast_type(checksum));
    return encode_base58(unencoded_data);
}

secret_parameter wif_to_secret(const std::string& wif)
{
    private_data decoded = decode_base58(wif);
    // 1 marker, 32 byte secret, 4 checksum bytes
    if (decoded.size() != 1 + sha256_digest_size + 4)
        return secret_parameter();
    // Check first byte is valid and remove it
    if (decoded[0] != payment_address::wif_version)
        return secret_parameter();
    // Proceed to verify the checksum
    private_data checksum_bytes(decoded.end() - 4, decoded.end());
    BITCOIN_ASSERT(checksum_bytes.size() == 4);
    decoded.erase(decoded.end() - 4, decoded.end());
    if (cast_chunk<uint32_t>(checksum_bytes) !=
            generate_sha256_checksum(decoded))
        return secret_parameter();
    // Checks passed. Drop the 0x80 start byte.
    decoded.erase(decoded.begin());
    secret_parameter secret;
    BITCOIN_ASSERT(secret.size() == decoded.size());
    std::copy(decoded.begin(), decoded.end(), secret.begin());
    return secret;
}

hash_digest single_sha256(const std::string& mini)
{
    SHA256_CTX ctx;
    hash_digest digest;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, mini.c_str(), mini.size());
    SHA256_Final(digest.data(), &ctx);
    return digest;
}

bool check_minikey(const std::string& minikey)
{
    // Legacy minikeys are 22 chars long
    if (minikey.size() != 22 && minikey.size() != 30)
        return false;
    return single_sha256(minikey + "?")[0] == 0x00;
}

secret_parameter minikey_to_secret(const std::string& minikey)
{
    if (!check_minikey(minikey))
        return secret_parameter();
    return single_sha256(minikey);
}

} // libbitcoin

