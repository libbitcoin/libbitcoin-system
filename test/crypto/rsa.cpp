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

// Tor chutney 1024 bit key, unprefixed signature of sha1 digest (arti).
const auto tor_key = base16_chunk("30818902818100d527b6c63d6e81d39c328a94ce157dccdc044eb1ad8c210c9c9e22487b4cfade6d4041bd10469a657e3d82bc00cf62ac3b6a99247e573b54c10c47f5dc849b0accda031eca6f6e5dc85677f76dec49ff24d2fcb2b5887fb125aa204744119bb6417f45ee696f8dfc1c2fc21b2bae8e9e37a19dc2518a2c24e7d8fd7fac0f46950203010001");
const auto tor_signature = base16_chunk("8bb056a58e8480f0984104112fcc886dafb60bfb0049558c99c0f9c7f69294655ebb09dae21d794ab38a00c65405e70426d02a483b92cd71a7e853fd59735d8bec5ee464339dbd43df08cf6ab4e7a35cbf918b4989103932b27413571684b979ac8dabce1aaa048873145698bb1c9102191207108a4dd97a5f493be1a853dcc3");
const auto tor_digest = base16_chunk("8c28f494a3ca4522926b177124a51158a790bec0");
const auto tor_wrong_digest = base16_chunk("8c28f494a3ca4522926b177124a51158a790bec1");

// openssl 3.5 keys and "abc" signatures, pss salt of hash size unless named.
const auto key1025 = base16_chunk("308189028181017288c6104e9b27f23be8ef4f9e31bc3104f4fe292ff4fb885d63a7247ff04eea6024f37c370a2934e1b2d69af2b8140b9ab5af9de6b790c5516e097422422e28911b00b0c7d116d9cbaf40ec8397cf710befa34a73c2dc5e7d8d7e4d812a21172714c9127b9b064545e6f0f9b00d839b45c04c9fd48daa9b6799ac9629915a550203010001");
const auto key2048 = base16_chunk("3082010a0282010100a24baa7f8ea18bc621a284dbba56723c91457cba2823d1c1b30182017dbdae9a079f5e41dfba3d68a984084fe534f2d3459c0175404f6a93815b9dde50ef9b4d6a28c678f2b70b0178d1b0bf848c468f3f9240c6f6420cc0354ff2426c792f9d9cc1e174221e6f9154b5774a4beb33d6ef3bdd94e3cf18513a4462509dfb8a8758d90f4a3b443ec6854427584858c37a0b081acbd38ad99db2e4692c5d89549e6a8feed8b7eafe8efd4a61af2c51ba5b7fe8b6ba86c9c0c485aac4374a436b2c02b0d95066c2703de64c115929fb8e1e8ba7f7025e398bab66dded014804ae69764d762035ca7f78e97ee55a7a8e032c23457d31371d484b571d438f74fec28b0203010001");
const auto key3072 = base16_chunk("3082018a0282018100a041a74ace03e2eca82cc3c38a02b79488736a01d09021e1f678d197e16b173105f10b3b422340ca7738160606934666a2ba00f04a6b4b26ad604a4c0fe4d0ff6f4aaba0ce361a6434ab1e10ffeb4fb8ca52a5df008c49a625177dcf1b1be877163f3369c222eac34a3dae3c09d9ce4c87f768b84475abbce799190b40206ca1e44238fdc066bb5210cc9325729c9f5dd1a6ea966a8a4975b4eb6aad03ad1beddd1f166d40b4ae0cfb42ee6fe915a0d0dcfcf3e0364021cee375b99e29ba9206b2f9a57a50bf5cb564911908786f8b752e765f65d34e808b87765b365a4cd2a252f0375c6f4558725191cadf528d4fcb5918931820da6b4477b94158b86a35539512fc8748487d4d4eb8b3c05e8048113287a85910d7a785cc3142015d74b3a2457717710d22c045752265846261f90b55b2d0558ec8975e0881fa69edb2620287163af5765d2585fde1a41cefdc0d66774fb135e3bfc74dc868dfb6cf03f0cd0600f2d74ca884072001a3ecc617158ab00de5ca7a81578dd5c0ceb0e51db2330203010001");
const auto key4096 = base16_chunk("3082020a0282020100d7e5d299297c6a5aadf9eb74bea37a27ca7f97a6598ef03991f660dd4f04bc4ec58d8800da4d852dfebd68df82d2331d2f6beae825fa152c3cdd43ae3415a515d6e6b32246a1b4bd4f8d882241f363e07f8ae5d17291b3be223e2a6e899a70eed9d01817cf3083817acc8d3b7913c452c6dc6e413afb724d0833680fc48b83be02881882833f395f1299a39729acb35581d095ff5393161776e2b8566a8d87bdf9bfd77993994adedd2fe5d7d8f94aaf4f20b6357838a935b6900fa7ac9e740c63d942a0337646619e1eb50b1809434dcfc3f232596af5da4738b269e6be7f6e5353e0b41b6e51768dde09a8f601a7f02c617b55cd118ac59d222d2e2c3be3e97f53b6d76206a575e2a9163858f119c38e0d67fe7ea115d1300cdf7b62fdd6e845eaa541a2a48ab7bfe43e9f9cfed6444034a66478f8d8dcf5d5d18be78221b5df892ac0a408277fc2e68e0851961cd731b41d9cdd7e11b9293ee1a66515e57c39800c6ee01e3b222d909d8526a1e7a271f3bd8b771474666e5ac90afaf3398e242494a3c7d2ac74ec2117e66d41a8dc5c5f35dc277087b8ae4d5a9ceb5c85eb9c9251d201042bf64a579170beaffa3d315fdf26305e77a7184c91a42efa973282948e72c7be23faf120119e2dc99932ce79a2475463ce6bb68d553813d4670474c203101009eb9a29aac730e07c155d958adab8f541d77719117e55d472e0f30203010001");
const auto pss1025_256 = base16_chunk("0159b4bd644ca88bae45ebb6a4601af150b3b280100a8d50467ded161969748207be5d1d7f6af9bb3cc0fb7b5480a6019de1fbcb2c41de6814c7d020b04f698c74f8802f81a04198bacbbc19009f709d0e0180e7cb7f08effd7947c533359af4044aff109cefc5d486da6241934e78eeee98ccc2c2a82a0c2080df2875d1529f2f");
const auto pss2048_256 = base16_chunk("1aa2ec9ef4fe2995e6c6552f5a75bb6189d05353ca6004b33b734fc7ccac8bec9a18c8861d7102e0e7ccbe984a5d81cd231b9d9f4a2fe9d5ddb01826a94778176c528f89eba904e9ce366cd81b5ad53d819e06ed4595668615133ef25b39c78a5e424209986ed7ef5d8dc49f950af40393223188378c3c1044aa1d826131355740e9eea2094cf69c60daa097c579712ad4d30f6d8cffa631e9122eb647b08443bc97acd77bb9f33848abf32e150957f5b62c7fe943dac8f7363b32b3586df417dd36d4a0aff113a4047cdcbf63fed64c816be250462b18cd166e02470f5fa9706650a62d14b0a8ab903290fe173741d340d1c7dd2fabbc999ac20f0628e5c8c8");
const auto pss2048_256_salt0 = base16_chunk("8497a909e3c733539cbef997c541037ca29332a54c11dd02388f1de09c952da33398b4909d77fe120ddf4c18a69e9942a8d5bc5c962b9c9ef7bb5a2b48a1cf52dc07c82cdd4fc5c9818506bef28c74b819410dc5f731525ed241eccaee88347b1d76a72bf001a615fba36b792b8401948500efe58734edd601cdd220cbe09b39c6c231e411202868eefc705fef91fdea2cf17ac342b2d5cfe7080969c0558b0aceae46b72174b6568e57350f2025e1c1690ab1743298fa13389517fe798fbef091700f009fe2e8473c4a9b6021c66a9820aaa747247e5df684970d71c02939b2a293d3842a59cb4ea83f7385e37ee78f4433379a092ab98deb729f2c18053f0e");
const auto pss3072_384 = base16_chunk("3e4cdf0df97f18c9563195ec0a14b09a017afc8ca03e778cb439b884995077069b28f7d31516cba0a37f749370188dc13fbad6286af982747bdbb2e24ee0445095040760d9f41f299d0175d4aead9d3c856b157fede17b3651e1b9a0e2bc2f85efca9740e5acfe6f216108d51a3eea9d44fed18e74662107acfddc9fd4c2c11f98db35f371a0fef08ab553ec792dcdbb36253e047e64b3cb8c91acec260510bcf1bc594bb7098e1860510df01c48a620c7c329fc0958d79313daed7d97e5947ca3a9a880f8b19ae00bc3d912b25daa544d734892e0908bb8a4560b9775ca87c3ce7e49206279a0907c82c08790fd6f0b541f10fe243cbcd831850d7284fa4e11b015ac650b4dfb4eedfccae63d195b5ed9fa7ee91541487b12c51b2b932979053cadedb1cd16d52fd6c57debd6aa88c4fde869f70106e8c5707b82a25f026b078c7caf44ec41327ec3c5564d6d991dd83e531a229e1e6ca0194da50ecd13e774dadccbdb5cfdd51f296aca0b52b873344b12411469b1d085bf79054b35912bd0");
const auto pss4096_512 = base16_chunk("9e55cbec0be0fb3ea30479c9ffe5dc6d50d11fdc035bad6da95ac8f052e2c03a80988a79616a3bcca2518f551d8bb9ff8249535913dadbde0694040bf0dc7ec0a3e1eb33b257cc1e0dbb1fea0dd041fb74c51168813dd094f58b4209f89117456daf1ccd7931d664cb8c962ebbdd20d2bfc679d9887623e3648f2489d2921416d99195825aa9854910996d572ac5cf968f1c6a21bc12104ec4aaaa19675c4385cec69b7e6d129fdb3aff9127745ee06fe4632b1016fae3ed8d9090e4b0ba82af72f03910e15f436fffeb71e21bb2741c67889950ba49586a81a5bbacb3a2e6a7c074fe8b1088b96cb20b996ade22aef50262a736a51f30e8e1d10c7d4077105f5937ed429a785bb86932ab80a8cadacf21d8d22f1fe4100e0a5bcc012cd8418837d120320e2c29b08a712abb6f4efae1cd30ef1d724ffb3c7a7e57f9b3fc5acdb3e6c47329b1b74bf51a2abb3d9da43701d8dac25e89d6a5cd2ca2957dbb0dbb16d9aa0d0a88573f674b959d1b0f4f3eec4eea7dc832668c220880628eff1cea80b50299204f0f9b230a78399cefe3b95cf01a2fed2012143b0cc6f865f81fdc22a8283a45bc750b1b44a6d503ff53e193918faf7179de99c06e323ed05f62af30eb40a5c5b3d6d12475687bb4f59615d58a166bfbe8b51691ef2602979de8dae4f096b9b8bfa68dc1f494a07725ec2e7cdf41817a20ca22ed3d021582f5e4e2");
const auto pkcs2048_256 = base16_chunk("531ce2f22969b293ef66c9b62899650c7ae47f271a93019a202ce4597e2470ad519922707db4b423bfb515608011837d0269bbebaafbd2fed1a88e451085d58a2a20ee1e17b8b10a659076d1337316ceaa096f0341b0b1f83a65dd81ecb0e592fdf2d49774fde270b611099b88237915dab8b55f24657f9a0eb0c11391068944ed4168ba599b6ec6ec6ee9ea557aa70e23e424b2f186907dd12e55e6e4efefa6f4af016af9e050cef2d6c06d702d933b58caf5e334f619c7d24ea15bf4ea404432eef6d7827c7b6a52a275f1db2aa6ac2653ef7548131813a2fbff6c740c194f4e64ab68b8296257b13340f53225f28d325335574d03af521bf6933e2d13ffdd");
const auto abc_sha256 = base16_chunk("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
const auto abc_sha384 = base16_chunk("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7");
const auto abc_sha512 = base16_chunk("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
const auto sha256_info = base16_chunk("3031300d060960864801650304020105000420");

// A 4097 bit modulus.
const auto key_oversize = base16_chunk("3082020a0282020101ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0203010001");

BC_AUTO_TEST_SUITE(rsa_tests)

// decode

BC_AUTO_TEST_CASE(rsa__decode__tor_key__expected)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, tor_key));
    BC_REQUIRE_EQUAL(key.modulus.size(), 128_size);
    BC_REQUIRE_EQUAL(key.exponent, base16_chunk("010001"));
}

BC_AUTO_TEST_CASE(rsa__decode__maximum_size__expected)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key4096));
    BC_REQUIRE_EQUAL(key.modulus.size(), rsa::maximum_size);
}

BC_AUTO_TEST_CASE(rsa__decode__oversize__false)
{
    rsa::public_key key{};
    BC_REQUIRE(!rsa::decode(key, key_oversize));
}

BC_AUTO_TEST_CASE(rsa__decode__trailing_byte__false)
{
    const auto der = splice(tor_key, base16_chunk("00"));
    rsa::public_key key{};
    BC_REQUIRE(!rsa::decode(key, der));
}

BC_AUTO_TEST_CASE(rsa__decode__empty__false)
{
    rsa::public_key key{};
    BC_REQUIRE(!rsa::decode(key, {}));
}

// verify_pkcs1

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__tor_digest__true)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, tor_key));
    BC_REQUIRE(rsa::verify_pkcs1(key, tor_signature, tor_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__tor_wrong_digest__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, tor_key));
    BC_REQUIRE(!rsa::verify_pkcs1(key, tor_signature, tor_wrong_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__signature_of_modulus__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, tor_key));
    BC_REQUIRE(!rsa::verify_pkcs1(key, key.modulus, tor_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__short_signature__false)
{
    const data_chunk signature{ std::next(tor_signature.begin()), tor_signature.end() };
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, tor_key));
    BC_REQUIRE(!rsa::verify_pkcs1(key, signature, tor_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__digest_info__true)
{
    const auto payload = splice(sha256_info, abc_sha256);
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(rsa::verify_pkcs1(key, pkcs2048_256, payload));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__missing_digest_info__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(!rsa::verify_pkcs1(key, pkcs2048_256, abc_sha256));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__even_modulus__false)
{
    rsa::public_key key{};
    const auto decoded = rsa::decode(key, tor_key);
    key.modulus.back() ^= 0x01;
    BC_REQUIRE(decoded);
    BC_REQUIRE(!rsa::verify_pkcs1(key, tor_signature, tor_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pkcs1__unit_exponent__false)
{
    rsa::public_key key{};
    const auto decoded = rsa::decode(key, tor_key);
    key.exponent = base16_chunk("01");
    BC_REQUIRE(decoded);
    BC_REQUIRE(!rsa::verify_pkcs1(key, tor_signature, tor_digest));
}

// verify_pss

BC_AUTO_TEST_CASE(rsa__verify_pss__sha256_1025_bits__true)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key1025));
    BC_REQUIRE(rsa::verify_pss(key, pss1025_256, abc_sha256));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__sha256_2048_bits__true)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(rsa::verify_pss(key, pss2048_256, abc_sha256));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__sha384_3072_bits__true)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key3072));
    BC_REQUIRE(rsa::verify_pss(key, pss3072_384, abc_sha384));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__sha512_4096_bits__true)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key4096));
    BC_REQUIRE(rsa::verify_pss(key, pss4096_512, abc_sha512));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__zero_salt__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(!rsa::verify_pss(key, pss2048_256_salt0, abc_sha256));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__altered_signature__false)
{
    auto signature = pss2048_256;
    signature.back() ^= 0x01;
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(!rsa::verify_pss(key, signature, abc_sha256));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__other_hash__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key3072));
    BC_REQUIRE(!rsa::verify_pss(key, pss3072_384, abc_sha512));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__unsupported_digest__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(!rsa::verify_pss(key, pss2048_256, tor_digest));
}

BC_AUTO_TEST_CASE(rsa__verify_pss__pkcs1_signature__false)
{
    rsa::public_key key{};
    BC_REQUIRE(rsa::decode(key, key2048));
    BC_REQUIRE(!rsa::verify_pss(key, pkcs2048_256, abc_sha256));
}

BC_AUTO_TEST_SUITE_END()
