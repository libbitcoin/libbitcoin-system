#ifndef LIBBITCOIN_WIF_H
#define LIBBITCOIN_WIF_H

#include <string>

#include <bitcoin/utility/elliptic_curve_key.hpp>

namespace libbitcoin {

/**
 * Convert a raw private key to the wallet import format.
 * Returns an empty string on error.
 *
 * @code
 *  std::string wif_key = private_key_to_wif(raw_privkey);
 *  if (wif_key.empty())
 *      // Error...
 * @endcode
 */
std::string private_key_to_wif(private_data unencoded_data);

/**
 * Convert a key from wallet import format key to raw.
 * Returns an empty private key on error.
 *
 * @code
 *  private_data raw_key = wif_to_private_key(
 *      "5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ");
 *  if (key.empty())
 *      // Error...
 * @endcode
 */
private_data wif_to_private_key(const std::string& wif);

} // libbitcoin

#endif

