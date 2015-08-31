/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "bip38.hpp"

#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::bip38;

BOOST_AUTO_TEST_SUITE(bip38_tests)

BOOST_AUTO_TEST_CASE(bip38__lock_test)
{
    for (const auto& vector: bip38_test_vector)
    {
        if (!vector.ec_multiplied)
        {
#ifdef WITH_ICU
            data_chunk unlocked;
            decode_base16(unlocked, vector.unlocked);

            ec_secret private_key;
            std::copy_n(unlocked.begin(), ec_secret_size,
                private_key.begin());

            const auto& locked = bip38_lock_secret(private_key,
                vector.passphrase, vector.compressed);

            BOOST_REQUIRE_EQUAL(encode_base58(locked), vector.locked);
#endif // WITH_ICU
        }
        else if (vector.random_seed_data.size() > 0)
        {
            data_chunk intermediate;
            decode_base58(intermediate, vector.intermediate);

            data_chunk seedb;
            decode_base16(seedb, vector.random_seed_data);

            data_chunk confirmation_code;
            const auto& locked = bip38_lock_intermediate(intermediate,
                seedb, confirmation_code, vector.compressed);

            BOOST_REQUIRE_EQUAL(encode_base58(locked), vector.locked);
            BOOST_REQUIRE_EQUAL(encode_base58(confirmation_code),
                vector.confirmation_code);

#ifdef WITH_ICU
            // lastly, verify the confirmation code
            std::string address;
            const bool ret = bip38_lock_verify(confirmation_code,
                vector.passphrase, address);
            BOOST_REQUIRE_EQUAL(ret, true);
            BOOST_REQUIRE_EQUAL(address, vector.address);
#endif // WITH_ICU
        }
    }
}

#ifdef WITH_ICU
BOOST_AUTO_TEST_CASE(bip38__unlock_test)
{
    for (const auto& vector: bip38_test_vector)
    {
        data_chunk locked;
        decode_base58(locked, vector.locked);

        const auto& unlocked = bip38_unlock_secret(
            locked, vector.passphrase);

        BOOST_REQUIRE_EQUAL(encode_base16(unlocked), vector.unlocked);
    }
}
#endif // WITH_ICU

BOOST_AUTO_TEST_SUITE_END()
