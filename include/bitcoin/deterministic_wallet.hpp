/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DETERMINISTIC_WALLET_HPP
#define LIBBITCOIN_DETERMINISTIC_WALLET_HPP

#include <bitcoin/types.hpp>
#include <bitcoin/utility/elliptic_curve_key.hpp>

namespace libbitcoin {

/**
 * Electrum compatible deterministic wallet.
 */
class deterministic_wallet
{
public:
    static constexpr size_t seed_size = 32;

    /**
     * Generate a new seed.
     *
     * @code
     * deterministic_wallet wallet;
     * wallet.new_seed();
     * log_info() << "new seed: " << wallet.seed();
     * @endcode
     */
    void new_seed();

    /**
     * Restore wallet from seed.
     *
     * @code
     * if (!wallet.set_seed("..."))
     *   // Error...
     * @endcode
     */
    bool set_seed(std::string seed);

    /**
     * Return the wallet seed. The seed should always be
     * deterministic_wallet::seed_size in length.
     *
     * @return  Wallet seed. Empty string if not existant.
     */
    const std::string& seed() const;

    bool set_master_public_key(const data_chunk& mpk);
    const data_chunk& master_public_key() const;

    /**
     * Generate the n'th public key. A seed or master_public_key must be set.
     *
     * @code
     * payment_address addr;
     * set_public_key(addr, wallet.generate_public_key(2));
     * btc_address = addr.encoded();
     * @endcode
     */
    data_chunk generate_public_key(size_t n, bool for_change=false) const;

    /**
     * Generate the n'th secret. A seed must be set.
     *
     * The secret can be used to get the corresponding private key,
     * and also the public key. If just the public key is desired then
     * use generate_public_key() instead.
     *
     * @code
     * elliptic_curve_key privkey;
     * privkey.set_secret(wallet.generate_secret(2));
     * @endcode
     */
    secret_parameter generate_secret(size_t n, bool for_change=false) const;

private:
    hash_digest get_sequence(size_t n, bool for_change) const;

    std::string seed_;
    secret_parameter stretched_seed_;
    data_chunk master_public_key_;
};

} // namespace libbitcoin

#endif

