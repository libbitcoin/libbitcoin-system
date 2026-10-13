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
#include "../../../test.hpp"

BC_PUSH_WARNING(NO_USE_OF_SPAN)

BOOST_AUTO_TEST_SUITE(secp256k1_batch_scan_tests)

using namespace system::scan;
using tx_links = std::vector<batch::tx_link_t>;
using keys_t = std::vector<ec_xonly>;

constexpr ec_secret scan_secret = base16_array("0f694e068028a717f8af6b9411f9a133dd3565258714cc226594b34db90c1f2c");
constexpr ec_secret spend_secret = base16_array("9d6ad855ce3417ef84e836892e5a56392bfba05fa5d97ccea30e266f540e08b3");
constexpr ec_compressed unlabeled_point = base16_array("024ac253c216532e961988e2a8ce266a447c894c781e52ef6cee902361db960004");
constexpr ec_compressed labeled_point = base16_array("0314bec14463d6c0181083d607fecfba67bb83f95915f6f247975ec566d5642ee8");
constexpr ec_xonly unlabeled_key = base16_array("3e9fce73d4e77a4809908e3c3a2e54ee147b9312dc5044a193d1fc85de46e3c1");
constexpr ec_xonly labeled_key = base16_array("67626aebb3c4307cf0f6c39ca23247598fabf675ab783292eb2f81ae75ad1f8c");

struct transactions
{
    void add(batch::tx_link_t link, const ec_compressed& point,
        const keys_t& keys) NOEXCEPT
    {
        constexpr auto size = array_count<batch::prefix>;
        constexpr auto link_size = array_count<batch::prefix_link>;
        constexpr auto count_size = array_count<batch::prefix_count>;
        correlates.push_back(to_little_endian(link));
        points.push_back(point);
        outputs.push_back(
        {
            to_little_endian_size<link_size>(prefixes.size()),
            to_little_endian_size<count_size>(keys.size())
        });

        for (const auto& key: keys)
            prefixes.push_back(array_cast<uint8_t, size>(key));
    }

    void add(batch::tx_link_t first, batch::tx_link_t count,
        const ec_compressed& point, const keys_t& keys) NOEXCEPT
    {
        for (auto link = first; link < first + count; ++link)
            add(link, point, keys);
    }

    batch to_batch() const NOEXCEPT
    {
        return { correlates, points, outputs, prefixes };
    }

    std::vector<batch::tx_link> correlates{};
    std::vector<ec_compressed> points{};
    std::vector<batch::outputs_t> outputs{};
    std::vector<batch::prefix> prefixes{};
};

static batch::receiver get_keys(const std_vector<uint32_t>& labels) NOEXCEPT
{
    ec_compressed spend{};
    if (!secret_to_public(spend, spend_secret))
        return {};

    return wallet::silent_payment{ scan_secret, spend, labels }.keys();
}

static tx_links scan_links(const transactions& txs,
    const batch::receiver& keys, bool turbo) NOEXCEPT
{
    std::mutex mutex{};
    tx_links out{};
    const auto handler = [&](const code&, auto link, const auto&) NOEXCEPT
    {
        std::unique_lock lock{ mutex };
        out.push_back(link);
    };

    const stopper cancel{};
    constexpr auto minimum = batch::gpu_txs;
    batch::scan(cancel, txs.to_batch(), keys, minimum, handler, turbo);
    sort(out);
    return out;
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__empty__empty)
{
    BOOST_REQUIRE(scan_links({}, get_keys({}), false).empty());
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__unlabeled__expected)
{
    transactions txs{};
    txs.add(1, unlabeled_point, { labeled_key, unlabeled_key });
    txs.add(2, unlabeled_point, { labeled_key });
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), false), tx_links{ 1 });
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__labeled__expected)
{
    transactions txs{};
    txs.add(1, unlabeled_point, { labeled_key });
    txs.add(2, labeled_point, { labeled_key });
    txs.add(3, labeled_point, { unlabeled_key });
    BOOST_REQUIRE(scan_links(txs, get_keys({}), false).empty());
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({ 2, 3, 1001337 }), false), tx_links{ 2 });
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__no_outputs__skipped)
{
    transactions txs{};
    txs.add(1, unlabeled_point, {});
    txs.add(2, unlabeled_point, { unlabeled_key });
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), false), tx_links{ 2 });
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__invalid_point__skipped)
{
    transactions txs{};
    txs.add(1, null_ec_compressed, { unlabeled_key });
    txs.add(2, unlabeled_point, { unlabeled_key });
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), false), tx_links{ 2 });
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__chunk_straddle__once)
{
    transactions txs{};
    txs.add(100, 1023, unlabeled_point, { labeled_key });
    txs.add(7, unlabeled_point, { labeled_key, unlabeled_key });
    txs.add(8, unlabeled_point, { unlabeled_key });
    BOOST_REQUIRE_EQUAL(txs.points.size(), 1025u);
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), false), (tx_links{ 7, 8 }));
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), true), (tx_links{ 7, 8 }));
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_scan__scan__gpu_transactions__expected)
{
    transactions txs{};
    txs.add(100, 70000, unlabeled_point, { labeled_key });
    txs.add(7, unlabeled_point, { labeled_key, unlabeled_key });
    txs.add(8, labeled_point, { labeled_key });
    txs.add(9, labeled_point, { unlabeled_key });
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({}), true), (tx_links{ 7 }));
    BOOST_REQUIRE_EQUAL(scan_links(txs, get_keys({ 2, 3, 1001337 }), true), (tx_links{ 7, 8 }));
}

BOOST_AUTO_TEST_SUITE_END()

BC_POP_WARNING()
