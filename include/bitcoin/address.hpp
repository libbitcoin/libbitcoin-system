#ifndef LIBBITCOIN_ADDRESS_H
#define LIBBITCOIN_ADDRESS_H

/*
 * This is a convenience file which is going to be subject
 * to heavy revision as the API is more finely revised.
 * It is strictly temporary.
 */

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {

class payment_address
{
public:
    bool set_public_key(const data_chunk& public_key);
    bool set_public_key_hash(const short_hash& public_key_hash);

    bool set_script(const script& eval_script);
    bool set_script_hash(const short_hash& script_hash);

    bool set_encoded(const std::string& encoded_address);
    std::string encoded();

    payment_type type();
    const short_hash& hash();
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


