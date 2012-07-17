#ifndef LIBBITCOIN_KEY_FORMATS_H
#define LIBBITCOIN_KEY_FORMATS_H

#include <string>

#include <bitcoin/utility/elliptic_curve_key.hpp>

namespace libbitcoin {

/**
 * Convert a secret parameter to the wallet import format.
 * Returns an empty string on error.
 *
 * @code
 *  std::string wif = secret_to_wif(secret);
 *  if (wif.empty())
 *      // Error...
 * @endcode
 */
std::string secret_to_wif(const secret_parameter& secret);

/**
 * Convert wallet import format key to secret parameter.
 * Returns a nulled secret on error.
 *
 * @code
 *  secret_parameter secret = wif_to_secret(
 *      "5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ");
 *  if (secret == null_hash)
 *      // Error...
 * @endcode
 */
secret_parameter wif_to_secret(const std::string& wif);

/**
 * Convert Cascasius minikey to secret parameter.
 * Returns a nulled secret on error.
 *
 * @code
 *  secret_parameter secret =
 *      minikey_to_secret("S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy");
 *  if (secret == null_hash)
 *      // Error...
 * @endcode
 */
secret_parameter minikey_to_secret(const std::string& minikey);

} // libbitcoin

#endif

