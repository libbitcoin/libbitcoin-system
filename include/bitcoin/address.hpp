#ifndef LIBBITCOIN_ADDRESS_H
#define LIBBITCOIN_ADDRESS_H

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {

class payment_address
{
public:
    payment_address();

    bool set_public_key(const data_chunk& public_key);
    bool set_public_key_hash(const short_hash& public_key_hash);

    bool set_script(const script& eval_script);
    bool set_script_hash(const short_hash& script_hash);

    bool set_encoded(const std::string& encoded_address);
    std::string encoded() const;

    payment_type type() const;
    const short_hash& hash() const;
private:
    enum
    {
        pubkey_version = 0,
        script_version = 5
    };

    payment_type type_;
    short_hash hash_;
};

} // namespace libbitcoin

#endif


