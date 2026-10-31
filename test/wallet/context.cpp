/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../test.hpp"

BC_AUTO_TEST_SUITE(context_tests)

using namespace bc::system::wallet;

#define SECRET "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define BTC_WIF_COMPRESSED "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC"
#define LTC_WIF_COMPRESSED "T7LvGRBeb2kKs7hPxSscvrYYNMPGRvbpt1dHcaP6QbSBUMKVEsBo"
#define LTC_WIF_UNCOMPRESSED "6v6RJYJpV6qLEAg4PXnTGNeWJyjCepVHHYQdPZV7fRZtmHGAaGL"

BC_AUTO_TEST_CASE(context__encoded__ltc_mainnet__expected_wif)
{
    const ec_private secret(base16_array(SECRET), ctx::ltc::main.versions());
    BC_REQUIRE_EQUAL(secret.encoded(), LTC_WIF_COMPRESSED);
}

BC_AUTO_TEST_CASE(context__encoded__ltc_mainnet_uncompressed__expected_wif)
{
    const ec_private secret(base16_array(SECRET), ctx::ltc::main.versions(), false);
    BC_REQUIRE_EQUAL(secret.encoded(), LTC_WIF_UNCOMPRESSED);
}

BC_AUTO_TEST_CASE(context__construct__ltc_mainnet_wif__round_trips)
{
    const ec_private secret(LTC_WIF_COMPRESSED, ctx::ltc::main.versions());
    BC_REQUIRE(secret);
    BC_REQUIRE_EQUAL(secret.encoded(), LTC_WIF_COMPRESSED);
}

BC_AUTO_TEST_CASE(context__to_payment_address__ltc_mainnet__ltc_prefix)
{
    const ec_private secret(LTC_WIF_COMPRESSED, ctx::ltc::main.versions());
    BC_REQUIRE_EQUAL(secret.to_payment_address().prefix(), prefix::p2kh::main::ltc);
}

BC_AUTO_TEST_CASE(context__output_script__ltc_p2sh_address__pay_script_hash)
{
    const payment_address address(null_short_hash, ctx::ltc::main.p2sh);
    const auto script = address.output_script(ctx::ltc::main.p2kh, ctx::ltc::main.p2sh);
    BC_REQUIRE(script.output_pattern() == chain::script_pattern::pay_script_hash);
}

BC_AUTO_TEST_CASE(context__output_script__ltc_p2sh_address_btc_context__empty)
{
    const payment_address address(null_short_hash, ctx::ltc::main.p2sh);
    BC_REQUIRE(address.output_script(ctx::btc::main.p2kh, ctx::btc::main.p2sh).ops().empty());
}

BC_AUTO_TEST_CASE(context__construct__ltc_wif_btc_versions__invalid)
{
    BC_REQUIRE(!ec_private(LTC_WIF_COMPRESSED, ctx::btc::main.versions()));
}

BC_AUTO_TEST_CASE(context__construct__btc_wif_ltc_versions__invalid)
{
    BC_REQUIRE(!ec_private(BTC_WIF_COMPRESSED, ctx::ltc::main.versions()));
}

BC_AUTO_TEST_CASE(context__construct__ltc_testnet_wif__round_trips)
{
    const ec_private secret(base16_array(SECRET), ctx::ltc::test.versions());
    BC_REQUIRE_EQUAL(secret.wif_version(), prefix::wif::test::ltc);
    BC_REQUIRE(ec_private(secret.encoded(), ctx::ltc::test.versions()));
}


BC_AUTO_TEST_CASE(context__regtest__btc__testnet_versions_with_regtest_p2w)
{
    BC_REQUIRE_EQUAL(ctx::btc::regtest.p2w, "bcrt");
    BC_REQUIRE_EQUAL(ctx::btc::regtest.versions(), ctx::btc::test.versions());
    BC_REQUIRE_EQUAL(ctx::btc::regtest.hd_prefixes(), ctx::btc::test.hd_prefixes());
    BC_REQUIRE_EQUAL(ctx::btc::regtest.p2sh, ctx::btc::test.p2sh);
}

BC_AUTO_TEST_CASE(context__regtest__ltc__testnet_versions_with_regtest_p2w)
{
    BC_REQUIRE_EQUAL(ctx::ltc::regtest.p2w, "rltc");
    BC_REQUIRE_EQUAL(ctx::ltc::regtest.versions(), ctx::ltc::test.versions());
    BC_REQUIRE_EQUAL(ctx::ltc::regtest.hd_prefixes(), ctx::ltc::test.hd_prefixes());
    BC_REQUIRE_EQUAL(ctx::ltc::regtest.p2sh, ctx::ltc::test.p2sh);
}

BC_AUTO_TEST_CASE(context__sp__btc_networks__bip352)
{
    BC_REQUIRE_EQUAL(ctx::btc::main.sp, "sp");
    BC_REQUIRE_EQUAL(ctx::btc::test.sp, "tsp");
    BC_REQUIRE_EQUAL(ctx::btc::regtest.sp, "tsp");
    BC_REQUIRE(ctx::ltc::main.sp.empty());
}

BC_AUTO_TEST_CASE(context__construct__regtest_wif__round_trips)
{
    const ec_private secret(base16_array(SECRET), ctx::btc::regtest.versions());
    BC_REQUIRE(ec_private(secret.encoded(), ctx::btc::regtest.versions()));
    BC_REQUIRE(!ec_private(secret.encoded(), ctx::btc::main.versions()));
}

BC_AUTO_TEST_SUITE_END()
