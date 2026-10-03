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

BOOST_AUTO_TEST_SUITE(secp256k1_algorithm_verify_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    template <typename Word>
    using field_t = algorithm::field_t<Word>;
    template <typename Word>
    using affine_t = algorithm::affine_t<Word>;
    using bytes_t = algorithm::bytes_t;
    using algorithm::generator;
    using algorithm::from_bytes;
    using algorithm::verify_ecdsa;
    using algorithm::verify_schnorr;
};

using field = accessor::field_t<uint64_t>;
using affine = accessor::affine_t<uint64_t>;
using bytes = accessor::bytes_t;

// vectors
// ----------------------------------------------------------------------------

constexpr auto zero_value = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto order_value = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
constexpr auto prime_minus_order = base16_array("000000000000000000000000000000014551231950b75fc4402da1722fc9baee");

// Public keys of G.
constexpr ec_uncompressed hybrid_even =base16_array("0679be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
constexpr ec_uncompressed hybrid_odd = base16_array("0779be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
constexpr ec_uncompressed off_curve = base16_array("0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b9");
constexpr ec_compressed bad_sign = base16_array("0579be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr ec_compressed prime_x = base16_array("02fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
constexpr ec_compressed zero_x = base16_array("020000000000000000000000000000000000000000000000000000000000000000");

// ECDSA (secp256k1 wrapper tests, sighash in hash encoding).
constexpr ec_compressed key2 = base16_array("03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b");
constexpr auto sighash2 = base16_hash("ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f");
constexpr auto r2 = base16_array("bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cda33a624378");
constexpr auto s2 = base16_array("6b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f6b8bb67d");
constexpr auto s2_negated = base16_array("946c122950839a624bd3a8614cb5c5b204a2b431088c193e6d6b8a5d64aa8ac4");

// ECDSA with x(R) = r + n.
constexpr ec_compressed overflow_key = base16_array("02af4cb1801bad101fb1f320b93cb1c7a9fe106baadbe8ab3f6ec92c0de9bd56bb");
constexpr auto overflow_hash = base16_array("1111111111111111111111111111111111111111111111111111111111111111");
constexpr auto overflow_r = base16_array("0000000000000000000000000000000000000000000000000000000000000002");
constexpr auto overflow_s = base16_array("2222222222222222222222222222222222222222222222222222222222222222");
const auto overflow_der = base16_chunk("302502010202202222222222222222222222222222222222222222222222222222222222222222");

// ECDSA by key G with r = s = z = x(2G), so that u1 = u2 = 1 and G adds to G.
constexpr auto doubled = base16_array("c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5");

// ECDSA and Schnorr signing keys.
constexpr ec_secret secret1 = base16_array("8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
constexpr ec_secret secret2 = base16_array("33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce");
constexpr ec_secret secret3 = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
constexpr ec_secret secret4 = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
constexpr auto message1 = base16_array("f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0");
constexpr auto message2 = base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
constexpr auto message3 = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto message4 = base16_array("243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c89");

// BIP340 test vectors.
constexpr auto bip340_key0 = base16_array("f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9");
constexpr auto bip340_key1 = base16_array("dff1d77f2a671c5f36183726db2341be58feae1da2deced843240f7b502ba659");
constexpr auto bip340_key2 = base16_array("dd308afec5777e13121fa72b9cc1b7cc0139715309b086c960e18fd969774eb8");
constexpr auto bip340_key3 = base16_array("25d1dff95105f5253c4022f628a996ad3a0d95fbf21d468a1b33f8c160d8f517");
constexpr auto bip340_key4 = base16_array("d69c3509bb99e412e68b0fe8544e72837dfa30746d8be2aa65975f29d22dc7b9");
constexpr auto bip340_key5 = base16_array("eefdea4cdb677750a420fee807eacf21eb9898ae79b9768766e4faa04a2d4a34");
constexpr auto bip340_key14 = base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc30");
constexpr auto bip340_key15 = base16_array("778caa53b4393ac467774d09497a87224bf9fab6f6e68b23086497324d6fd117");
const auto bip340_message0 = base16_chunk("0000000000000000000000000000000000000000000000000000000000000000");
const auto bip340_message1 = base16_chunk("243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c89");
const auto bip340_message2 = base16_chunk("7e2d58d8b3bcdf1abadec7829054f90dda9805aab56c77333024b9d0a508b75c");
const auto bip340_message3 = base16_chunk("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
const auto bip340_message4 = base16_chunk("4df3c3f68fcc83b27e9d42c90431a72499f17875c81a599b566c9889b9696703");
const auto bip340_message15 = data_chunk{};
const auto bip340_message16 = base16_chunk("11");
const auto bip340_message17 = base16_chunk("0102030405060708090a0b0c0d0e0f1011");
const auto bip340_message18 = base16_chunk("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999");
constexpr ec_signature bip340_signature0 = base16_array("e907831f80848d1069a5371b402410364bdf1c5f8307b0084c55f1ce2dca821525f66a4a85ea8b71e482a74f382d2ce5ebeee8fdb2172f477df4900d310536c0");
constexpr ec_signature bip340_signature1 = base16_array("6896bd60eeae296db48a229ff71dfe071bde413e6d43f917dc8dcf8c78de33418906d11ac976abccb20b091292bff4ea897efcb639ea871cfa95f6de339e4b0a");
constexpr ec_signature bip340_signature2 = base16_array("5831aaeed7b44bb74e5eab94ba9d4294c49bcf2a60728d8b4c200f50dd313c1bab745879a5ad954a72c45a91c3a51d3c7adea98d82f8481e0e1e03674a6f3fb7");
constexpr ec_signature bip340_signature3 = base16_array("7eb0509757e246f19449885651611cb965ecc1a187dd51b64fda1edc9637d5ec97582b9cb13db3933705b32ba982af5af25fd78881ebb32771fc5922efc66ea3");
constexpr ec_signature bip340_signature4 = base16_array("00000000000000000000003b78ce563f89a0ed9414f5aa28ad0d96d6795f9c6376afb1548af603b3eb45c9f8207dee1060cb71c04e80f593060b07d28308d7f4");
constexpr ec_signature bip340_signature5 = base16_array("6cff5c3ba86c69ea4b7376f31a9bcb4f74c1976089b2d9963da2e5543e17776969e89b4c5564d00349106b8497785dd7d1d713a8ae82b32fa79d5f7fc407d39b");
constexpr ec_signature bip340_signature6 = base16_array("fff97bd5755eeea420453a14355235d382f6472f8568a18b2f057a14602975563cc27944640ac607cd107ae10923d9ef7a73c643e166be5ebeafa34b1ac553e2");
constexpr ec_signature bip340_signature7 = base16_array("1fa62e331edbc21c394792d2ab1100a7b432b013df3f6ff4f99fcb33e0e1515f28890b3edb6e7189b630448b515ce4f8622a954cfe545735aaea5134fccdb2bd");
constexpr ec_signature bip340_signature8 = base16_array("6cff5c3ba86c69ea4b7376f31a9bcb4f74c1976089b2d9963da2e5543e177769961764b3aa9b2ffcb6ef947b6887a226e8d7c93e00c5ed0c1834ff0d0c2e6da6");
constexpr ec_signature bip340_signature9 = base16_array("0000000000000000000000000000000000000000000000000000000000000000123dda8328af9c23a94c1feecfd123ba4fb73476f0d594dcb65c6425bd186051");
constexpr ec_signature bip340_signature10 = base16_array("00000000000000000000000000000000000000000000000000000000000000017615fbaf5ae28864013c099742deadb4dba87f11ac6754f93780d5a1837cf197");
constexpr ec_signature bip340_signature11 = base16_array("4a298dacae57395a15d0795ddbfd1dcb564da82b0f269bc70a74f8220429ba1d69e89b4c5564d00349106b8497785dd7d1d713a8ae82b32fa79d5f7fc407d39b");
constexpr ec_signature bip340_signature12 = base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f69e89b4c5564d00349106b8497785dd7d1d713a8ae82b32fa79d5f7fc407d39b");
constexpr ec_signature bip340_signature13 = base16_array("6cff5c3ba86c69ea4b7376f31a9bcb4f74c1976089b2d9963da2e5543e177769fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
constexpr ec_signature bip340_signature14 = base16_array("6cff5c3ba86c69ea4b7376f31a9bcb4f74c1976089b2d9963da2e5543e17776969e89b4c5564d00349106b8497785dd7d1d713a8ae82b32fa79d5f7fc407d39b");
constexpr ec_signature bip340_signature15 = base16_array("71535db165ecd9fbbc046e5ffaea61186bb6ad436732fccc25291a55895464cf6069ce26bf03466228f19a3a62db8a649f2d560fac652827d1af0574e427ab63");
constexpr ec_signature bip340_signature16 = base16_array("08a20a0afef64124649232e0693c583ab1b9934ae63b4c3511f3ae1134c6a303ea3173bfea6683bd101fa5aa5dbc1996fe7cacfc5a577d33ec14564cec2bacbf");
constexpr ec_signature bip340_signature17 = base16_array("5130f39a4059b43bc7cac09a19ece52b5d8699d1a71e3c52da9afdb6b50ac370c4a482b77bf960f8681540e25b6771ece1e5a37fd80e5a51897c5566a97ea5a5");
constexpr ec_signature bip340_signature18 = base16_array("403b12b0d8555a344175ea7ec746566303321e5dbfa8be6f091635163eca79a8585ed3e3170807e7c03b720fc54c7b23897fcba0e9d0b4a06894cfd249f22367");

// helpers
// ----------------------------------------------------------------------------

constexpr bool is_key(const ec_compressed& key) NOEXCEPT
{
    affine out{};
    return accessor::from_bytes(out, key);
}

constexpr bool is_key(const ec_uncompressed& key) NOEXCEPT
{
    affine out{};
    return accessor::from_bytes(out, key);
}

constexpr bool is_generator(const ec_compressed& key) NOEXCEPT
{
    affine out{};
    return accessor::from_bytes(out, key) &&
        out.x == accessor::generator.x && out.y == accessor::generator.y;
}

constexpr bool is_generator(const ec_uncompressed& key) NOEXCEPT
{
    affine out{};
    return accessor::from_bytes(out, key) &&
        out.x == accessor::generator.x && out.y == accessor::generator.y;
}

template <size_t Bits = zero>
static bool ecdsa_verify(const ec_compressed& key, const hash_digest& hash,
    const bytes& r, const bytes& s) NOEXCEPT
{
    affine point{};
    return accessor::from_bytes(point, key) &&
        accessor::verify_ecdsa<Bits>(point, hash, r, s);
}

// Local verification of a signature by the linked implementation.
static bool ecdsa_signed(const ec_secret& secret, const hash_digest& hash,
    bool mutate) NOEXCEPT
{
    ec_compressed key{};
    ec_signature signature{}, canonical{};
    if (!secret_to_public(key, secret) ||
        !ecdsa::sign(signature, secret, hash) ||
        !ecdsa::canonicalize_signature(canonical, signature))
        return false;

    auto message = hash;
    if (mutate)
        message[0] ^= 1;

    const auto& r = array_cast<uint8_t, ec_secret_size>(canonical);
    const auto& s = array_cast<uint8_t, ec_secret_size, ec_secret_size>(canonical);
    const auto valid = ecdsa_verify(key, message, r, s);
    return valid == ecdsa::verify_signature(key, message, signature) &&
        valid == !mutate;
}

// BIP340 challenge hash of r, x-only key, and message.
static hash_digest challenge(const ec_signature& signature, const bytes& key,
    const data_slice& message) NOEXCEPT
{
    accumulator<sha256> context{ tagged_midstate<"BIP0340/challenge">, one };
    context.write(array_cast<uint8_t, ec_secret_size>(signature));
    context.write(key);
    context.write(message.size(), message.data());
    return context.flush();
}

template <size_t Bits = zero>
static bool schnorr_verify(const bytes& key, const data_slice& message,
    const ec_signature& signature) NOEXCEPT
{
    const auto& r = array_cast<uint8_t, ec_secret_size>(signature);
    const auto& s = array_cast<uint8_t, ec_secret_size, ec_secret_size>(signature);
    return accessor::verify_schnorr<Bits>(key, challenge(signature, key, message), r, s);
}

// Local verification of a signature by the linked implementation.
static bool schnorr_signed(const ec_secret& secret, const hash_digest& hash,
    bool mutate) NOEXCEPT
{
    ec_compressed point{};
    ec_signature signature{};
    if (!secret_to_public(point, secret) ||
        !schnorr::sign(signature, secret, hash, hash))
        return false;

    auto message = hash;
    if (mutate)
        message[0] ^= 1;

    const auto& key = array_cast<uint8_t, ec_xonly_size, one>(point);
    const auto valid = schnorr_verify(key, message, signature);
    return valid == schnorr::verify_signature(key, message, signature) &&
        valid == !mutate;
}

// keys
// ----------------------------------------------------------------------------

static_assert(is_generator(ec_compressed_generator));
static_assert(is_generator(ec_uncompressed_generator));
static_assert(is_generator(hybrid_even));
static_assert(!is_key(hybrid_odd));
static_assert(!is_key(off_curve));
static_assert(!is_key(bad_sign));
static_assert(!is_key(prime_x));
static_assert(!is_key(zero_x));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__from_bytes__keys__expected)
{
    BOOST_CHECK(is_generator(ec_compressed_generator));
    BOOST_CHECK(is_generator(ec_uncompressed_generator));
    BOOST_CHECK(is_generator(hybrid_even));
    BOOST_CHECK(is_key(key2));
    BOOST_CHECK(is_key(overflow_key));
    BOOST_CHECK(!is_key(hybrid_odd));
    BOOST_CHECK(!is_key(off_curve));
    BOOST_CHECK(!is_key(bad_sign));
    BOOST_CHECK(!is_key(prime_x));
    BOOST_CHECK(!is_key(zero_x));
}

// ecdsa
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__vector__expected)
{
    BOOST_CHECK(ecdsa_verify(key2, sighash2, r2, s2));
    BOOST_CHECK(ecdsa_verify(key2, sighash2, r2, s2_negated));
    BOOST_CHECK(!ecdsa_verify(key2, overflow_hash, r2, s2));
    BOOST_CHECK(!ecdsa_verify(overflow_key, sighash2, r2, s2));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__exceptional__expected)
{
    BOOST_CHECK(ecdsa_verify(ec_compressed_generator, doubled, doubled, doubled));
    BOOST_CHECK(!ecdsa_verify(ec_compressed_generator, sighash2, doubled, doubled));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__out_of_range__false)
{
    BOOST_CHECK(!ecdsa_verify(key2, sighash2, zero_value, s2));
    BOOST_CHECK(!ecdsa_verify(key2, sighash2, r2, zero_value));
    BOOST_CHECK(!ecdsa_verify(key2, sighash2, order_value, s2));
    BOOST_CHECK(!ecdsa_verify(key2, sighash2, r2, order_value));
    BOOST_CHECK(!ecdsa_verify(key2, sighash2, prime_minus_order, s2));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__overflow__expected)
{
    ec_signature signature{};
    BOOST_REQUIRE(ecdsa::decode_signature(signature, overflow_der, true));
    BOOST_CHECK(ecdsa::verify_signature(overflow_key, overflow_hash, signature));
    BOOST_CHECK(ecdsa_verify(overflow_key, overflow_hash, overflow_r, overflow_s));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__windows__expected)
{
    BOOST_CHECK(ecdsa_verify<4>(key2, sighash2, r2, s2));
    BOOST_CHECK(ecdsa_verify<4>(key2, sighash2, r2, s2_negated));
    BOOST_CHECK(!ecdsa_verify<4>(key2, overflow_hash, r2, s2));
    BOOST_CHECK(!ecdsa_verify<4>(overflow_key, sighash2, r2, s2));
    BOOST_CHECK(ecdsa_verify<4>(ec_compressed_generator, doubled, doubled, doubled));
    BOOST_CHECK(!ecdsa_verify<4>(ec_compressed_generator, sighash2, doubled, doubled));
    BOOST_CHECK(!ecdsa_verify<4>(key2, sighash2, zero_value, s2));
    BOOST_CHECK(!ecdsa_verify<4>(key2, sighash2, r2, order_value));
    BOOST_CHECK(ecdsa_verify<4>(overflow_key, overflow_hash, overflow_r, overflow_s));
    BOOST_CHECK(ecdsa_verify<5>(key2, sighash2, r2, s2));
    BOOST_CHECK(ecdsa_verify<5>(ec_compressed_generator, doubled, doubled, doubled));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_ecdsa__signed__agrees)
{
    BOOST_CHECK(ecdsa_signed(secret1, message1, false));
    BOOST_CHECK(ecdsa_signed(secret2, message2, false));
    BOOST_CHECK(ecdsa_signed(secret3, message3, false));
    BOOST_CHECK(ecdsa_signed(secret4, message4, false));
    BOOST_CHECK(ecdsa_signed(secret1, message1, true));
    BOOST_CHECK(ecdsa_signed(secret2, message2, true));
    BOOST_CHECK(ecdsa_signed(secret3, message3, true));
    BOOST_CHECK(ecdsa_signed(secret4, message4, true));
}

// schnorr
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_schnorr__bip340_valid__true)
{
    BOOST_CHECK(schnorr_verify(bip340_key0, bip340_message0, bip340_signature0));
    BOOST_CHECK(schnorr_verify(bip340_key1, bip340_message1, bip340_signature1));
    BOOST_CHECK(schnorr_verify(bip340_key2, bip340_message2, bip340_signature2));
    BOOST_CHECK(schnorr_verify(bip340_key3, bip340_message3, bip340_signature3));
    BOOST_CHECK(schnorr_verify(bip340_key4, bip340_message4, bip340_signature4));
    BOOST_CHECK(schnorr_verify(bip340_key15, bip340_message15, bip340_signature15));
    BOOST_CHECK(schnorr_verify(bip340_key15, bip340_message16, bip340_signature16));
    BOOST_CHECK(schnorr_verify(bip340_key15, bip340_message17, bip340_signature17));
    BOOST_CHECK(schnorr_verify(bip340_key15, bip340_message18, bip340_signature18));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_schnorr__bip340_invalid__false)
{
    BOOST_CHECK(!schnorr_verify(bip340_key5, bip340_message1, bip340_signature5));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature6));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature7));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature8));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature9));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature10));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature11));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature12));
    BOOST_CHECK(!schnorr_verify(bip340_key1, bip340_message1, bip340_signature13));
    BOOST_CHECK(!schnorr_verify(bip340_key14, bip340_message1, bip340_signature14));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_schnorr__windows__expected)
{
    BOOST_CHECK(schnorr_verify<4>(bip340_key0, bip340_message0, bip340_signature0));
    BOOST_CHECK(schnorr_verify<4>(bip340_key1, bip340_message1, bip340_signature1));
    BOOST_CHECK(schnorr_verify<4>(bip340_key4, bip340_message4, bip340_signature4));
    BOOST_CHECK(schnorr_verify<4>(bip340_key15, bip340_message18, bip340_signature18));
    BOOST_CHECK(!schnorr_verify<4>(bip340_key5, bip340_message1, bip340_signature5));
    BOOST_CHECK(!schnorr_verify<4>(bip340_key1, bip340_message1, bip340_signature6));
    BOOST_CHECK(!schnorr_verify<4>(bip340_key1, bip340_message1, bip340_signature7));
    BOOST_CHECK(!schnorr_verify<4>(bip340_key1, bip340_message1, bip340_signature13));
    BOOST_CHECK(!schnorr_verify<4>(bip340_key14, bip340_message1, bip340_signature14));
    BOOST_CHECK(schnorr_verify<5>(bip340_key0, bip340_message0, bip340_signature0));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_schnorr__signed__agrees)
{
    BOOST_CHECK(schnorr_signed(secret1, message1, false));
    BOOST_CHECK(schnorr_signed(secret2, message2, false));
    BOOST_CHECK(schnorr_signed(secret3, message3, false));
    BOOST_CHECK(schnorr_signed(secret4, message4, false));
    BOOST_CHECK(schnorr_signed(secret1, message1, true));
    BOOST_CHECK(schnorr_signed(secret2, message2, true));
    BOOST_CHECK(schnorr_signed(secret3, message3, true));
    BOOST_CHECK(schnorr_signed(secret4, message4, true));
}

// batch
// ----------------------------------------------------------------------------

template <typename Key>
struct rows_t
{
    std_vector<Key> keys{};
    hashes digests{};
    ec_signatures signatures{};
    data_chunk expected{};
};

template <typename Key>
static void add_row(rows_t<Key>& rows, const Key& key, const hash_digest& hash,
    const ec_signature& signature, bool valid) NOEXCEPT
{
    rows.keys.push_back(key);
    rows.digests.push_back(hash);
    rows.signatures.push_back(signature);
    rows.expected.push_back(to_int<uint8_t>(valid));
}

static void add_schnorr(rows_t<ec_xonly>& rows, const ec_xonly& key,
    const hash_digest& message, const ec_signature& signature,
    bool valid) NOEXCEPT
{
    add_row(rows, key, challenge(signature, key, message), signature, valid);
}

// Canonical (r || s) ECDSA signature by libsecp256k1.
static ec_signature ecdsa_signature(ec_compressed& key, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    ec_signature signature{}, canonical{};
    secret_to_public(key, secret);
    ecdsa::sign(signature, secret, hash);
    ecdsa::canonicalize_signature(canonical, signature);
    return canonical;
}

// BIP340 signature by libsecp256k1.
static ec_signature schnorr_signature(ec_xonly& key, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    ec_compressed point{};
    ec_signature signature{};
    secret_to_public(point, secret);
    schnorr::sign(signature, secret, hash, hash);
    key = array_cast<uint8_t, ec_xonly_size, one>(point);
    return signature;
}

static const rows_t<ec_compressed>& ecdsa_rows() NOEXCEPT
{
    static const auto rows = []() NOEXCEPT
    {
        rows_t<ec_compressed> out{};
        ec_compressed key{};
        add_row(out, key2, sighash2, splice(r2, s2), true);
        add_row(out, key2, sighash2, splice(r2, s2_negated), true);
        add_row(out, overflow_key, overflow_hash, splice(overflow_r, overflow_s), true);
        add_row(out, key2, overflow_hash, splice(r2, s2), false);
        add_row(out, key2, sighash2, splice(zero_value, s2), false);
        add_row(out, bad_sign, sighash2, splice(r2, s2), false);
        add_row(out, zero_x, sighash2, splice(r2, s2), false);
        add_row(out, key, message1, ecdsa_signature(key, secret1, message1), true);
        add_row(out, key, message2, ecdsa_signature(key, secret2, message2), true);
        add_row(out, key, message3, ecdsa_signature(key, secret3, message3), true);
        add_row(out, key, message4, ecdsa_signature(key, secret4, message4), true);
        add_row(out, key, message1, ecdsa_signature(key, secret4, message4), false);
        add_row(out, ec_compressed_generator, doubled, splice(doubled, doubled), true);
        return out;
    }();

    return rows;
}

static const rows_t<ec_xonly>& schnorr_rows() NOEXCEPT
{
    static const auto rows = []() NOEXCEPT
    {
        rows_t<ec_xonly> out{};
        ec_xonly key{};
        const auto message = [](const data_chunk& bytes) NOEXCEPT
        {
            return data_slice{ bytes }.to_array<hash_size>();
        };

        add_schnorr(out, bip340_key0, message(bip340_message0), bip340_signature0, true);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature1, true);
        add_schnorr(out, bip340_key2, message(bip340_message2), bip340_signature2, true);
        add_schnorr(out, bip340_key3, message(bip340_message3), bip340_signature3, true);
        add_schnorr(out, bip340_key4, message(bip340_message4), bip340_signature4, true);
        add_schnorr(out, bip340_key5, message(bip340_message1), bip340_signature5, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature6, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature7, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature8, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature9, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature10, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature11, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature12, false);
        add_schnorr(out, bip340_key1, message(bip340_message1), bip340_signature13, false);
        add_schnorr(out, bip340_key14, message(bip340_message1), bip340_signature14, false);
        add_schnorr(out, key, message1, schnorr_signature(key, secret1, message1), true);
        add_schnorr(out, key, message2, schnorr_signature(key, secret2, message2), true);
        add_schnorr(out, key, message3, schnorr_signature(key, secret3, message3), true);
        add_schnorr(out, key, message4, schnorr_signature(key, secret4, message4), true);
        add_schnorr(out, key, message1, schnorr_signature(key, secret4, message4), false);
        return out;
    }();

    return rows;
}

template <typename Word>
static void check_ecdsa_batch()
{
    const auto& rows = ecdsa_rows();
    data_chunk results{};
    BOOST_CHECK(!accessor::verify_ecdsa<Word>(results, rows.keys, rows.digests, rows.signatures));
    BOOST_CHECK_EQUAL(results, rows.expected);
}

template <typename Word>
static void check_schnorr_batch()
{
    const auto& rows = schnorr_rows();
    data_chunk results{};
    BOOST_CHECK(!accessor::verify_schnorr<Word>(results, rows.keys, rows.digests, rows.signatures));
    BOOST_CHECK_EQUAL(results, rows.expected);
}

template <typename Word>
static void check_batches()
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        check_ecdsa_batch<Word>();
        check_schnorr_batch<Word>();
    }
    else if constexpr (have<Word>)
    {
        check_ecdsa_batch<Word>();
        check_schnorr_batch<Word>();
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__batch__integral__expected)
{
    check_batches<uint64_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__batch__lanes__expected)
{
    check_batches<xint128_t>();
    check_batches<xint256_t>();
    check_batches<xint512_t>();
}

static bool combined(const rows_t<ec_xonly>& rows, size_t first,
    size_t count) NOEXCEPT
{
    return accessor::verify_schnorr(
        std::span{ rows.keys }.subspan(first, count),
        std::span{ rows.digests }.subspan(first, count),
        std::span{ rows.signatures }.subspan(first, count));
}

static const rows_t<ec_xonly>& duplicate_rows() NOEXCEPT
{
    static const auto rows = []() NOEXCEPT
    {
        const auto& in = schnorr_rows();
        rows_t<ec_xonly> out{};
        add_row(out, in.keys[1], in.digests[1], in.signatures[1], true);
        add_row(out, in.keys[1], in.digests[1], in.signatures[1], true);
        add_row(out, in.keys[2], in.digests[2], in.signatures[2], true);
        return out;
    }();

    return rows;
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__verify_schnorr__combined__expected)
{
    const auto& rows = schnorr_rows();
    BOOST_CHECK(combined(rows, 0, 0));
    BOOST_CHECK(combined(rows, 0, 5));
    BOOST_CHECK(combined(rows, 15, 4));
    BOOST_CHECK(combined(duplicate_rows(), 0, 3));
    BOOST_CHECK(!combined(rows, 0, 6));
    BOOST_CHECK(!combined(rows, 15, 5));
    BOOST_CHECK(!combined(rows, 6, 1));
    BOOST_CHECK(!combined(rows, 7, 1));
    BOOST_CHECK(!combined(rows, 8, 1));
    BOOST_CHECK(!combined(rows, 9, 1));
    BOOST_CHECK(!combined(rows, 10, 1));
    BOOST_CHECK(!combined(rows, 11, 1));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__batch__valid_rows__true)
{
    const auto& rows = ecdsa_rows();
    const std::span<const ec_compressed> keys{ rows.keys.data(), 3 };
    const std::span<const hash_digest> digests{ rows.digests.data(), 3 };
    const std::span<const ec_signature> signatures{ rows.signatures.data(), 3 };
    data_chunk results{};
    BOOST_CHECK(accessor::verify_ecdsa<uint64_t>(results, keys, digests, signatures));
    BOOST_CHECK_EQUAL(results, data_chunk({ 1, 1, 1 }));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_verify__batch__empty__true)
{
    data_chunk results{};
    BOOST_CHECK(accessor::verify_ecdsa<uint64_t>(results, std::span<const ec_compressed>{}, std::span<const hash_digest>{}, std::span<const ec_signature>{}));
    BOOST_CHECK(results.empty());
}

BOOST_AUTO_TEST_SUITE_END()
