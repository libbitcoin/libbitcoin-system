/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_HASH_HPP
#define LIBBITCOIN_SYSTEM_TEST_HASH_HPP

// rmd
// ============================================================================

// test vectors
// homes.esat.kuleuven.be/~bosselae/ripemd160.html

static const auto rmd_1 = to_chunk("");
static const auto rmd_2 = to_chunk("a");
static const auto rmd_3 = to_chunk("abc");
static const auto rmd_4 = to_chunk("message digest");
static const auto rmd_5 = to_chunk("abcdefghijklmnopqrstuvwxyz");
static const auto rmd_6 = to_chunk("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
static const auto rmd_7 = to_chunk("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
static const auto rmd_8 = to_chunk("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
static const auto rmd_9 = data_chunk(1'000'000, to_byte('a'));

// rmd128_tests
// ----------------------------------------------------------------------------

struct rmd128_test
{
    const data_chunk data;
    const half_hash expected;
};

static const std_array<rmd128_test, 9> rmd128_tests
{
    rmd128_test
    { rmd_1, base16_array("cdf26213a150dc3ecb610f18f6b38b46") },
    { rmd_2, base16_array("86be7afa339d0fc7cfc785e72f578d33") },
    { rmd_3, base16_array("c14a12199c66e4ba84636b0f69144c77") },
    { rmd_4, base16_array("9e327b3d6e523062afc1132d7df9d1b8") },
    { rmd_5, base16_array("fd2aa607f71dc8f510714922b371834e") },
    { rmd_6, base16_array("a1aa0689d0fafa2ddc22e88b49133a06") },
    { rmd_7, base16_array("d1e959eb179c911faea4624c60c5c702") },
    { rmd_8, base16_array("3f45ef194732c2dbb2c4a2c769795fa3") },
    { rmd_9, base16_array("4a7f5723f954eba1216c9d8f6320431f") }
};

// rmd160_tests
// ----------------------------------------------------------------------------

struct rmd160_test
{
    const data_chunk data;
    const short_hash expected;
};

static const std_array<rmd160_test, 9> rmd160_tests
{
    rmd160_test
    { rmd_1, base16_array("9c1185a5c5e9fc54612808977ee8f548b2258d31") },
    { rmd_2, base16_array("0bdc9d2d256b3ee9daae347be6f4dc835a467ffe") },
    { rmd_3, base16_array("8eb208f7e05d987a9b044a8e98c6b087f15a0bfc") },
    { rmd_4, base16_array("5d0689ef49d2fae572b881b123a85ffa21595f36") },
    { rmd_5, base16_array("f71c27109c692c1b56bbdceb5b9d2865b3708dbc") },
    { rmd_6, base16_array("12a053384a9c0c88e405a06c27dcf49ada62eb2b") },
    { rmd_7, base16_array("b0e20b6e3116640286ed3a87a5713079b21f5189") },
    { rmd_8, base16_array("9b752e45573d4b39f4dbd3323cab82bf63326bfb") },
    { rmd_9, base16_array("52783243c1697bdbe16d37f97f68f08325dc1528") }
};

// sha
// ============================================================================

// test vectors
// www.di-mgt.com.au/sha_testvectors.html

const auto sha_1 = to_chunk("abc");
const auto sha_2 = to_chunk("");
const auto sha_3 = to_chunk("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
const auto sha_4 = to_chunk("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");
constexpr auto million = 1'000'000_size;
static const auto sha_5 = data_chunk(million, to_byte('a'));
#if defined(HAVE_X64) && defined(HAVE_SLOW_TESTS)
#define BIG_SHA_TEST
// requires > uint64_t size_t.
constexpr auto alpha2_size = 64_size;
constexpr auto alpha2_count = 16'777'216_size;
constexpr auto long_alpha_size = alpha2_size * alpha2_count;
static const auto alpha2 = to_array<alpha2_size>("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno");
static const std_vector<long_hash> long_alpha(alpha2_count, alpha2);
static const auto long_alpha_data = pointer_cast<uint8_t>(long_alpha.front().data());
static const auto sha_6 = to_chunk(unsafe_array_cast<uint8_t, long_alpha_size>(long_alpha_data));
constexpr auto sha_tests = 6;
#else
constexpr auto sha_tests = 5;
#endif

// sha160_tests
// ----------------------------------------------------------------------------

struct sha160_test
{
    const data_chunk data;
    const short_hash expected;
};

static const std_array<sha160_test, sha_tests> sha160_tests
{
    sha160_test
    { sha_1, base16_array("a9993e364706816aba3e25717850c26c9cd0d89d") },
    { sha_2, base16_array("da39a3ee5e6b4b0d3255bfef95601890afd80709") },
    { sha_3, base16_array("84983e441c3bd26ebaae4aa1f95129e5e54670f1") },
    { sha_4, base16_array("a49b2446a02c645bf419f995b67091253a04a259") },
#if defined(BIG_SHA_TEST)
    { sha_6, base16_array("7789f0c9ef7bfc40d93311143dfbe69e2017f592") },
#endif
    { sha_5, base16_array("34aa973cd4c4daa4f61eeb2bdbad27316534016f") }
};

// sha256_tests
// ----------------------------------------------------------------------------

struct sha256_test
{
    const data_chunk data;
    const hash_digest expected;
};

static const std_array<sha256_test, sha_tests> sha256_tests
{
    sha256_test
    { sha_1,base16_array("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") },
    { sha_2,base16_array("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") },
    { sha_3,base16_array("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1") },
    { sha_4,base16_array("cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1") },
#if defined(BIG_SHA_TEST)
    { sha_6,base16_array("50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e") },
#endif
    { sha_5,base16_array("cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0") }
};

// sha512_tests
// ----------------------------------------------------------------------------

struct sha512_test
{
    const data_chunk data;
    const long_hash expected;
};

static const std_array<sha512_test, sha_tests> sha512_tests
{
    sha512_test
    { sha_1,base16_array("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f") },
    { sha_2,base16_array("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e") },
    { sha_3,base16_array("204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445") },
    { sha_4,base16_array("8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909") },
#if defined(BIG_SHA_TEST)
    { sha_6,base16_array("b47c933421ea2db149ad6e10fce6c7f93d0752380180ffd7f4629a712134831d77be6091b819ed352c2967a2e2d4fa5050723c9630691f1a05a7281dbe6c1086") },
#endif
    { sha_5,base16_array("e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b") }
};

// hmac
// ============================================================================

// test vectors
// datatracker.ietf.org/doc/html/rfc2202 (sha160)
// datatracker.ietf.org/doc/html/rfc4231 (sha256/512)

static const auto hmac_1k = base16_chunk("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
static const auto hmac_1d = to_chunk("Hi There");
static const auto hmac_2k = to_chunk("Jefe");
static const auto hmac_2d = to_chunk("what do ya want for nothing?");
static const auto hmac_3k = base16_chunk("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
static const auto hmac_3d = base16_chunk("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
static const auto hmac_4k = base16_chunk("0102030405060708090a0b0c0d0e0f10111213141516171819");
static const auto hmac_4d = base16_chunk("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd");
static const auto hmac_5k = base16_chunk("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c");
static const auto hmac_5d = to_chunk("Test With Truncation"); // not truncated here.
static const auto hmac_6k = base16_chunk("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
static const auto hmac_6d = to_chunk("Test Using Larger Than Block-Size Key - Hash Key First");
static const auto hmac_7k = base16_chunk("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
static const auto hmac_7d = to_chunk("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.");
constexpr auto hmac_tests = 7;

// hmac_sha160_tests
// ----------------------------------------------------------------------------

struct hmac_sha160_test
{
    const data_chunk key;
    const data_chunk data;
    const short_hash expected;
};

// This is the subset of rfc2202 vectors in common with rfc4231.
static const std_array<hmac_sha160_test, 2> hmac_sha160_tests
{
    hmac_sha160_test
    { hmac_1k, hmac_1d, base16_array("b617318655057264e28bc0b6fb378c8ef146be00") },
    { hmac_2k, hmac_2d, base16_array("effcdf6ae5eb2fa2d27416d5f184df9c259a7c79") }
};

// hmac_sha256_tests
// ----------------------------------------------------------------------------

struct hmac_sha256_test
{
    const data_chunk key;
    const data_chunk data;
    const hash_digest expected;
};

static const std_array<hmac_sha256_test, hmac_tests> hmac_sha256_tests
{
    hmac_sha256_test
    { hmac_1k, hmac_1d, base16_array("b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7") },
    { hmac_2k, hmac_2d, base16_array("5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843") },
    { hmac_3k, hmac_3d, base16_array("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe") },
    { hmac_4k, hmac_4d, base16_array("82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b") },
    { hmac_5k, hmac_5d, base16_array("a3b6167473100ee06e0c796c2955552bfa6f7c0a6a8aef8b93f860aab0cd20c5") },
    { hmac_6k, hmac_6d, base16_array("60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54") },
    { hmac_7k, hmac_7d, base16_array("9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2") }
};

// hmac_sha512_tests
// ----------------------------------------------------------------------------

struct hmac_sha512_test
{
    const data_chunk key;
    const data_chunk data;
    const long_hash expected;
};

static const std_array<hmac_sha512_test, hmac_tests> hmac_sha512_tests
{
    hmac_sha512_test
    { hmac_1k, hmac_1d, base16_array("87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854") },
    { hmac_2k, hmac_2d, base16_array("164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737") },
    { hmac_3k, hmac_3d, base16_array("fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb") },
    { hmac_4k, hmac_4d, base16_array("b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd") },
    { hmac_5k, hmac_5d, base16_array("415fad6271580a531d4179bc891d87a650188707922a4fbb36663a1eb16da008711c5b50ddd0fc235084eb9d3364a1454fb2ef67cd1d29fe6773068ea266e96b") },
    { hmac_6k, hmac_6d, base16_array("80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598") },
    { hmac_7k, hmac_7d, base16_array("e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58") }

};

// pbkd
// ============================================================================

// pbkd_sha160_tests
// ----------------------------------------------------------------------------
// www.ietf.org/rfc/rfc6070.txt

struct pbkd_sha160_test
{
    const std::string passphrase;
    const std::string salt;
    const size_t count;
    const short_hash expected;
};

static const std_array<pbkd_sha160_test, 3> pbkd_sha160_tests
{
    pbkd_sha160_test
    {
        "password",
        "salt",
        2,
        base16_array("ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957")
    },
    {
        "password",
        "salt",
        4096,
        base16_array("4b007901b765489abead49d926f721d065a429c1")
    },
    {
        "password",
        "salt",
        16777216,
        base16_array("eefe3d61cd4da4e4e9945b3d6ba2158c2634e984")
    }
};

// pbkd_sha256_tests
// ----------------------------------------------------------------------------
// www.rfc-editor.org/rfc/rfc7914.txt

struct pbkd_sha256_test
{
    const std::string passphrase;
    const std::string salt;
    const size_t count;
    const long_hash expected;
};

static const std_array<pbkd_sha256_test, 2> pbkd_sha256_tests
{
    pbkd_sha256_test
    {
        "passwd",
        "salt",
        1,
        base16_array("55ac046e56e3089fec1691c22544b605f94185216dde0465e68b9d57c20dacbc49ca9cccf179b645991664b39d77ef317c71b845b1e30bd509112041d3a19783")
    },
    {
        "Password",
        "NaCl",
        80000,
        base16_array("4ddcd8f60b98be21830cee5ef22701f9641a4418d04c0414aeff08876b34ab56a1d425a1225833549adb841b51c9b3176a272bdebba1d078478f62b397f33c8d")
    }
};

// pbkd_sha512_tests
// ----------------------------------------------------------------------------
// github.com/Anti-weakpasswords/PBKDF2-Test-Vectors/releases

struct pbkd_sha512_test
{
    const std::string passphrase;
    const std::string salt;
    const size_t count;
    const long_hash expected;
};

static const std_array<pbkd_sha512_test, 5> pbkd_sha512_tests
{
    pbkd_sha512_test
    {
        "password",
        "salt",
        1,
        base16_array("867f70cf1ade02cff3752599a3a53dc4af34c7a669815ae5d513554e1c8cf252c02d470a285a0501bad999bfe943c08f050235d7d68b1da55e63f73b60a57fce")
    },
    {
        "password",
        "salt",
        2,
        base16_array("e1d9c16aa681708a45f5c7c4e215ceb66e011a2e9f0040713f18aefdb866d53cf76cab2868a39b9f7840edce4fef5a82be67335c77a6068e04112754f27ccf4e")
    },
    {
        "password",
        "salt",
        4096,
        base16_array("d197b1b33db0143e018b12f3d1d1479e6cdebdcc97c5c0f87f6902e072f457b5143f30602641b3d55cd335988cb36b84376060ecd532e039b742a239434af2d5")
    },
    {
        "passwordPASSWORDpassword",
        "saltSALTsaltSALTsaltSALTsaltSALTsalt",
        4096,
        base16_array("8c0511f4c6e597c6ac6315d8f0362e225f3c501495ba23b868c005174dc4ee71115b59f9e60cd9532fa33e0f75aefe30225c583a186cd82bd4daea9724a3d3b8")
    },
    {
        "password",
        "NaCL",
        1,
        base16_array("73decfa58aa2e84f94771a75736bb88bd3c7b38270cfb50cb390ed78b305656af8148e52452b2216b2b8098b761fc6336060a09f76415e9f71ea47f9e9064306")
    }
};

// scrypt_tests
// ============================================================================
// Also rfc7914 tests inline.

struct scrypt_test
{
    const data_array<80> data;
    const hash_digest expected;
};

static const std_array<scrypt_test, 5> scrypt_tests
{
    scrypt_test
    {
        base16_array("020000004c1271c211717198227392b029a64a7971931d351b387bb80db027f270411e398a07046f7d4a08dd815412a8712f874a7ebf0507e3878bd24e20a3b73fd750a667d2f451eac7471b00de6659"),
        base16_array("065898d7ab2daa8235cdda9511d248f3010b5e11f682f80741ef2b0000000000")
    },
    {
        base16_array("0200000011503ee6a855e900c00cfdd98f5f55fffeaee9b6bf55bea9b852d9de2ce35828e204eef76acfd36949ae56d1fbe81c1ac9c0209e6331ad56414f9072506a77f8c6faf551eac7471b00389d01"),
        base16_array("94fc881c9ff1da50d235ed28f2bbcfddfeb7084e63ebd5bd110d3a0000000000")
    },
    {
        base16_array("02000000a72c8a177f523946f42f22c3e86b8023221b4105e8007e59e81f6beb013e29aaf635295cb9ac966213fb56e046dc71df5b3f7f67ceaeab24038e743f883aff1aaafaf551eac7471b0166249b"),
        base16_array("81caa81451ddf8659cf2afd8599d2d6c8a724432e188f295f8400b0000000000")
    },
    {
        base16_array("010000007824bc3a8a1b4628485eee3024abd8626721f7f870f8ad4d2f33a27155167f6a4009d1285049603888fe85a84b6c803a53305a8d497965a5e896e1a00568359589faf551eac7471b0065434e"),
        base16_array("fe05e1971818866adc7e8e6e2fd7e8d8991e032349cd91580007300000000000")
    },
    {
        base16_array("0200000050bfd4e4a307a8cb6ef4aef69abc5c0f2d579648bd80d7733e1ccc3fbc90ed664a7f74006cb11bde87785f229ecd366c2d4e44432832580e0608c579e4cb76f383f7f551eac7471b00c36982"),
        base16_array("8cec00384d72c7e74f5b340d73af02fa47cb0c13c7afa426b4f0180000000000")
    }
};

#endif
