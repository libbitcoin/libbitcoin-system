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

BOOST_AUTO_TEST_SUITE(secp256k1_bip324_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    template <typename Word>
    using field_t = algorithm::field_t<Word>;
    using bytes_t = algorithm::bytes_t;
    using algorithm::from_bytes;
    using algorithm::to_bytes;
    using algorithm::swift_inverse;
};

using field = accessor::field_t<uint64_t>;
using bytes = accessor::bytes_t;

static data_array<ec_xonly_size> to_x(const ec_compressed& point) NOEXCEPT
{
    return slice<one, ec_compressed_size>(point);
}

// bitcoin/bips bip-0324/ellswift_decode_test_vectors.csv

struct decode_vector
{
    ec_ellswift encoded;
    data_array<ec_xonly_size> x;
};

const std::vector<decode_vector> decode_vectors
{
    { base16_array("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"), base16_array("edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c") },
    { base16_array("000000000000000000000000000000000000000000000000000000000000000001d3475bf7655b0fb2d852921035b2ef607f49069b97454e6795251062741771"), base16_array("b5da00b73cd6560520e7c364086e7cd23a34bf60d0e707be9fc34d4cd5fdfa2c") },
    { base16_array("000000000000000000000000000000000000000000000000000000000000000082277c4a71f9d22e66ece523f8fa08741a7c0912c66a69ce68514bfd3515b49f"), base16_array("f482f2e241753ad0fb89150d8491dc1e34ff0b8acfbb442cfe999e2e5e6fd1d2") },
    { base16_array("00000000000000000000000000000000000000000000000000000000000000008421cc930e77c9f514b6915c3dbe2a94c6d8f690b5b739864ba6789fb8a55dd0"), base16_array("9f59c40275f5085a006f05dae77eb98c6fd0db1ab4a72ac47eae90a4fc9e57e0") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000bde70df51939b94c9c24979fa7dd04ebd9b3572da7802290438af2a681895441"), base16_array("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa9fffffd6b") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000d19c182d2759cd99824228d94799f8c6557c38a1c0d6779b9d4b729c6f1ccc42"), base16_array("70720db7e238d04121f5b1afd8cc5ad9d18944c6bdc94881f502b7a3af3aecff") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff2664bbd5"), base16_array("50873db31badcc71890e4f67753a65757f97aaa7dd5f1e82b753ace32219064b") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff7028de7d"), base16_array("1eea9cc59cfcf2fa151ac6c274eea4110feb4f7b68c5965732e9992e976ef68e") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffcbcfb7e7"), base16_array("12303941aedc208880735b1f1795c8e55be520ea93e103357b5d2adb7ed59b8e") },
    { base16_array("0000000000000000000000000000000000000000000000000000000000000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffff3113ad9"), base16_array("7eed6b70e7b0767c7d7feac04e57aa2a12fef5e0f48f878fcbb88b3b6b5e0783") },
    { base16_array("0a2d2ba93507f1df233770c2a797962cc61f6d15da14ecd47d8d27ae1cd5f8530000000000000000000000000000000000000000000000000000000000000000"), base16_array("532167c11200b08c0e84a354e74dcc40f8b25f4fe686e30869526366278a0688") },
    { base16_array("0a2d2ba93507f1df233770c2a797962cc61f6d15da14ecd47d8d27ae1cd5f853fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("532167c11200b08c0e84a354e74dcc40f8b25f4fe686e30869526366278a0688") },
    { base16_array("0ffde9ca81d751e9cdaffc1a50779245320b28996dbaf32f822f20117c22fbd6c74d99efceaa550f1ad1c0f43f46e7ff1ee3bd0162b7bf55f2965da9c3450646"), base16_array("74e880b3ffd18fe3cddf7902522551ddf97fa4a35a3cfda8197f947081a57b8f") },
    { base16_array("0ffde9ca81d751e9cdaffc1a50779245320b28996dbaf32f822f20117c22fbd6ffffffffffffffffffffffffffffffffffffffffffffffffffffffff156ca896"), base16_array("377b643fce2271f64e5c8101566107c1be4980745091783804f654781ac9217c") },
    { base16_array("123658444f32be8f02ea2034afa7ef4bbe8adc918ceb49b12773b625f490b368ffffffffffffffffffffffffffffffffffffffffffffffffffffffff8dc5fe11"), base16_array("ed16d65cf3a9538fcb2c139f1ecbc143ee14827120cbc2659e667256800b8142") },
    { base16_array("146f92464d15d36e35382bd3ca5b0f976c95cb08acdcf2d5b3570617990839d7ffffffffffffffffffffffffffffffffffffffffffffffffffffffff3145e93b"), base16_array("0d5cd840427f941f65193079ab8e2e83024ef2ee7ca558d88879ffd879fb6657") },
    { base16_array("15fdf5cf09c90759add2272d574d2bb5fe1429f9f3c14c65e3194bf61b82aa73ffffffffffffffffffffffffffffffffffffffffffffffffffffffff04cfd906"), base16_array("16d0e43946aec93f62d57eb8cde68951af136cf4b307938dd1447411e07bffe1") },
    { base16_array("1f67edf779a8a649d6def60035f2fa22d022dd359079a1a144073d84f19b92d50000000000000000000000000000000000000000000000000000000000000000"), base16_array("025661f9aba9d15c3118456bbe980e3e1b8ba2e047c737a4eb48a040bb566f6c") },
    { base16_array("1f67edf779a8a649d6def60035f2fa22d022dd359079a1a144073d84f19b92d5fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("025661f9aba9d15c3118456bbe980e3e1b8ba2e047c737a4eb48a040bb566f6c") },
    { base16_array("1fe1e5ef3fceb5c135ab7741333ce5a6e80d68167653f6b2b24bcbcfaaaff507fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("98bec3b2a351fa96cfd191c1778351931b9e9ba9ad1149f6d9eadca80981b801") },
    { base16_array("4056a34a210eec7892e8820675c860099f857b26aad85470ee6d3cf1304a9dcf375e70374271f20b13c9986ed7d3c17799698cfc435dbed3a9f34b38c823c2b4"), base16_array("868aac2003b29dbcad1a3e803855e078a89d16543ac64392d122417298cec76e") },
    { base16_array("4197ec3723c654cfdd32ab075506648b2ff5070362d01a4fff14b336b78f963fffffffffffffffffffffffffffffffffffffffffffffffffffffffffb3ab1e95"), base16_array("ba5a6314502a8952b8f456e085928105f665377a8ce27726a5b0eb7ec1ac0286") },
    { base16_array("47eb3e208fedcdf8234c9421e9cd9a7ae873bfbdbc393723d1ba1e1e6a8e6b24ffffffffffffffffffffffffffffffffffffffffffffffffffffffff7cd12cb1"), base16_array("d192d52007e541c9807006ed0468df77fd214af0a795fe119359666fdcf08f7c") },
    { base16_array("5eb9696a2336fe2c3c666b02c755db4c0cfd62825c7b589a7b7bb442e141c1d693413f0052d49e64abec6d5831d66c43612830a17df1fe4383db896468100221"), base16_array("ef6e1da6d6c7627e80f7a7234cb08a022c1ee1cf29e4d0f9642ae924cef9eb38") },
    { base16_array("7bf96b7b6da15d3476a2b195934b690a3a3de3e8ab8474856863b0de3af90b0e0000000000000000000000000000000000000000000000000000000000000000"), base16_array("50851dfc9f418c314a437295b24feeea27af3d0cd2308348fda6e21c463e46ff") },
    { base16_array("7bf96b7b6da15d3476a2b195934b690a3a3de3e8ab8474856863b0de3af90b0efffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("50851dfc9f418c314a437295b24feeea27af3d0cd2308348fda6e21c463e46ff") },
    { base16_array("851b1ca94549371c4f1f7187321d39bf51c6b7fb61f7cbf027c9da62021b7a65fc54c96837fb22b362eda63ec52ec83d81bedd160c11b22d965d9f4a6d64d251"), base16_array("3e731051e12d33237eb324f2aa5b16bb868eb49a1aa1fadc19b6e8761b5a5f7b") },
    { base16_array("943c2f775108b737fe65a9531e19f2fc2a197f5603e3a2881d1d83e4008f91250000000000000000000000000000000000000000000000000000000000000000"), base16_array("311c61f0ab2f32b7b1f0223fa72f0a78752b8146e46107f8876dd9c4f92b2942") },
    { base16_array("943c2f775108b737fe65a9531e19f2fc2a197f5603e3a2881d1d83e4008f9125fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("311c61f0ab2f32b7b1f0223fa72f0a78752b8146e46107f8876dd9c4f92b2942") },
    { base16_array("a0f18492183e61e8063e573606591421b06bc3513631578a73a39c1c3306239f2f32904f0d2a33ecca8a5451705bb537d3bf44e071226025cdbfd249fe0f7ad6"), base16_array("97a09cf1a2eae7c494df3c6f8a9445bfb8c09d60832f9b0b9d5eabe25fbd14b9") },
    { base16_array("a1ed0a0bd79d8a23cfe4ec5fef5ba5cccfd844e4ff5cb4b0f2e71627341f1c5b17c499249e0ac08d5d11ea1c2c8ca7001616559a7994eadec9ca10fb4b8516dc"), base16_array("65a89640744192cdac64b2d21ddf989cdac7500725b645bef8e2200ae39691f2") },
    { base16_array("ba94594a432721aa3580b84c161d0d134bc354b690404d7cd4ec57c16d3fbe98ffffffffffffffffffffffffffffffffffffffffffffffffffffffffea507dd7"), base16_array("5e0d76564aae92cb347e01a62afd389a9aa401c76c8dd227543dc9cd0efe685a") },
    { base16_array("bcaf7219f2f6fbf55fe5e062dce0e48c18f68103f10b8198e974c184750e1be3932016cbf69c4471bd1f656c6a107f1973de4af7086db897277060e25677f19a"), base16_array("2d97f96cac882dfe73dc44db6ce0f1d31d6241358dd5d74eb3d3b50003d24c2b") },
    { base16_array("bcaf7219f2f6fbf55fe5e062dce0e48c18f68103f10b8198e974c184750e1be3ffffffffffffffffffffffffffffffffffffffffffffffffffffffff6507d09a"), base16_array("e7008afe6e8cbd5055df120bd748757c686dadb41cce75e4addcc5e02ec02b44") },
    { base16_array("c5981bae27fd84401c72a155e5707fbb811b2b620645d1028ea270cbe0ee225d4b62aa4dca6506c1acdbecc0552569b4b21436a5692e25d90d3bc2eb7ce24078"), base16_array("948b40e7181713bc018ec1702d3d054d15746c59a7020730dd13ecf985a010d7") },
    { base16_array("c894ce48bfec433014b931a6ad4226d7dbd8eaa7b6e3faa8d0ef94052bcf8cff336eeb3919e2b4efb746c7f71bbca7e9383230fbbc48ffafe77e8bcc69542471"), base16_array("f1c91acdc2525330f9b53158434a4d43a1c547cff29f15506f5da4eb4fe8fa5a") },
    { base16_array("cbb0deab125754f1fdb2038b0434ed9cb3fb53ab735391129994a535d925f6730000000000000000000000000000000000000000000000000000000000000000"), base16_array("872d81ed8831d9998b67cb7105243edbf86c10edfebb786c110b02d07b2e67cd") },
    { base16_array("d917b786dac35670c330c9c5ae5971dfb495c8ae523ed97ee2420117b171f41effffffffffffffffffffffffffffffffffffffffffffffffffffffff2001f6f6"), base16_array("e45b71e110b831f2bdad8651994526e58393fde4328b1ec04d59897142584691") },
    { base16_array("e28bd8f5929b467eb70e04332374ffb7e7180218ad16eaa46b7161aa679eb4260000000000000000000000000000000000000000000000000000000000000000"), base16_array("66b8c980a75c72e598d383a35a62879f844242ad1e73ff12edaa59f4e58632b5") },
    { base16_array("e28bd8f5929b467eb70e04332374ffb7e7180218ad16eaa46b7161aa679eb426fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("66b8c980a75c72e598d383a35a62879f844242ad1e73ff12edaa59f4e58632b5") },
    { base16_array("e7ee5814c1706bf8a89396a9b032bc014c2cac9c121127dbf6c99278f8bb53d1dfd04dbcda8e352466b6fcd5f2dea3e17d5e133115886eda20db8a12b54de71b"), base16_array("e842c6e3529b234270a5e97744edc34a04d7ba94e44b6d2523c9cf0195730a50") },
    { base16_array("f292e46825f9225ad23dc057c1d91c4f57fcb1386f29ef10481cb1d22518593fffffffffffffffffffffffffffffffffffffffffffffffffffffffff7011c989"), base16_array("3cea2c53b8b0170166ac7da67194694adacc84d56389225e330134dab85a4d55") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f0000000000000000000000000000000000000000000000000000000000000000"), base16_array("edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f01d3475bf7655b0fb2d852921035b2ef607f49069b97454e6795251062741771"), base16_array("b5da00b73cd6560520e7c364086e7cd23a34bf60d0e707be9fc34d4cd5fdfa2c") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f4218f20ae6c646b363db68605822fb14264ca8d2587fdd6fbc750d587e76a7ee"), base16_array("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa9fffffd6b") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f82277c4a71f9d22e66ece523f8fa08741a7c0912c66a69ce68514bfd3515b49f"), base16_array("f482f2e241753ad0fb89150d8491dc1e34ff0b8acfbb442cfe999e2e5e6fd1d2") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f8421cc930e77c9f514b6915c3dbe2a94c6d8f690b5b739864ba6789fb8a55dd0"), base16_array("9f59c40275f5085a006f05dae77eb98c6fd0db1ab4a72ac47eae90a4fc9e57e0") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2fd19c182d2759cd99824228d94799f8c6557c38a1c0d6779b9d4b729c6f1ccc42"), base16_array("70720db7e238d04121f5b1afd8cc5ad9d18944c6bdc94881f502b7a3af3aecff") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2ffffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2fffffffffffffffffffffffffffffffffffffffffffffffffffffffff2664bbd5"), base16_array("50873db31badcc71890e4f67753a65757f97aaa7dd5f1e82b753ace32219064b") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2fffffffffffffffffffffffffffffffffffffffffffffffffffffffff7028de7d"), base16_array("1eea9cc59cfcf2fa151ac6c274eea4110feb4f7b68c5965732e9992e976ef68e") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2fffffffffffffffffffffffffffffffffffffffffffffffffffffffffcbcfb7e7"), base16_array("12303941aedc208880735b1f1795c8e55be520ea93e103357b5d2adb7ed59b8e") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3113ad9"), base16_array("7eed6b70e7b0767c7d7feac04e57aa2a12fef5e0f48f878fcbb88b3b6b5e0783") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff13cea4a70000000000000000000000000000000000000000000000000000000000000000"), base16_array("649984435b62b4a25d40c6133e8d9ab8c53d4b059ee8a154a3be0fcf4e892edb") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff13cea4a7fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("649984435b62b4a25d40c6133e8d9ab8c53d4b059ee8a154a3be0fcf4e892edb") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff15028c590063f64d5a7f1c14915cd61eac886ab295bebd91992504cf77edb028bdd6267f"), base16_array("3fde5713f8282eead7d39d4201f44a7c85a5ac8a0681f35e54085c6b69543374") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff2715de860000000000000000000000000000000000000000000000000000000000000000"), base16_array("3524f77fa3a6eb4389c3cb5d27f1f91462086429cd6c0cb0df43ea8f1e7b3fb4") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff2715de86fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("3524f77fa3a6eb4389c3cb5d27f1f91462086429cd6c0cb0df43ea8f1e7b3fb4") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff2c2c5709e7156c417717f2feab147141ec3da19fb759575cc6e37b2ea5ac9309f26f0f66"), base16_array("d2469ab3e04acbb21c65a1809f39caafe7a77c13d10f9dd38f391c01dc499c52") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff3a08cc1efffffffffffffffffffffffffffffffffffffffffffffffffffffffff760e9f0"), base16_array("38e2a5ce6a93e795e16d2c398bc99f0369202ce21e8f09d56777b40fc512bccc") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff3e91257d932016cbf69c4471bd1f656c6a107f1973de4af7086db897277060e25677f19a"), base16_array("864b3dc902c376709c10a93ad4bbe29fce0012f3dc8672c6286bba28d7d6d6fc") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff795d6c1c322cadf599dbb86481522b3cc55f15a67932db2afa0111d9ed6981bcd124bf44"), base16_array("766dfe4a700d9bee288b903ad58870e3d4fe2f0ef780bcac5c823f320d9a9bef") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff8e426f0392389078c12b1a89e9542f0593bc96b6bfde8224f8654ef5d5cda935a3582194"), base16_array("faec7bc1987b63233fbc5f956edbf37d54404e7461c58ab8631bc68e451a0478") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff91192139ffffffffffffffffffffffffffffffffffffffffffffffffffffffff45f0f1eb"), base16_array("ec29a50bae138dbf7d8e24825006bb5fc1a2cc1243ba335bc6116fb9e498ec1f") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff98eb9ab76e84499c483b3bf06214abfe065dddf43b8601de596d63b9e45a166a580541fe"), base16_array("1e0ff2dee9b09b136292a9e910f0d6ac3e552a644bba39e64e9dd3e3bbd3d4d4") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff9b77b7f2c74d99efceaa550f1ad1c0f43f46e7ff1ee3bd0162b7bf55f2965da9c3450646"), base16_array("8b7dd5c3edba9ee97b70eff438f22dca9849c8254a2f3345a0a572ffeaae0928") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff9b77b7f2ffffffffffffffffffffffffffffffffffffffffffffffffffffffff156ca896"), base16_array("0881950c8f51d6b9a6387465d5f12609ef1bb25412a08a74cb2dfb200c74bfbf") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffa2f5cd838816c16c4fe8a1661d606fdb13cf9af04b979a2e159a09409ebc8645d58fde02"), base16_array("2f083207b9fd9b550063c31cd62b8746bd543bdc5bbf10e3a35563e927f440c8") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffb13f75c00000000000000000000000000000000000000000000000000000000000000000"), base16_array("4f51e0be078e0cddab2742156adba7e7a148e73157072fd618cd60942b146bd0") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffb13f75c0fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("4f51e0be078e0cddab2742156adba7e7a148e73157072fd618cd60942b146bd0") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffe7bc1f8d0000000000000000000000000000000000000000000000000000000000000000"), base16_array("16c2ccb54352ff4bd794f6efd613c72197ab7082da5b563bdf9cb3edaafe74c2") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffe7bc1f8dfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), base16_array("16c2ccb54352ff4bd794f6efd613c72197ab7082da5b563bdf9cb3edaafe74c2") },
    { base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffef64d162750546ce42b0431361e52d4f5242d8f24f33e6b1f99b591647cbc808f462af51"), base16_array("d41244d11ca4f65240687759f95ca9efbab767ededb38fd18c36e18cd3b6f6a9") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffff0e5be52372dd6e894b2a326fc3605a6e8f3c69c710bf27d630dfe2004988b78eb6eab36"), base16_array("64bf84dd5e03670fdb24c0f5d3c2c365736f51db6c92d95010716ad2d36134c8") },
    { base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffefbb982fffffffffffffffffffffffffffffffffffffffffffffffffffffffff6d6db1f"), base16_array("1c92ccdfcf4ac550c28db57cff0c8515cb26936c786584a70114008d6c33a34b") },
};

BOOST_AUTO_TEST_CASE(secp256k1_bip324__ellswift_decode__vectors__expected_x)
{
    for (size_t index{}; index < decode_vectors.size(); ++index)
    {
        const auto& vector = decode_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(ellswift::decode(point, vector.encoded), index);
        BOOST_REQUIRE_MESSAGE(to_x(point) == vector.x, index);
    }
}

// bitcoin/bips bip-0324/xswiftec_inv_test_vectors.csv

struct inverse_vector
{
    bytes u;
    bytes x;
    uint8_t valid;
    std_array<bytes, 8> t;
};

const std::vector<inverse_vector> inverse_vectors
{
    { base16_array("05ff6bdad900fc3261bc7fe34e2fb0f569f06e091ae437d3a52e9da0cbfb9590"), base16_array("80cdf63774ec7022c89a5a8558e373a279170285e0ab27412dbce510bdfe23fc"), 0xcc, { bytes{}, bytes{}, base16_array("45654798ece071ba79286d04f7f3eb1c3f1d17dd883610f2ad2efd82a287466b"), base16_array("0aeaa886f6b76c7158452418cbf5033adc5747e9e9b5d3b2303db96936528557"), bytes{}, bytes{}, base16_array("ba9ab867131f8e4586d792fb080c14e3c0e2e82277c9ef0d52d1027c5d78b5c4"), base16_array("f51557790948938ea7badbe7340afcc523a8b816164a2c4dcfc24695c9ad76d8") } },
    { base16_array("1737a85f4c8d146cec96e3ffdca76d9903dcf3bd53061868d478c78c63c2aa9e"), base16_array("39e48dd150d2f429be088dfd5b61882e7e8407483702ae9a5ab35927b15f85ea"), 0x33, { base16_array("1be8cc0b04be0c681d0c6a68f733f82c6c896e0c8a262fcd392918e303a7abf4"), base16_array("605b5814bf9b8cb066667c9e5480d22dc5b6c92f14b4af3ee0a9eb83b03685e3"), bytes{}, bytes{}, base16_array("e41733f4fb41f397e2f3959708cc07d3937691f375d9d032c6d6e71bfc58503b"), base16_array("9fa4a7eb4064734f99998361ab7f2dd23a4936d0eb4b50c11f56147b4fc9764c"), bytes{}, bytes{} } },
    { base16_array("1aaa1ccebf9c724191033df366b36f691c4d902c228033ff4516d122b2564f68"), base16_array("c75541259d3ba98f207eaa30c69634d187d0b6da594e719e420f4898638fc5b0"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("2323a1d079b0fd72fc8bb62ec34230a815cb0596c2bfac998bd6b84260f5dc26"), base16_array("239342dfb675500a34a196310b8d87d54f49dcac9da50c1743ceab41a7b249ff"), 0x33, { base16_array("f63580b8aa49c4846de56e39e1b3e73f171e881eba8c66f614e67e5c975dfc07"), base16_array("b6307b332e699f1cf77841d90af25365404deb7fed5edb3090db49e642a156b6"), bytes{}, bytes{}, base16_array("09ca7f4755b63b7b921a91c61e4c18c0e8e177e145739909eb1981a268a20028"), base16_array("49cf84ccd19660e30887be26f50dac9abfb2148012a124cf6f24b618bd5ea579"), bytes{}, bytes{} } },
    { base16_array("2dc90e640cb646ae9164c0b5a9ef0169febe34dc4437d6e46acb0e27e219d1e8"), base16_array("d236f19bf349b9516e9b3f4a5610fe960141cb23bbc8291b9534f1d71de62a47"), 0x33, { base16_array("e69df7d9c026c36600ebdf588072675847c0c431c8eb730682533e964b6252c9"), base16_array("4f18bbdf7c2d6c5f818c18802fa35cd069eaa79fff74e4fc837c80d93fece2f8"), bytes{}, bytes{}, base16_array("196208263fd93c99ff1420a77f8d98a7b83f3bce37148cf97dacc168b49da966"), base16_array("b0e7442083d293a07e73e77fd05ca32f96155860008b1b037c837f25c0131937"), bytes{}, bytes{} } },
    { base16_array("3edd7b3980e2f2f34d1409a207069f881fda5f96f08027ac4465b63dc278d672"), base16_array("053a98de4a27b1961155822b3a3121f03b2a14458bd80eb4a560c4c7a85c149c"), 0xcc, { bytes{}, bytes{}, base16_array("b3dae4b7dcf858e4c6968057cef2b156465431526538199cf52dc1b2d62fda30"), base16_array("4aa77dd55d6b6d3cfa10cc9d0fe42f79232e4575661049ae36779c1d0c666d88"), bytes{}, bytes{}, base16_array("4c251b482307a71b39697fa8310d4ea9b9abcead9ac7e6630ad23e4c29d021ff"), base16_array("b558822aa29492c305ef3362f01bd086dcd1ba8a99efb651c98863e1f3998ea7") } },
    { base16_array("4295737efcb1da6fb1d96b9ca7dcd1e320024b37a736c4948b62598173069f70"), base16_array("fa7ffe4f25f88362831c087afe2e8a9b0713e2cac1ddca6a383205a266f14307"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("587c1a0cee91939e7f784d23b963004a3bf44f5d4e32a0081995ba20b0fca59e"), base16_array("2ea988530715e8d10363907ff25124524d471ba2454d5ce3be3f04194dfd3a3c"), 0xff, { base16_array("cfd5a094aa0b9b8891b76c6ab9438f66aa1c095a65f9f70135e8171292245e74"), base16_array("a89057d7c6563f0d6efa19ae84412b8a7b47e791a191ecdfdf2af84fd97bc339"), base16_array("475d0ae9ef46920df07b34117be5a0817de1023e3cc32689e9be145b406b0aef"), base16_array("a0759178ad80232454f827ef05ea3e72ad8d75418e6d4cc1cd4f5306c5e7c453"), base16_array("302a5f6b55f464776e48939546bc709955e3f6a59a0608feca17e8ec6ddb9dbb"), base16_array("576fa82839a9c0f29105e6517bbed47584b8186e5e6e132020d507af268438f6"), base16_array("b8a2f51610b96df20f84cbee841a5f7e821efdc1c33cd9761641eba3bf94f140"), base16_array("5f8a6e87527fdcdbab07d810fa15c18d52728abe7192b33e32b0acf83a1837dc") } },
    { base16_array("5fa88b3365a635cbbcee003cce9ef51dd1a310de277e441abccdb7be1e4ba249"), base16_array("79461ff62bfcbcac4249ba84dd040f2cec3c63f725204dc7f464c16bf0ff3170"), 0xcc, { bytes{}, bytes{}, base16_array("6bb700e1f4d7e236e8d193ff4a76c1b3bcd4e2b25acac3d51c8dac653fe909a0"), base16_array("f4c73410633da7f63a4f1d55aec6dd32c4c6d89ee74075edb5515ed90da9e683"), bytes{}, bytes{}, base16_array("9448ff1e0b281dc9172e6c00b5893e4c432b1d4da5353c2ae3725399c016f28f"), base16_array("0b38cbef9cc25809c5b0e2aa513922cd3b39276118bf8a124aaea125f25615ac") } },
    { base16_array("6fb31c7531f03130b42b155b952779efbb46087dd9807d241a48eac63c3d96d6"), base16_array("56f81be753e8d4ae4940ea6f46f6ec9fda66a6f96cc95f506cb2b57490e94260"), 0xcc, { bytes{}, bytes{}, base16_array("59059774795bdb7a837fbe1140a5fa59984f48af8df95d57dd6d1c05437dcec1"), base16_array("22a644db79376ad4e7b3a009e58b3f13137c54fdf911122cc93667c47077d784"), bytes{}, bytes{}, base16_array("a6fa688b86a424857c8041eebf5a05a667b0b7507206a2a82292e3f9bc822d6e"), base16_array("dd59bb2486c8952b184c5ff61a74c0ecec83ab0206eeedd336c9983a8f8824ab") } },
    { base16_array("704cd226e71cb6826a590e80dac90f2d2f5830f0fdf135a3eae3965bff25ff12"), base16_array("138e0afa68936ee670bd2b8db53aedbb7bea2a8597388b24d0518edd22ad66ec"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("725e914792cb8c8949e7e1168b7cdd8a8094c91c6ec2202ccd53a6a18771edeb"), base16_array("8da16eb86d347376b6181ee9748322757f6b36e3913ddfd332ac595d788e0e44"), 0x33, { base16_array("dd357786b9f6873330391aa5625809654e43116e82a5a5d82ffd1d6624101fc4"), base16_array("a0b7efca01814594c59c9aae8e49700186ca5d95e88bcc80399044d9c2d8613d"), bytes{}, bytes{}, base16_array("22ca8879460978cccfc6e55a9da7f69ab1bcee917d5a5a27d002e298dbefdc6b"), base16_array("5f481035fe7eba6b3a63655171b68ffe7935a26a1774337fc66fbb253d279af2"), bytes{}, bytes{} } },
    { base16_array("78fe6b717f2ea4a32708d79c151bf503a5312a18c0963437e865cc6ed3f6ae97"), base16_array("8701948e80d15b5cd8f72863eae40afc5aced5e73f69cbc8179a33902c094d98"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("7c37bb9c5061dc07413f11acd5a34006e64c5c457fdb9a438f217255a961f50d"), base16_array("5c1a76b44568eb59d6789a7442d9ed7cdc6226b7752b4ff8eaf8e1a95736e507"), 0x44, { bytes{}, bytes{}, base16_array("b94d30cd7dbff60b64620c17ca0fafaa40b3d1f52d077a60a2e0cafd145086c2"), bytes{}, bytes{}, bytes{}, base16_array("46b2cf32824009f49b9df3e835f05055bf4c2e0ad2f8859f5d1f3501ebaf756d"), bytes{} } },
    { base16_array("82388888967f82a6b444438a7d44838e13c0d478b9ca060da95a41fb94303de6"), base16_array("29e9654170628fec8b4972898b113cf98807f4609274f4f3140d0674157c90a0"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("91298f5770af7a27f0a47188d24c3b7bf98ab2990d84b0b898507e3c561d6472"), base16_array("144f4ccbd9a74698a88cbf6fd00ad886d339d29ea19448f2c572cac0a07d5562"), 0x33, { base16_array("e6a0ffa3807f09dadbe71e0f4be4725f2832e76cad8dc1d943ce839375eff248"), base16_array("837b8e68d4917544764ad0903cb11f8615d2823cefbb06d89049dbabc69befda"), bytes{}, bytes{}, base16_array("195f005c7f80f6252418e1f0b41b8da0d7cd189352723e26bc317c6b8a1009e7"), base16_array("7c8471972b6e8abb89b52f6fc34ee079ea2d7dc31044f9276fb6245339640c55"), bytes{}, bytes{} } },
    { base16_array("b682f3d03bbb5dee4f54b5ebfba931b4f52f6a191e5c2f483c73c66e9ace97e1"), base16_array("904717bf0bc0cb7873fcdc38aa97f19e3a62630972acff92b24cc6dda197cb96"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("c17ec69e665f0fb0dbab48d9c2f94d12ec8a9d7eacb58084833091801eb0b80b"), base16_array("147756e66d96e31c426d3cc85ed0c4cfbef6341dd8b285585aa574ea0204b55e"), 0x77, { base16_array("6f4aea431a0043bdd03134d6d9159119ce034b88c32e50e8e36c4ee45eac7ae9"), base16_array("fd5be16d4ffa2690126c67c3ef7cb9d29b74d397c78b06b3605fda34dc9696a6"), base16_array("5e9c60792a2f000e45c6250f296f875e174efc0e9703e628706103a9dd2d82c7"), bytes{}, base16_array("90b515bce5ffbc422fcecb2926ea6ee631fcb4773cd1af171c93b11aa1538146"), base16_array("02a41e92b005d96fed93983c1083462d648b2c683874f94c9fa025ca23696589"), base16_array("a1639f86d5d0fff1ba39daf0d69078a1e8b103f168fc19d78f9efc5522d27968"), bytes{} } },
    { base16_array("c25172fc3f29b6fc4a1155b8575233155486b27464b74b8b260b499a3f53cb14"), base16_array("1ea9cbdb35cf6e0329aa31b0bb0a702a65123ed008655a93b7dcd5280e52e1ab"), 0xcc, { bytes{}, bytes{}, base16_array("7422edc7843136af0053bb8854448a8299994f9ddcefd3a9a92d45462c59298a"), base16_array("78c7774a266f8b97ea23d05d064f033c77319f923f6b78bce4e20bf05fa5398d"), bytes{}, bytes{}, base16_array("8bdd12387bcec950ffac4477abbb757d6666b06223102c5656d2bab8d3a6d2a5"), base16_array("873888b5d990746815dc2fa2f9b0fcc388ce606dc09487431b1df40ea05ac2a2") } },
    { base16_array("cab6626f832a4b1280ba7add2fc5322ff011caededf7ff4db6735d5026dc0367"), base16_array("2b2bef0852c6f7c95d72ac99a23802b875029cd573b248d1f1b3fc8033788eb6"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("d8621b4ffc85b9ed56e99d8dd1dd24aedcecb14763b861a17112dc771a104fd2"), base16_array("812cabe972a22aa67c7da0c94d8a936296eb9949d70c37cb2b2487574cb3ce58"), 0x33, { base16_array("fbc5febc6fdbc9ae3eb88a93b982196e8b6275a6d5a73c17387e000c711bd0e3"), base16_array("8724c96bd4e5527f2dd195a51c468d2d211ba2fac7cbe0b4b3434253409fb42d"), bytes{}, bytes{}, base16_array("043a014390243651c147756c467de691749d8a592a58c3e8c781fff28ee42b4c"), base16_array("78db36942b1aad80d22e6a5ae3b972d2dee45d0538341f4b4cbcbdabbf604802"), bytes{}, bytes{} } },
    { base16_array("da463164c6f4bf7129ee5f0ec00f65a675a8adf1bd931b39b64806afdcda9a22"), base16_array("25b9ce9b390b408ed611a0f13ff09a598a57520e426ce4c649b7f94f2325620d"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("dafc971e4a3a7b6dcfb42a08d9692d82ad9e7838523fcbda1d4827e14481ae2d"), base16_array("250368e1b5c58492304bd5f72696d27d526187c7adc03425e2b7d81dbb7e4e02"), 0xcc, { bytes{}, bytes{}, base16_array("370c28f1be665efacde6aa436bf86fe21e6e314c1e53dd040e6c73a46b4c8c49"), base16_array("cd8acee98ffe56531a84d7eb3e48fa4034206ce825ace907d0edf0eaeb5e9ca2"), bytes{}, bytes{}, base16_array("c8f3d70e4199a105321955bc9407901de191ceb3e1ac22fbf1938c5a94b36fe6"), base16_array("327531167001a9ace57b2814c1b705bfcbdf9317da5316f82f120f1414a15f8d") } },
    { base16_array("e0294c8bc1a36b4166ee92bfa70a5c34976fa9829405efea8f9cd54dcb29b99e"), base16_array("ae9690d13b8d20a0fbbf37bed8474f67a04e142f56efd78770a76b359165d8a1"), 0x44, { bytes{}, bytes{}, base16_array("dcd45d935613916af167b029058ba3a700d37150b9df34728cb05412c16d4182"), bytes{}, bytes{}, bytes{}, base16_array("232ba26ca9ec6e950e984fd6fa745c58ff2c8eaf4620cb8d734fabec3e92baad"), bytes{} } },
    { base16_array("e148441cd7b92b8b0e4fa3bd68712cfd0d709ad198cace611493c10e97f5394e"), base16_array("164a639794d74c53afc4d3294e79cdb3cd25f99f6df45c000f758aba54d699c0"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("e4b00ec97aadcca97644d3b0c8a931b14ce7bcf7bc8779546d6e35aa5937381c"), base16_array("94e9588d41647b3fcc772dc8d83c67ce3be003538517c834103d2cd49d62ef4d"), 0xff, { base16_array("c88d25f41407376bb2c03a7fffeb3ec7811cc43491a0c3aac0378cdc78357bee"), base16_array("51c02636ce00c2345ecd89adb6089fe4d5e18ac924e3145e6669501cd37a00d4"), base16_array("205b3512db40521cb200952e67b46f67e09e7839e0de44004138329ebd9138c5"), base16_array("58aab390ab6fb55c1d1b80897a207ce94a78fa5b4aa61a33398bcae9adb20d3e"), base16_array("3772da0bebf8c8944d3fc5800014c1387ee33bcb6e5f3c553fc8732287ca8041"), base16_array("ae3fd9c931ff3dcba132765249f7601b2a1e7536db1ceba19996afe22c85fb5b"), base16_array("dfa4caed24bfade34dff6ad1984b90981f6187c61f21bbffbec7cd60426ec36a"), base16_array("a7554c6f54904aa3e2e47f7685df8316b58705a4b559e5ccc6743515524deef1") } },
    { base16_array("e5bbb9ef360d0a501618f0067d36dceb75f5be9a620232aa9fd5139d0863fde5"), base16_array("e5bbb9ef360d0a501618f0067d36dceb75f5be9a620232aa9fd5139d0863fde5"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
    { base16_array("e6bcb5c3d63467d490bfa54fbbc6092a7248c25e11b248dc2964a6e15edb1457"), base16_array("19434a3c29cb982b6f405ab04439f6d58db73da1ee4db723d69b591da124e7d8"), 0xff, { base16_array("67119877832ab8f459a821656d8261f544a553b89ae4f25c52a97134b70f3426"), base16_array("ffee02f5e649c07f0560eff1867ec7b32d0e595e9b1c0ea6e2a4fc70c97cd71f"), base16_array("b5e0c189eb5b4bacd025b7444d74178be8d5246cfa4a9a207964a057ee969992"), base16_array("5746e4591bf7f4c3044609ea372e908603975d279fdef8349f0b08d32f07619d"), base16_array("98ee67887cd5470ba657de9a927d9e0abb5aac47651b0da3ad568eca48f0c809"), base16_array("0011fd0a19b63f80fa9f100e7981384cd2f1a6a164e3f1591d5b038e36832510"), base16_array("4a1f3e7614a4b4532fda48bbb28be874172adb9305b565df869b5fa71169629d"), base16_array("a8b91ba6e4080b3cfbb9f615c8d16f79fc68a2d8602107cb60f4f72bd0f89a92") } },
    { base16_array("f28fba64af766845eb2f4302456e2b9f8d80affe57e7aae42738d7cddb1c2ce6"), base16_array("f28fba64af766845eb2f4302456e2b9f8d80affe57e7aae42738d7cddb1c2ce6"), 0x33, { base16_array("4f867ad8bb3d840409d26b67307e62100153273f72fa4b7484becfa14ebe7408"), base16_array("5bbc4f59e452cc5f22a99144b10ce8989a89a995ec3cea1c91ae10e8f721bb5d"), bytes{}, bytes{}, base16_array("b079852744c27bfbf62d9498cf819deffeacd8c08d05b48b7b41305db1418827"), base16_array("a443b0a61bad33a0dd566ebb4ef317676576566a13c315e36e51ef1608de40d2"), bytes{}, bytes{} } },
    { base16_array("f455605bc85bf48e3a908c31023faf98381504c6c6d3aeb9ede55f8dd528924d"), base16_array("d31fbcd5cdb798f6c00db6692f8fe8967fa9c79dd10958f4a194f01374905e99"), 0xcc, { bytes{}, bytes{}, base16_array("0c00c5715b56fe632d814ad8a77f8e66628ea47a6116834f8c1218f3a03cbd50"), base16_array("df88e44fac84fa52df4d59f48819f18f6a8cd4151d162afaf773166f57c7ff46"), bytes{}, bytes{}, base16_array("f3ff3a8ea4a9019cd27eb527588071999d715b859ee97cb073ede70b5fc33edf"), base16_array("20771bb0537b05ad20b2a60b77e60e7095732beae2e9d505088ce98fa837fce9") } },
    { base16_array("f58cd4d9830bad322699035e8246007d4be27e19b6f53621317b4f309b3daa9d"), base16_array("78ec2b3dc0948de560148bbc7c6dc9633ad5df70a5a5750cbed721804f082a3b"), 0xff, { base16_array("6c4c580b76c7594043569f9dae16dc2801c16a1fbe12860881b75f8ef929bce5"), base16_array("94231355e7385c5f25ca436aa64191471aea4393d6e86ab7a35fe2afacaefd0d"), base16_array("dff2a1951ada6db574df834048149da3397a75b829abf58c7e69db1b41ac0989"), base16_array("a52b66d3c907035548028bf804711bf422aba95f1a666fc86f4648e05f29caae"), base16_array("93b3a7f48938a6bfbca9606251e923d7fe3e95e041ed79f77e48a07006d63f4a"), base16_array("6bdcecaa18c7a3a0da35bc9559be6eb8e515bc6c291795485ca01d4f5350ff22"), base16_array("200d5e6ae525924a8b207cbfb7eb625cc6858a47d6540a73819624e3be53f2a6"), base16_array("5ad4992c36f8fcaab7fd7407fb8ee40bdd5456a0e599903790b9b71ea0d63181") } },
    { base16_array("fd7d912a40f182a3588800d69ebfb5048766da206fd7ebc8d2436c81cbef6421"), base16_array("8d37c862054debe731694536ff46b273ec122b35a9bf1445ac3c4ff9f262c952"), 0x00, { bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{}, bytes{} } },
};

BOOST_AUTO_TEST_CASE(secp256k1_bip324__swift_inverse__vectors__expected_t)
{
    for (size_t index{}; index < inverse_vectors.size(); ++index)
    {
        const auto& vector = inverse_vectors[index];
        field u{}, x{};
        BOOST_REQUIRE_MESSAGE(accessor::from_bytes(u, vector.u), index);
        BOOST_REQUIRE_MESSAGE(accessor::from_bytes(x, vector.x), index);

        for (uint8_t branch{}; branch < 8u; ++branch)
        {
            field t{};
            const auto valid = get_right(vector.valid, branch);
            BOOST_REQUIRE_MESSAGE(accessor::swift_inverse(t, x, u, branch) == valid, (index << 8 | branch));
            if (valid)
            {
                bytes out{};
                accessor::to_bytes(out, t);
                BOOST_REQUIRE_MESSAGE(out == vector.t[branch], (index << 8 | branch));
            }
        }
    }
}

// bitcoin/bips bip-0324/packet_encoding_test_vectors.csv (secp256k1 columns)

struct packet_vector
{
    ec_secret secret;
    ec_ellswift ours;
    ec_ellswift theirs;
    bool initiating;
    data_array<ec_xonly_size> x_ours;
    data_array<ec_xonly_size> x_theirs;
    hash_digest shared;
};

const std::vector<packet_vector> packet_vectors
{
    { base16_array("61062ea5071d800bbfd59e2e8b53d47d194b095ae5a4df04936b49772ef0d4d7"), base16_array("ec0adff257bbfe500c188c80b4fdd640f6b45a482bbc15fc7cef5931deff0aa186f6eb9bba7b85dc4dcc28b28722de1e3d9108b985e2967045668f66098e475b"), base16_array("a4a94dfce69b4a2a0a099313d10f9f7e7d649d60501c9e1d274c300e0d89aafaffffffffffffffffffffffffffffffffffffffffffffffffffffffff8faf88d5"), true, base16_array("19e965bc20fc40614e33f2f82d4eeff81b5e7516b12a5c6c0d6053527eba0923"), base16_array("0c71defa3fafd74cb835102acd81490963f6b72d889495e06561375bd65f6ffc"), base16_array("c6992a117f5edbea70c3f511d32d26b9798be4b81a62eaee1a5acaa8459a3592") },
    { base16_array("6f312890ec83bbb26798abaadd574684a53e74ccef7953b790fcc29409080246"), base16_array("a8785af31c029efc82fa9fc677d7118031358d7c6a25b5779a9b900e5ccd94aac97eb36a3c5dbcdb2ca5843cc4c2fe0aaa46d10eb3d233a81c3dde476da00eef"), base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f0000000000000000000000000000000000000000000000000000000000000000"), false, base16_array("d4b65faa965b31fe2d9faaeb806c6449a50fe3679555c3518f7a0885f572457f"), base16_array("edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c"), base16_array("a6f79eb08243b6f65dbe42bfe4a6cf3f131d6963fa5d06c770a18f7b9c489b78") },
    { base16_array("846a784f1a03dea59cc679754a60a7145542fa130e3efbd815c81e909ce32933"), base16_array("480eacf1536b52257bf8ce78d8f4ce09395d744767c6c129e7838947ee625af3245592c111275e877d5baae22584cb5f1153e67c16bcd7da767726cd0d0c846a"), base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff22d5e441524d571a52b3def126189d3f416890a99d4da6ede2b0cde1760ce2c3f98457ae"), true, base16_array("014e5bdbb1d7eb34a88a016ab3dd45e343dc703fafa8266907ab67a76c5eb2d6"), base16_array("568146140669e69646a6ffeb3793e8010e2732209b4c34ec13e209a070109183"), base16_array("e500c670f1b32f60e05009bddcdbfa7153afb19c20479583a54b43d85b3433a8") },
    { base16_array("c0f15820459f64d98e5c48681d13340572c574533dd9f7161b85fcc8224fdf30"), base16_array("682871104d694baca8b9c7990ae6288f49e1ff4feb21dd5cffad67db7752fdfb6c3608d6996c54be04b35feef037da09ee4d9dca2363b343bc2d4f6d0ea609da"), base16_array("56bd0c06f10352c3a1a9f4b4c92f6fa2b26df124b57878353c1fc691c51abea77c8817daeeb9fa546b77c8daf79d89b22b0e1b87574ece42371f00237aa9d83a"), false, base16_array("5d673dd0a75ccacf4e1310e9402ecdacdd474d8bbfa6eeefdde2e1b216d41dbe"), base16_array("2dd7b9cc85524f8670f695c3143ac26b45cebcabb2782a85e0fe15aee3956535"), base16_array("b764f617cf8c8dcf6018e4f5e8ee603a086498a3732621c9b0fc0a485ea0d2f0") },
    { base16_array("96cb391886681d1d3e23948e51987771a8ec3001b640c18fb994a855cea66b6e"), base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffdde3a077a6fd73711a27250c439ba78ef63d89cd0918c0a0a75f301ed96aa2a43ecf3f61"), base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffa7730be30000000000000000000000000000000000000000000000000000000000000000"), true, base16_array("f7561c791f6f4aa73dcef3cac32f2433b4cfa4ab0666e93552b7cbc7249fb2de"), base16_array("5232c4b6bde9d3d45d7b763ebd7495399bb825cc21de51011761cd81a51bdc84"), base16_array("779a18107756169a6b369d043f3ef9a90178c7ab8c8c37b4edcd9b5397e41eca") },
    { base16_array("4a7065c3ddbf84e29b8e20da0da3aaae1f708eae8ad1af4c4c00f46a7cda7b6b"), base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff450012ec3aeecf516f4b374af2e7fbb040e92dc3c0f12eafd00c729a137f4e892e5293c3"), base16_array("9652d78baefc028cd37a6a92625b8b8f85fde1e4c944ad3f20e198bef8c02f19fffffffffffffffffffffffffffffffffffffffffffffffffffffffff2e91870"), false, base16_array("a0ff3dd41ca11036eea75ea08993c938894c7eebca99354ac2e0daa8a1a6b2ca"), base16_array("64c383e0e78ac99476ddff2061683eeefa505e3666673a1371342c3e6c26981d"), base16_array("a993062a328371beecae7e2b05a34355c1cefbad7f855ad48331dcf002972999") },
    { base16_array("0f69aeffeff6172647ee5aa80bfb418ee742f4e9f1a51b463ac7c120d620e37d"), base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffff04df0e67f9753e2cdb066b3b588a0069fde936a312e0d3f31acb335026b7072d8f2ad24c"), base16_array("12a50f3fafea7c1eeada4cf8d33777704b77361453afc83bda91eef349ae044d20126c6200547ea5a6911776c05dee2a7f1a9ba7dfbabbbd273c3ef29ef46e46"), true, base16_array("115b298a52a9362706ddd1e493de09443dd8ac2b0c3e4e5e8b6bb295598db05d"), base16_array("eef379db9bd4b1aa90fc347fad33f7d53083389e22e971036f59f4e29d325ac2"), base16_array("1756deace376ece25da9825fe49f76a9272a89a7b746c83ca2c4016f5a30ead4") },
};

BOOST_AUTO_TEST_CASE(secp256k1_bip324__ellswift_decode__packet_vectors__expected_x)
{
    for (size_t index{}; index < packet_vectors.size(); ++index)
    {
        const auto& vector = packet_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(ellswift::decode(point, vector.ours), index);
        BOOST_REQUIRE_MESSAGE(to_x(point) == vector.x_ours, index);
        BOOST_REQUIRE_MESSAGE(ellswift::decode(point, vector.theirs), index);
        BOOST_REQUIRE_MESSAGE(to_x(point) == vector.x_theirs, index);
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_bip324__secret_to_public__packet_vectors__expected_x)
{
    for (size_t index{}; index < packet_vectors.size(); ++index)
    {
        const auto& vector = packet_vectors[index];
        ec_compressed point{};
        BOOST_REQUIRE_MESSAGE(secret_to_public(point, vector.secret), index);
        BOOST_REQUIRE_MESSAGE(to_x(point) == vector.x_ours, index);
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_bip324__ellswift_exchange__packet_vectors__expected_secret)
{
    for (size_t index{}; index < packet_vectors.size(); ++index)
    {
        const auto& vector = packet_vectors[index];
        hash_digest shared{};
        const auto& key_a = vector.initiating ? vector.ours : vector.theirs;
        const auto& key_b = vector.initiating ? vector.theirs : vector.ours;
        BOOST_REQUIRE_MESSAGE(ellswift::exchange(shared, vector.secret, key_a, key_b, !vector.initiating), index);
        BOOST_REQUIRE_MESSAGE(shared == vector.shared, index);
    }
}

BOOST_AUTO_TEST_SUITE_END()
