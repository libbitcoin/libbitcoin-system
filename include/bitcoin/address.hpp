#ifndef LIBBITCOIN_ADDRESS_H
#define LIBBITCOIN_ADDRESS_H

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {

class payment_address
{
public:
    enum
    {
        pubkey_version = 0,
        script_version = 5
    };

    payment_address();
    payment_address(payment_type address_type, const short_hash& hash);
    payment_address(const std::string& encoded_address);

    bool set(payment_type address_type, const short_hash& hash);
    bool set_raw(byte version_byte, const short_hash& hash);

    const short_hash& hash() const;
    payment_type type() const;

    bool set_encoded(const std::string& encoded_address);
    std::string encoded() const;

    byte version() const;

private:
    bool set_version(byte version_byte);

    payment_type type_;
    short_hash hash_;
};

bool set_public_key_hash(payment_address& address,
    const short_hash& pubkey_hash);
bool set_script_hash(payment_address& address,
    const short_hash& script_hash);

bool set_public_key(payment_address& address, const data_chunk& public_key);
bool set_script(payment_address& address, const script& eval_script);

bool extract(payment_address& address, const script& output_script);

} // namespace libbitcoin

#endif


