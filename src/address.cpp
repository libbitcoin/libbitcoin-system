#include <bitcoin/address.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/utility/base58.hpp>
#include <bitcoin/utility/ripemd.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

bool payment_address::set_public_key(const data_chunk& public_key)
{
    set_public_key_hash(generate_ripemd_hash(public_key));
    return true;
}

bool payment_address::set_public_key_hash(const short_hash& public_key_hash)
{
    type_ = payment_type::pubkey_hash;
    hash_ = public_key_hash;
    return true;
}

bool payment_address::set_script(const script& eval_script)
{
    set_script_hash(generate_ripemd_hash(save_script(eval_script)));
    return true;
}

bool payment_address::set_script_hash(const short_hash& script_hash)
{
    type_ = payment_type::script_hash;
    hash_ = script_hash;
    return true;
}

bool payment_address::set_encoded(const std::string& encoded_address)
{
    const data_chunk decoded_address = decode_base58(encoded_address);
    // version + 20 bytes short hash + 4 bytes checksum
    if (decoded_address.size() != 25)
        return false;
    const uint8_t version = decoded_address[0];
    if (version == pubkey_version)
        type_ = payment_type::pubkey_hash;
    else if (version == script_version)
        type_ = payment_type::script_hash;
    else
        return false;
    const data_chunk checksum_bytes(
        decoded_address.end() - 4, decoded_address.end());
    // version + short hash
    const data_chunk main_body(
        decoded_address.begin(), decoded_address.end() - 4);
    // verify checksum bytes
    if (generate_sha256_checksum(main_body) != 
            cast_chunk<uint32_t>(checksum_bytes))
        return false;
    std::copy(main_body.begin() + 1, main_body.end(), hash_.begin());
    return true;
}

std::string payment_address::encoded()
{
    data_chunk unencoded_address;
    BITCOIN_ASSERT(
        type_ == payment_type::pubkey_hash ||
        type_ == payment_type::script_hash);
    // Type, Hash, Checksum doth make thy address
    if (type_ == payment_type::pubkey_hash)
        unencoded_address.push_back(pubkey_version);
    else if (type_ == payment_type::script_hash)
        unencoded_address.push_back(script_version);
    //else
    //    ... should never happen!
    extend_data(unencoded_address, hash_);
    uint32_t checksum = generate_sha256_checksum(unencoded_address);
    extend_data(unencoded_address, uncast_type(checksum));
    BITCOIN_ASSERT(unencoded_address.size() == 25);
    return encode_base58(unencoded_address);
}

payment_type payment_address::type()
{
    return type_;
}

const short_hash& payment_address::hash()
{
    return hash_;
}

} // namespace libbitcoin

