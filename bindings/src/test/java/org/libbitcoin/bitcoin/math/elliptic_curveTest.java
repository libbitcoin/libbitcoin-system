/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
package org.libbitcoin.bitcoin.math;

class elliptic_curveTest {
	//BOOST_AUTO_TEST_SUITE(elliptic_curve_tests)
	
	// Scenario 1
	private static final String SECRET1 = "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536";
	private static final String COMPRESSED1 "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731";
	private static final String UNCOMPRESSED1 "0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc7327318c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9";
	
	// Scenario 2
	private static final String COMPRESSED2 "03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b";
	private static final String SIGHASH2 "ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f";
	private static final String SIGNATURE2 "3045022100bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cda33a62437802206b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f6b8bb67d";
	
	// Scenario 3
	private static final String SECRET3 "ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333";
	private static final String SIGHASH3 "f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0";
	private static final String EC_SIGNATURE3 "4832febef8b31c7c922a15cb4063a43ab69b099bba765e24facef50dfbb4d057928ed5c6b6886562c2fe6972fd7c7f462e557129067542cce6b37d72e5ea5037";
	private static final String DER_SIGNATURE3 "3044022057d0b4fb0df5cefa245e76ba9b099bb63aa46340cb152a927c1cb3f8befe324802203750eae5727db3e6cc4275062971552e467f7cfd7269fec2626588b6c6d58e92";
	
	public static void elliptic_curve__secret_to_public__positive__test()
	{
//		ec_compressed point;
//		assertTrue(secret_to_public(point, base16_literal(SECRET1)));
//		assertEquals(encode_base16(point), COMPRESSED1);
	}
	
	public static void elliptic_curve__decompress__positive__test()
	{
//		ec_uncompressed uncompressed;
//		assertTrue(decompress(uncompressed, base16_literal(COMPRESSED1)));
//		assertEquals(encode_base16(uncompressed), UNCOMPRESSED1);
	}
	
	public static void elliptic_curve__sign__positive__test()
	{
//		ec_signature signature;
//		const ec_secret secret = hash_literal(SECRET3);
//		const hash_digest sighash = hash_literal(SIGHASH3);
//		assertTrue(sign(signature, secret, sighash));
//	
//		const auto result = encode_base16(signature);
//		assertEquals(result, EC_SIGNATURE3);
	}
	
	public static void elliptic_curve__encode_signature__positive__test()
	{
//		der_signature out;
//		const ec_signature signature = base16_literal(EC_SIGNATURE3);
//		assertTrue(encode_signature(out, signature));
//	
//		const auto result = encode_base16(out);
//		assertEquals(result, DER_SIGNATURE3);
	}
	
	public static void elliptic_curve__sign__round_trip_positive__test()
	{
//		ec_compressed point;
//		ec_signature signature;
//		const data_chunk data{ 'd', 'a', 't', 'a' };
//		const hash_digest hash = bitcoin_hash(data);
//		const ec_secret secret = hash_literal(SECRET1);
//		assertTrue(secret_to_public(point, secret));
//		assertTrue(sign(signature, secret, hash));
//		assertTrue(verify_signature(point, hash, signature));
	}
	
	public static void elliptic_curve__sign__round_trip_negative__test()
	{
//		ec_compressed point;
//		ec_signature signature;
//		const data_chunk data{ 'd', 'a', 't', 'a' };
//		hash_digest hash = bitcoin_hash(data);
//		const ec_secret secret = base16_literal(SECRET1);
//		assertTrue(secret_to_public(point, secret));
//		assertTrue(sign(signature, secret, hash));
//	
//		// Invalidate the positive test.
//		hash[0] = 0;
//		assertTrue(!verify_signature(point, hash, signature));
	}
	
	public static void elliptic_curve__verify_signature__positive__test()
	{
//		ec_signature signature;
//		static const auto strict = false;
//		const hash_digest sighash = hash_literal(SIGHASH2);
//		const ec_compressed point = base16_literal(COMPRESSED2);
//		der_signature distinguished;
//		assertTrue(decode_base16(distinguished, SIGNATURE2));
//		assertTrue(parse_signature(signature, distinguished, strict));
//		assertTrue(verify_signature(point, sighash, signature));
	}
	
	public static void elliptic_curve__verify_signature__negative__test()
	{
//		ec_signature signature;
//		static const auto strict = false;
//		const hash_digest sighash = hash_literal(SIGHASH2);
//		const ec_compressed point = base16_literal(COMPRESSED2);
//		der_signature distinguished;
//		assertTrue(decode_base16(distinguished, SIGNATURE2));
//		assertTrue(parse_signature(signature, distinguished, strict));
//	
//		// Invalidate the positive test.
//		signature[10] = 110;
//		assertTrue(!verify_signature(point, sighash, signature));
	}
	
	public static void elliptic_curve__ec_add__positive__test()
	{
//		ec_secret secret1{ { 1, 2, 3 } };
//		const ec_secret secret2{ { 3, 2, 1 } };
//		ec_compressed public1;
//		assertTrue(secret_to_public(public1, secret1));
//		assertTrue(ec_add(secret1, secret2));
//		assertEquals(encode_base16(secret1), "0404040000000000000000000000000000000000000000000000000000000000");
//	
//		ec_compressed public2;
//		assertTrue(secret_to_public(public2, secret1));
//		assertTrue(ec_add(public1, secret2));
//		assertTrue(std::equal(public1.begin(), public1.end(), public2.begin()));
	}
	
	public static void elliptic_curve__ec_add__negative__test()
	{
//		// = n - 1
//		ec_secret secret1 = base16_literal("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
//		ec_secret secret2{ { 0 } };
//		secret2[31] = 1;
//		ec_compressed public1;
//		assertTrue(secret_to_public(public1, secret1));
//		assertTrue(!ec_add(secret1, secret2));
//		assertTrue(!ec_add(public1, secret2));
	}
	
	public static void elliptic_curve__ec_multiply_test()
	{
//		ec_secret secret1{{0}};
//		ec_secret secret2{{0}};
//		secret1[31] = 11;
//		secret2[31] = 22;
//		ec_compressed public1;
//		assertTrue(secret_to_public(public1, secret1));
//		assertTrue(ec_multiply(secret1, secret2));
//		assertEquals(secret1[31], 242u);
//		assertTrue(ec_multiply(public1, secret2));
//	
//		ec_compressed public2;
//		assertTrue(secret_to_public(public2, secret1));
//		assertTrue(std::equal(public1.begin(), public1.end(), public2.begin()));
	}

	public static void main(String[] args) {
	}
}