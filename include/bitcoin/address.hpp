#ifndef LIBBITCOIN_ADDRESS_HPP
#define LIBBITCOIN_ADDRESS_HPP

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {

/**
 * A class for handling Bitcoin addresses. Supports encoding and decoding
 * Bitcoin string addresses.
 *
 * To validate a Bitcoin address we can try to set a string address.
 *
 * @code
 *   payment_address payaddr;
 *   if (!payaddr.set_encoded("155GwFbFET2HCT6r6jHAHUoxc897sSdjaq"))
 *       // Address is invalid
 * @endcode
 *
 * To check whether a payment_address has successfully been set, the
 * hash value can be compared to null_short_hash (defined in constants.hpp).
 *
 * @code
 *   if (payaddr.hash() == null_short_hash)
 *       // This payment_address is empty.
 * @endcode
 */
class payment_address
{
public:
    enum
    {
        pubkey_version = 0,
        script_version = 5
    };

    payment_address();
    payment_address(payment_type type, const short_hash& hash);
    payment_address(const std::string& encoded_address);

    bool set(payment_type type, const short_hash& hash);
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

/**
 * Extract a Bitcoin address from an output script.
 * Returns false on failure.
 */
bool extract(payment_address& address, const script& output_script);
/**
 * Extract a Bitcoin address from an input script.
 * Returns false on failure.
 */
bool extract_input_address(
    payment_address& address, const script& input_script);

} // namespace libbitcoin

#endif

