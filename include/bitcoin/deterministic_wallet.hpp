#ifndef LIBBITCOIN_DETERMINISTIC_WALLET_H
#define LIBBITCOIN_DETERMINISTIC_WALLET_H

/**
 * Electrum compatible deterministic wallet
 */

#include <bitcoin/types.hpp>
#include <bitcoin/utility/elliptic_curve_key.hpp>

namespace libbitcoin {

class deterministic_wallet
{
public:
    static constexpr size_t seed_size = 32;

    void new_seed();
    bool set_seed(const std::string& seed);
    const std::string& seed() const;

    bool set_master_public_key(const data_chunk& mpk);
    const data_chunk& master_public_key() const;

    /**
     * Generate the n'th public key.
     *
     * @code
     * payment_addr addr;
     * set_public_key(addr, wallet.generate_public_key(2));
     * btc_address = addr.encoded();
     * @endcode
     */
    data_chunk generate_public_key(size_t n, bool for_change=false);

    /**
     * Generate the n'th secret.
     *
     * The secret can be used to get the corresponding private key,
     * and also the public key. If just the public key is desired then
     * use generate_public_key() instead.
     *
     * @code
     * elliptic_curve_key privkey;
     * privkey.set_secret(generate_secret(2));
     * @endcode
     */
    secret_parameter generate_secret(size_t n, bool for_change=false);

private:
    hash_digest get_sequence(size_t n, bool for_change);

    std::string seed_;
    secret_parameter stretched_seed_;
    data_chunk master_public_key_;
};

} // namespace libbitcoin

#endif

