/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/settings.hpp>

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

settings::settings() NOEXCEPT
  : initial_subsidy_bitcoin(50),
    subsidy_interval_blocks(210000),
    timestamp_limit_seconds(2 * 60 * 60),

    retargeting_factor(4),
    retargeting_interval_seconds(2 * 7 * 24 * 60 * 60),
    block_spacing_seconds(10 * 60),
    proof_of_work_limit(0x1d00ffff),

    first_version(1),
    bip34_version(2),
    bip66_version(3),
    bip65_version(4),
    bip9_version_bit0(1 << 0),
    bip9_version_bit1(1 << 1),
    bip9_version_bit2(1 << 2),
    bip9_version_base(0x20000000)
{
    forks.bip16 = true;
    forks.bip30 = true;
    forks.bip30_deactivate = true;
    forks.bip30_reactivate = true;
    forks.bip34 = true;
    forks.bip42 = true;
    forks.bip65 = true;
    forks.bip66 = true;
    forks.bip68 = true;
    forks.bip90 = true;
    forks.bip112 = true;
    forks.bip113 = true;
    forks.bip141 = true;
    forks.bip143 = true;
    forks.bip147 = true;
    forks.bip341 = true;
    forks.bip342 = true;
    forks.retarget = true;                 // !regtest
    forks.difficult = true;                // !testnet
    forks.time_warp_patch = false;         // litecoin
    forks.retarget_overflow_patch = false; // litecoin
    forks.scrypt_proof_of_work = false;    // litecoin
}

settings::settings(chain::selection context) NOEXCEPT
  : settings()
{
    switch (context)
    {
        case chain::selection::mainnet:
        {
            genesis_block = chain::block(data_chunk
            {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
                0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
                0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
                0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
                0x4b, 0x1e, 0x5e, 0x4a, 0x29, 0xab, 0x5f, 0x49,
                0xff, 0xff, 0x00, 0x1d, 0x1d, 0xac, 0x2b, 0x7c,
                0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
                0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
                0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
                0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
                0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
                0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
                0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
                0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
                0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
                0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
                0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
                0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
                0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
                0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
                0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
                0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
                0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
                0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
                0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
                0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
                0xac, 0x00, 0x00, 0x00, 0x00
            }, false);

            forks.bip16 = true;
            forks.bip30 = true;
            forks.bip30_deactivate = true;
            forks.bip30_reactivate = true;
            forks.bip34 = true;
            forks.bip42 = true;
            forks.bip65 = true;
            forks.bip66 = true;
            forks.bip68 = true;
            forks.bip90 = true;
            forks.bip112 = true;
            forks.bip113 = true;
            forks.bip141 = true;
            forks.bip143 = true;
            forks.bip147 = true;
            forks.bip341 = true;
            forks.bip342 = true;
            forks.retarget = true;                 // !regtest
            forks.difficult = true;                // !testnet
            forks.time_warp_patch = false;         // litecoin
            forks.retarget_overflow_patch = false; // litecoin
            forks.scrypt_proof_of_work = false;    // litecoin

            bip16_activation_time = 0x4f779a80;
            bip34_activation_threshold = 750;
            bip34_enforcement_threshold = 950;
            bip34_activation_sample = 1000;
            bip90_bip34_height = 227931;
            bip90_bip65_height = 388381;
            bip90_bip66_height = 363725;
            bip30_reactivate_height = 1983702;
            bip30_deactivate_checkpoint = { "000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8", 227931 };
            bip9_bit0_active_checkpoint = { "000000000000000004a1b34462cb8aeebd5799177f7a29cf28f2d1961716b5b5", 419328 };
            bip9_bit1_active_checkpoint = { "0000000000000000001c8018d9cb3b742ef25114f27563e3fc4a1902167f9893", 481824 };
            bip9_bit2_active_checkpoint = { "0000000000000000000687bca986194dc2c1f949318629b44bb54ec0a94d8244", 709632 };
            checkpoints =
            {
                { "00000000000000004d9b4ef50f0f9d686fd69db2e03af35a100370c64632a983", 295000 },
                { "0000000000000001ae8c72a0b0c301f67e3afca10e819efa9041e458e9bd7e40", 279000 },
                { "000000000000003887df1f29024b06fc2200b55f8af8f35453d7be294df2d214", 250000 },
                { "00000000000001c108384350f74090433e7fcf79a606b8e797f065b130575932", 225430 },
                { "00000000000001b4f4b433e81ee46494af945cf96014816a4e2370f11b23df4e", 216116 },
                { "000000000000048b95347e83192f69cf0366076336c639f9b7228e9ba171342e", 210000 },
                { "000000000000059f452a5f7340de6682a977387c17010ff6e6c3bd83ca8b1317", 193000 },
                { "000000000000099e61ea72015e79632f216fe6cb33d7899acb35b75c8303b763", 168000 },
                { "00000000000005b12ffd4cd315cd34ffd4a594f430ac814c91184a0d42d2b0fe", 134444 },
                { "00000000000291ce28027faea320c8d2b054b2e0fe44a773f3eefb151d6bdc97", 105000 },
                { "0000000000573993a3c9e41ce34471c079dcf5f52a0e824a81e7f953b8661a20", 74000 },
                { "000000002dd5588a74784eaa7ab0507a18ad16a236e7b1ce69f00d7ddfb5d0a6", 33333 },
                { "0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d", 11111 }
            };
            milestone = { "00000000000000000002a0b5db2a7f8d9087464c2586b546be7bce8eb53b8187", 850000 };
            minimum_work = base16_hash("000000000000000000000000000000000000000052b2559353df4117b7348b64");
            break;
        }

        case chain::selection::testnet:
        {
            genesis_block = chain::block(data_chunk
            {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
                0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
                0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
                0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
                0x4b, 0x1e, 0x5e, 0x4a, 0xda, 0xe5, 0x49, 0x4d,
                0xff, 0xff, 0x00, 0x1d, 0x1a, 0xa4, 0xae, 0x18,
                0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
                0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
                0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
                0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
                0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
                0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
                0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
                0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
                0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
                0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
                0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
                0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
                0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
                0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
                0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
                0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
                0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
                0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
                0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
                0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
                0xac, 0x00, 0x00, 0x00, 0x00
            }, false);

            forks.bip16 = true;
            forks.bip30 = true;
            forks.bip30_deactivate = true;
            forks.bip30_reactivate = true;
            forks.bip34 = true;
            forks.bip42 = true;
            forks.bip65 = true;
            forks.bip66 = true;
            forks.bip68 = true;
            forks.bip90 = true;
            forks.bip112 = true;
            forks.bip113 = true;
            forks.bip141 = true;
            forks.bip143 = true;
            forks.bip147 = true;
            forks.bip341 = true;
            forks.bip342 = true;
            forks.retarget = true;                 // !regtest
            forks.difficult = false;               // !testnet
            forks.time_warp_patch = false;         // litecoin
            forks.retarget_overflow_patch = false; // litecoin
            forks.scrypt_proof_of_work = false;    // litecoin

            bip16_activation_time = 0x4f3af580;
            bip34_activation_threshold = 51;
            bip34_enforcement_threshold = 75;
            bip34_activation_sample = 100;
            bip90_bip34_height = 21111;
            bip90_bip65_height = 581885;
            bip90_bip66_height = 330776;

            // Approximation, see satoshi PR#12204.
            bip30_reactivate_height = 486'000'000_size;

            bip30_deactivate_checkpoint = { "0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8", 21111 };
            bip9_bit0_active_checkpoint = { "00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb", 770112 };
            bip9_bit1_active_checkpoint = { "00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca", 834624 };
            bip9_bit2_active_checkpoint = { "000000000000eb906ab072652b86b05a1afeb95df53ed5c5a63aa3574e1f1b33", 2011968};
            checkpoints =
            {
                { "000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70", 546 }
            };
            milestone = { "0000000000000093bcb68c03a9a168ae252572d348a2eaeba2cdf9231d73206f", 2500000 };
            minimum_work = base16_hash("000000000000000000000000000000000000000000000b6a51f415a67c0da307");
            break;
        }

        case chain::selection::regtest:
        {
            proof_of_work_limit = 0x207fffff;
            subsidy_interval_blocks = 150;
            genesis_block = chain::block(data_chunk
            {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
                0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
                0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
                0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
                0x4b, 0x1e, 0x5e, 0x4a, 0xda, 0xe5, 0x49, 0x4d,
                0xff, 0xff, 0x7f, 0x20, 0x02, 0x00, 0x00, 0x00,
                0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
                0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
                0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
                0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
                0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
                0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
                0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
                0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
                0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
                0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
                0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
                0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
                0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
                0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
                0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
                0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
                0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
                0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
                0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
                0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
                0xac, 0x00, 0x00, 0x00, 0x00
            }, false);

            forks.bip16 = true;
            forks.bip30 = true;
            forks.bip30_deactivate = true;
            forks.bip30_reactivate = true;
            forks.bip34 = true;
            forks.bip42 = true;
            forks.bip65 = true;
            forks.bip66 = true;
            forks.bip68 = true;
            forks.bip90 = true;
            forks.bip112 = true;
            forks.bip113 = true;
            forks.bip141 = true;
            forks.bip143 = true;
            forks.bip147 = true;
            forks.bip341 = true;
            forks.bip342 = true;
            forks.retarget = false;                // !regtest
            forks.difficult = true;                // !testnet
            forks.time_warp_patch = false;         // litecoin
            forks.retarget_overflow_patch = false; // litecoin
            forks.scrypt_proof_of_work = false;    // litecoin

            bip16_activation_time = 0x4f3af580;
            bip34_activation_threshold = 0;
            bip34_enforcement_threshold = 0;
            bip34_activation_sample = 0;
            bip90_bip34_height = 100000000;
            bip90_bip65_height = 1351;
            bip90_bip66_height = 1251;
            bip30_reactivate_height = 0;
            bip30_deactivate_checkpoint = { "0000000000000000000000000000000000000000000000000000000000000000", 0 };

            // bip9's are fixed and closed, so assume genesis activation.
            // bip90 assumes a historical bip34 activation block, so use genesis.
            bip9_bit0_active_checkpoint = { "0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206", 0 };
            bip9_bit1_active_checkpoint = { "0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206", 0 };
            bip9_bit2_active_checkpoint = { "0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206", 0 };
            checkpoints = {};
            milestone = { "0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206", 0 };
            minimum_work = base16_hash("0000000000000000000000000000000000000000000000000000000000000000");
            break;
        }

        default:
        case chain::selection::none:
        {
        }
    }
}

// Computed properties.
// ----------------------------------------------------------------------------
// These are not used internal to system.

// The exact maximum amount of money (as of bip42).
//*****************************************************************************
// CONSENSUS: This assumes bip42 as otherwise money supply is unbounded.
//*****************************************************************************
//*****************************************************************************
// CONSENSUS: The satoshi client uses a "sanity check" value that is
// effectively based on a round but incorrect value of recursive_money,
// which is higher than this true value. Despite comments to the contrary
// in the satoshi code, the value could not be consensus critical unless it was
// *less* than the true value. This is non a consensus-critical method, as the
// consensus limit on money is stricly a function of subsidy recursion and
// overspend constraints.
//*****************************************************************************
uint64_t settings::max_money() const NOEXCEPT
{
    std::function<uint64_t(uint64_t)> total = [&](uint64_t subsidy) NOEXCEPT
    {
        // Guarded by parameterization (config).
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        return is_zero(subsidy) ? 0 : safe_add(subsidy, total(subsidy >> 1));
        BC_POP_WARNING()
    };

    // Guarded by parameterization (config).
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return safe_multiply<uint64_t>(total(initial_subsidy()),
        subsidy_interval_blocks);
    BC_POP_WARNING()
}

// The initial block subsidy in satoshis.
uint64_t settings::initial_subsidy() const NOEXCEPT
{
    return bitcoin_to_satoshi(initial_subsidy_bitcoin);
}

// Used to initialize initial subsidy setting.
uint64_t settings::bitcoin_to_satoshi(uint64_t value) const NOEXCEPT
{
    // Guarded by parameterization (config).
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return safe_multiply(value, chain::satoshi_per_bitcoin);
    BC_POP_WARNING()
}

// Computed properties.
// ----------------------------------------------------------------------------
// These are used by node.

chain::checkpoints settings::sorted_checkpoints() const NOEXCEPT
{
    return sort_copy(checkpoints);
}

chain::checkpoint settings::top_checkpoint() const NOEXCEPT
{
    if (checkpoints.empty())
        return { genesis_block.hash(), zero };

    return sorted_checkpoints().back();
}

// Computed properties.
// ----------------------------------------------------------------------------
// These are used internal to system.

// The lower bound for the retargeting timespan.
uint32_t settings::minimum_timespan() const NOEXCEPT
{
    // TODO: optimize to prevent recomputation.
    return chain::chain_state::minimum_timespan(retargeting_interval_seconds,
        retargeting_factor);
}

// The upper bound for the retargeting timespan.
uint32_t settings::maximum_timespan() const NOEXCEPT
{
    // TODO: optimize to prevent recomputation.
    return chain::chain_state::maximum_timespan(retargeting_interval_seconds,
        retargeting_factor);
}

// The target number of blocks for 2 weeks of work (2016 blocks).
size_t settings::retargeting_interval() const NOEXCEPT
{
    // TODO: optimize to prevent recomputation.
    return chain::chain_state::retargeting_interval(
        retargeting_interval_seconds, block_spacing_seconds);
}

} // namespace system
} // namespace libbitcoin
