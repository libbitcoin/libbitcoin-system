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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(secp256k1_bip341_tests)

// bitcoin/bips bip-0341/wallet-test-vectors.json (scriptPubKey)

struct tweak_vector
{
    ec_xonly internal;
    hash_digest tweak;
    ec_xonly tweaked;
};

const std::vector<tweak_vector> tweak_vectors
{
    { base16_array("d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961d"), base16_array("b86e7be8f39bab32a6f2c0443abbc210f0edac0e2c53d501b36b64437d9c6c70"), base16_array("53a1f6e454df1aa2776a2814a721372d6258050de330b3c6d10ee8f4e0dda343") },
    { base16_array("187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"), base16_array("cbd8679ba636c1110ea247542cfbd964131a6be84f873f7f3b62a777528ed001"), base16_array("147c9c57132f6e7ecddba9800bb0c4449251c92a1e60371ee77557b6620f3ea3") },
    { base16_array("93478e9488f956df2396be2ce6c5cced75f900dfa18e7dabd2428aae78451820"), base16_array("6af9e28dbf9d6aaf027696e2598a5b3d056f5fd2355a7fd5a37a0e5008132d30"), base16_array("e4d810fd50586274face62b8a807eb9719cef49c04177cc6b76a9a4251d5450e") },
    { base16_array("ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592"), base16_array("9e0517edc8259bb3359255400b23ca9507f2a91cd1e4250ba068b4eafceba4a9"), base16_array("712447206d7a5238acc7ff53fbe94a3b64539ad291c7cdbc490b7577e4b17df5") },
    { base16_array("f9f400803e683727b14f463836e1e78e1c64417638aa066919291a225f0e8dd8"), base16_array("639f0281b7ac49e742cd25b7f188657626da1ad169209078e2761cefd91fd65e"), base16_array("77e30a5522dd9f894c3f8b8bd4c4b2cf82ca7da8a3ea6a239655c39c050ab220") },
    { base16_array("e0dfe2300b0dd746a3f8674dfd4525623639042569d829c7f0eed9602d263e6f"), base16_array("b57bfa183d28eeb6ad688ddaabb265b4a41fbf68e5fed2c72c74de70d5a786f4"), base16_array("91b64d5324723a985170e4dc5a0f84c041804f2cd12660fa5dec09fc21783605") },
    { base16_array("55adf4e8967fbd2e29f20ac896e60c3b0f1d5b0efa9d34941b5958c7b0a0312d"), base16_array("6579138e7976dc13b6a92f7bfd5a2fc7684f5ea42419d43368301470f3b74ed9"), base16_array("75169f4001aa68f15bbed28b218df1d0a62cbbcf1188c6665110c293c907b831") },
};

BOOST_AUTO_TEST_CASE(secp256k1_bip341__ec_add__tweak_vectors__expected_key)
{
    for (size_t index{}; index < tweak_vectors.size(); ++index)
    {
        const auto& vector = tweak_vectors[index];
        ec_compressed point = splice(data_array<1>{ ec_even_sign }, vector.internal);
        BOOST_REQUIRE_MESSAGE(ec_add(point, vector.tweak), index);
        const ec_xonly key = slice<one, ec_compressed_size>(point);
        BOOST_REQUIRE_MESSAGE(key == vector.tweaked, index);
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_bip341__verify_commitment__tweak_vectors__expected)
{
    for (size_t index{}; index < tweak_vectors.size(); ++index)
    {
        const auto& vector = tweak_vectors[index];
        ec_compressed point = splice(data_array<1>{ ec_even_sign }, vector.internal);
        BOOST_REQUIRE_MESSAGE(ec_add(point, vector.tweak), index);

        const auto parity = point.front() == ec_odd_sign;
        BOOST_REQUIRE_MESSAGE(schnorr::verify_commitment(vector.internal, vector.tweak, vector.tweaked, parity), index);
        BOOST_REQUIRE_MESSAGE(!schnorr::verify_commitment(vector.internal, vector.tweak, vector.tweaked, !parity), index);
    }
}

// bitcoin/bips bip-0341/wallet-test-vectors.json (keyPathSpending)

struct spend_vector
{
    ec_secret secret;
    ec_xonly internal;
    hash_digest tweak;
    ec_secret tweaked;
    hash_digest sighash;
    ec_signature signature;
};

const std::vector<spend_vector> spend_vectors
{
    { base16_array("6b973d88838f27366ed61c9ad6367663045cb456e28335c109e30717ae0c6baa"), base16_array("d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961d"), base16_array("b86e7be8f39bab32a6f2c0443abbc210f0edac0e2c53d501b36b64437d9c6c70"), base16_array("2405b971772ad26915c8dcdf10f238753a9b837e5f8e6a86fd7c0cce5b7296d9"), base16_array("2514a6272f85cfa0f45eb907fcb0d121b808ed37c6ea160a5a9046ed5526d555"), base16_array("ed7c1647cb97379e76892be0cacff57ec4a7102aa24296ca39af7541246d8ff14d38958d4cc1e2e478e4d4a764bbfd835b16d4e314b72937b29833060b87276c") },
    { base16_array("1e4da49f6aaf4e5cd175fe08a32bb5cb4863d963921255f33d3bc31e1343907f"), base16_array("187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"), base16_array("cbd8679ba636c1110ea247542cfbd964131a6be84f873f7f3b62a777528ed001"), base16_array("ea260c3b10e60f6de018455cd0278f2f5b7e454be1999572789e6a9565d26080"), base16_array("325a644af47e8a5a2591cda0ab0723978537318f10e6a63d4eed783b96a71a4d"), base16_array("052aedffc554b41f52b521071793a6b88d6dbca9dba94cf34c83696de0c1ec35ca9c5ed4ab28059bd606a4f3a657eec0bb96661d42921b5f50a95ad33675b54f") },
    { base16_array("d3c7af07da2d54f7a7735d3d0fc4f0a73164db638b2f2f7c43f711f6d4aa7e64"), base16_array("93478e9488f956df2396be2ce6c5cced75f900dfa18e7dabd2428aae78451820"), base16_array("6af9e28dbf9d6aaf027696e2598a5b3d056f5fd2355a7fd5a37a0e5008132d30"), base16_array("97323385e57015b75b0339a549c56a948eb961555973f0951f555ae6039ef00d"), base16_array("bf013ea93474aa67815b1b6cc441d23b64fa310911d991e713cd34c7f5d46669"), base16_array("ff45f742a876139946a149ab4d9185574b98dc919d2eb6754f8abaa59d18b025637a3aa043b91817739554f4ed2026cf8022dbd83e351ce1fabc272841d2510a") },
    { base16_array("f36bb07a11e469ce941d16b63b11b9b9120a84d9d87cff2c84a8d4affb438f4e"), base16_array("e0dfe2300b0dd746a3f8674dfd4525623639042569d829c7f0eed9602d263e6f"), base16_array("b57bfa183d28eeb6ad688ddaabb265b4a41fbf68e5fed2c72c74de70d5a786f4"), base16_array("a8e7aa924f0d58854185a490e6c41f6efb7b675c0f3331b7f14b549400b4d501"), base16_array("4f900a0bae3f1446fd48490c2958b5a023228f01661cda3496a11da502a7f7ef"), base16_array("b4010dd48a617db09926f729e79c33ae0b4e94b79f04a1ae93ede6315eb3669de185a17d2b0ac9ee09fd4c64b678a0b61a0a86fa888a273c8511be83bfd6810f") },
    { base16_array("415cfe9c15d9cea27d8104d5517c06e9de48e2f986b695e4f5ffebf230e725d8"), base16_array("55adf4e8967fbd2e29f20ac896e60c3b0f1d5b0efa9d34941b5958c7b0a0312d"), base16_array("6579138e7976dc13b6a92f7bfd5a2fc7684f5ea42419d43368301470f3b74ed9"), base16_array("241c14f2639d0d7139282aa6abde28dd8a067baa9d633e4e7230287ec2d02901"), base16_array("15f25c298eb5cdc7eb1d638dd2d45c97c4c59dcaec6679cfc16ad84f30876b85"), base16_array("a3785919a2ce3c4ce26f298c3d51619bc474ae24014bcdd31328cd8cfbab2eff3395fa0a16fe5f486d12f22a9cedded5ae74feb4bbe5351346508c5405bcfee0") },
    { base16_array("c7b0e81f0a9a0b0499e112279d718cca98e79a12e2f137c72ae5b213aad0d103"), base16_array("ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592"), base16_array("9e0517edc8259bb3359255400b23ca9507f2a91cd1e4250ba068b4eafceba4a9"), base16_array("65b6000cd2bfa6b7cf736767a8955760e62b6649058cbc970b7c0871d786346b"), base16_array("cd292de50313804dabe4685e83f923d2969577191a3e1d2882220dca88cbeb10"), base16_array("ea0c6ba90763c2d3a296ad82ba45881abb4f426b3f87af162dd24d5109edc1cdd11915095ba47c3a9963dc1e6c432939872bc49212fe34c632cd3ab9fed429c4") },
    { base16_array("77863416be0d0665e517e1c375fd6f75839544eca553675ef7fdf4949518ebaa"), base16_array("f9f400803e683727b14f463836e1e78e1c64417638aa066919291a225f0e8dd8"), base16_array("639f0281b7ac49e742cd25b7f188657626da1ad169209078e2761cefd91fd65e"), base16_array("ec18ce6af99f43815db543f47b8af5ff5df3b2cb7315c955aa4a86e8143d2bf5"), base16_array("cccb739eca6c13a8a89e6e5cd317ffe55669bbda23f2fd37b0f18755e008edd2"), base16_array("bbc9584a11074e83bc8c6759ec55401f0ae7b03ef290c3139814f545b58a9f8127258000874f44bc46db7646322107d4d86aec8e73b8719a61fff761d75b5dd9") },
};

BOOST_AUTO_TEST_CASE(secp256k1_bip341__secret_to_public__spend_vectors__internal_key)
{
    for (size_t index{}; index < spend_vectors.size(); ++index)
    {
        const auto& vector = spend_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(secret_to_public(point, vector.secret), index);
        const ec_xonly key = slice<one, ec_compressed_size>(point);
        BOOST_REQUIRE_MESSAGE(key == vector.internal, index);
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_bip341__ec_add__spend_vectors__tweaked_secret)
{
    for (size_t index{}; index < spend_vectors.size(); ++index)
    {
        const auto& vector = spend_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(secret_to_public(point, vector.secret), index);

        auto secret = vector.secret;
        if (point.front() == ec_odd_sign)
        {
            BOOST_REQUIRE_MESSAGE(ec_negate(secret), index);
        }

        BOOST_REQUIRE_MESSAGE(ec_add(secret, vector.tweak), index);
        BOOST_REQUIRE_MESSAGE(secret == vector.tweaked, index);
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_bip341__verify_signature__spend_vectors__true)
{
    for (size_t index{}; index < spend_vectors.size(); ++index)
    {
        const auto& vector = spend_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(secret_to_public(point, vector.tweaked), index);

        const ec_xonly key = slice<one, ec_compressed_size>(point);
        BOOST_REQUIRE_MESSAGE(schnorr::verify_signature(key, vector.sighash, vector.signature), index);
    }
}

BOOST_AUTO_TEST_SUITE_END()
