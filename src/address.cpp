#include <bitcoin/address.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/utility/base58.hpp>
#include <bitcoin/utility/ripemd.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

payment_address::payment_address()
  : version_(pubkey_version), hash_(null_short_hash)
{
}
payment_address::payment_address(uint8_t version, const short_hash& hash)
  : payment_address()
{
    set(version, hash);
}
payment_address::payment_address(const std::string& encoded_address)
  : payment_address()
{
    set_encoded(encoded_address);
}

void payment_address::set(uint8_t version, const short_hash& hash)
{
    version_ = version;
    hash_ = hash;
}

uint8_t payment_address::version() const
{
    return version_;
}
const short_hash& payment_address::hash() const
{
    return hash_;
}

bool payment_address::set_encoded(const std::string& encoded_address)
{
    const data_chunk decoded_address = decode_base58(encoded_address);
    // version + 20 bytes short hash + 4 bytes checksum
    if (decoded_address.size() != 25)
        return false;
    version_ = decoded_address[0];
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

std::string payment_address::encoded() const
{
    data_chunk unencoded_address;
    // Type, Hash, Checksum doth make thy address
    unencoded_address.push_back(version_);
    extend_data(unencoded_address, hash_);
    uint32_t checksum = generate_sha256_checksum(unencoded_address);
    extend_data(unencoded_address, uncast_type(checksum));
    BITCOIN_ASSERT(unencoded_address.size() == 25);
    return encode_base58(unencoded_address);
}

void set_public_key_hash(payment_address& address,
    const short_hash& pubkey_hash)
{
    address.set(payment_address::pubkey_version, pubkey_hash);
}

void set_script_hash(payment_address& address,
    const short_hash& script_hash)
{
    address.set(payment_address::script_version, script_hash);
}

void set_public_key(payment_address& address, const data_chunk& public_key)
{
    address.set(payment_address::pubkey_version,
        generate_ripemd_hash(public_key));
}

void set_script(payment_address& address, const script& eval_script)
{
    address.set(payment_address::script_version,
        generate_ripemd_hash(save_script(eval_script)));
}

bool extract(payment_address& address, const script& scr)
{
    // Cast a data_chunk to a short_hash and set the address
    auto set_hash_data =
        [&address](uint8_t version, const data_chunk& raw_hash)
        {
            short_hash hash_data;
            BITCOIN_ASSERT(raw_hash.size() == hash_data.size());
            std::copy(raw_hash.begin(), raw_hash.end(), hash_data.begin());
            address.set(version, hash_data);
        };
    const operation_stack& ops = scr.operations();
    payment_type pay_type = scr.type();
    switch (pay_type)
    {
        case payment_type::pubkey:
            BITCOIN_ASSERT(ops.size() == 2);
            set_public_key(address, ops[0].data);
            return true;

        case payment_type::pubkey_hash:
            BITCOIN_ASSERT(ops.size() == 5);
            set_hash_data(payment_address::pubkey_version, ops[2].data);
            return true;

        case payment_type::script_hash:
            BITCOIN_ASSERT(ops.size() == 3);
            set_hash_data(payment_address::script_version, ops[1].data);
            return true;

        case payment_type::multisig:
            // Unimplemented...
            return false;

        case payment_type::pubkey_hash_sig:
            BITCOIN_ASSERT(ops.size() == 2);
            set_public_key(address, ops[1].data);
            return true;

        case payment_type::script_code_sig:
            // Should have at least 1 sig and the script code.
            BITCOIN_ASSERT(ops.size() > 1);
            set_script_hash(address,
                generate_ripemd_hash(ops.back().data));
            return true;

        default:
            return false;
    }
    // Should never happen!
    return false;
}

bool input_has_pubkey(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code == opcode::special &&
        ops[1].code == opcode::special;
}
bool extract_input_address(
    payment_address& address, const script& input_script)
{
    const operation_stack& ops = input_script.operations();
    if (!input_has_pubkey(ops))
        return false;
    BITCOIN_ASSERT(ops.size() == 2);
    const data_chunk& pubkey = ops[1].data;
    set_public_key(address, pubkey);
    return true;
}

bool operator==(const payment_address& lhs, const payment_address& rhs)
{
    return lhs.hash() == rhs.hash() && lhs.version() == rhs.version();
}

} // namespace libbitcoin

