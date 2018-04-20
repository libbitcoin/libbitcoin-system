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
package org.libbitcoin.chain;

import static org.libbitcoin.bitcoin.Test.*;
import static org.libbitcoin.formats.*;

class blockTest {

	static {
        System.loadLibrary("javawrapchain");
	}
	
	static boolean all_valid(TransactionVec transactions)
	{
	    boolean valid = true;
	
	    for (transaction tx: transactions)
	    {
	        valid = valid && tx.is_valid();
	
	        for (input _input: tx.inputs())
	        {
	            valid &= _input.is_valid();
	            valid &= _input.script().is_valid();
	        }
	
	        for (output _output: tx.outputs())
	        {
	            valid &= _output.is_valid();
	            valid &= _output.script().is_valid();
	        }
	    }
	
	    return valid;
	}
		
	public static void block__locator_size__zero_backoff__returns_top_plus_one()
	{
		long top = 7;
	    assertEquals(block.locator_size(top), top + 1);
	}
	
	public static void block__locator_size__positive_backoff__returns_log_plus_eleven()
	{
	    long top = 138;
	    assertEquals(block.locator_size(top), 18);
	}
	
	public static void block__locator_heights__zero_backoff__returns_top_to_zero()
	{
	    long[] expected = { 7, 6, 5, 4, 3, 2, 1, 0 };
	    long top = 7;
	    SizeTVec result = block.locator_heights(top);
	    assertEquals(expected, result)
	}
	
	static boolean assertEquals(long[] expected, SizeTVec actual) {
	    assertTrue(expected.length == actual.size());
	    for (int i=0; i<expected.length; i++) {
	    	assertEquals(expected[i], actual.get(i));
	    }
	}
	
	public static void block__locator_heights__positive_backoff__returns_top_plus_log_offset_to_zero()
	{
	    long[] expected =
	    {
	        138, 137, 136, 135, 134, 133, 132, 131, 130,
	        129, 128, 126, 122, 114,  98,  66,   2,   0
	    };
	
	    long top = 138;
	    SizeTVec result = block.locator_heights(top);
	    assertEquals(expected, result);
	}
	
	public static void block__constructor_1__always__invalid()
	{
	    block instance;
	    assertTrue(!instance.is_valid());
	}
	
	public static void block__constructor_2__always__equals_params()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, transaction(4, 16, {}, {}));
	
	    block instance = new block(header, transactions);
	    assertTrue(instance.is_valid());
	    assertTrue(header.eq(instance.header()));
	    assertEquals(transactions, instance.transactions());
	}
	
	public static void assertEquals(TransactionVec expected, TransactionVec actual() {
		assertTrue(expected.size() == actual.size())
		for (int i=0; i<expected.size(); i++) {
			assertTrue(expected.get(i).eq(actual.get(i)));
		}
	}
	
	public static void block__constructor_3__always__equals_params()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    // These must be non-const.
	    header dup_header = new header(header);
	    TransactionVec dup_transactions = new TransactionVec(transactions.size());
	    for (int i=0; i<transactions.size(); i++) {
	    	dup_transactions.set(i, transactions.get(i));
	    }
	
	    block instance = new block(std::move(dup_header), std::move(dup_transactions));
	    assertTrue(instance.is_valid());
	    assertTrue(header.eq(instance.header()));
	    assertEquals(transactions, instance.transactions());
	
	}
// continue from here
	
	public static void block__constructor_4__always__equals_params()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    block value = new block(header, transactions);
	    block instance = new block(value);
	    assertTrue(instance.is_valid());
	    assertTrue(value.eq(instance));
	    assertTrue(header.eq(instance.header()));
	    assertTrue(transactions.eq(instance.transactions()));
	}
	
	public static void block__constructor_5__always__equals_params()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    // This must be non-const.
	    block value = new block(header, transactions);
	
	    block instance = new block(value);
	    assertTrue(instance.is_valid());
	    assertTrue(header.eq(instance.header()));
	    assertTrue(transactions.eq(instance.transactions()));
	}
	
	public static void block__hash__always__returns_header_hash()
	{
	    block instance = new block();
	    assertTrue(instance.header().hash().eq(instance.hash()));
	}
	
	public static void block__is_valid_merkle_root__uninitialized__returns_true()
	{
	    block instance = new block();
	    assertTrue(instance.is_valid_merkle_root());
	}
	
	public static void block__is_valid_merkle_root__non_empty_tx_invalid_block__returns_false)
	{
	    block instance = new block();
	    TransactionVec transactions = new TransactionVec(1);
	    transactions.set(0, new transaction[0]);
	    instance.set_transactions(transactions);
	    assertTrue(!instance.is_valid_merkle_root());
	}
	
	public static void block__is_valid_merkle_root__valid__returns_true()
	{
	    data_chunk raw_block = to_chunk(base16_literal(
	        new String("010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a0000000"
	        +"0005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4dc118244d6"
	        +"7fb74c9d8e2f1bea5ee82a03010000000100000000000000000000000000000000000"
	        +"00000000000000000000000000000ffffffff07049d8e2f1b0114ffffffff0100f205"
	        +"2a0100000043410437b36a7221bc977dce712728a954e3b5d88643ed5aef46660ddcf"
	        +"eeec132724cd950c1fdd008ad4a2dfd354d6af0ff155fc17c1ee9ef802062feb07ef1"
	        +"d065f0ac000000000100000001260fd102fab456d6b169f6af4595965c03c2296ecf2"
	        +"5bfd8790e7aa29b404eff010000008c493046022100c56ad717e07229eb93ecef2a32"
	        +"a42ad041832ffe66bd2e1485dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1"
	        +"312634664bac46f36ddd35761edaae20cefb16f01410417e418ba79380f462a60d8dd"
	        +"12dcef8ebfd7ab1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583"
	        +"040b1bc341b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914"
	        +"fc7b44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a9146"
	        +"17f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122cd6da2"
	        +"6eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb013010000008c493"
	        +"0460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d15774594bfedc45c4f9"
	        +"9e2f022100ae0135094a7d651801539df110a028d65459d24bc752d7512bc8a9f78b4"
	        +"ab368014104a2e06c38dc72c4414564f190478e3b0d01260f09b8520b196c2f6ec3d0"
	        +"6239861e49507f09b7568189efe8d327c3384a4e488f8c534484835f8020b3669e5ae"
	        +"bffffffff0200ac23fc060000001976a914b9a2c9700ff9519516b21af338d28d53dd"
	        +"f5349388ac00743ba40b0000001976a914eb675c349c474bec8dea2d79d12cff6f330"
	        +"ab48788ac00000000").toBytes(StandardCharsets.ISO_8859_1)));
	
	    block instance = new block();
	    assertTrue(instance.from_data(raw_block));
	    assertTrue(instance.is_valid_merkle_root());
	}
	
//	BOOST_AUTO_TEST_SUITE(block_serialization_tests)
	
	public static void block__from_data__insufficient_bytes__failure()
	{
	    data_chunk data = new data_chunk(10);
	    block instance = new block();
	    assertTrue(!instance.from_data(data));
	    assertTrue(!instance.is_valid());
	}
	
	public static void block__from_data__insufficient_transaction_bytes__failure()
	{
	    data_chunk data = to_chunk(base16_literal(
	        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a00"
	        +"000000005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4"
	        +"dc118244d67fb74c9d8e2f1bea5ee82a03010000000100000000000000000000"
	        +"00000000000000000000000000000000000000000000ffffffff07049d8e2f1b"
	        +"0114ffffffff0100f2052a0100000043410437b36a7221bc977dce712728a954"));
	
	    block instance = new block();
	    assertTrue(!instance.from_data(data));
	    assertTrue(!instance.is_valid());
	}
	
	public static void block__genesis__mainnet__valid_structure()
	{
	    block genesis = bc.chain.block.genesis_mainnet();
	    assertTrue(genesis.is_valid());
	    assertTrue(genesis.transactions().size() == 1);
	    assertTrue(genesis.header().merkle().eq(genesis.generate_merkle_root()));
	}
	
	public static void block__genesis__testnet__valid_structure()
	{
	    block genesis = bc.chain.block.genesis_testnet();
	    assertTrue(genesis.is_valid());
	    assertTrue(genesis.transactions().size() == 1);
	    assertTrue(genesis.header().merkle().eq(genesis.generate_merkle_root()));
	}
	
	public static void block__genesis__regtest__valid_structure()
	{
	    block genesis = bc.chain.block.genesis_regtest();
	    assertTrue(genesis.is_valid());
	    assertTrue(genesis.transactions().size() == 1);
	    assertTrue(genesis.header().merkle().eq( genesis.generate_merkle_root()));
	}
	
	
	public static void block__factory_1__genesis_mainnet__success()
	{
	    block genesis = bc.chain.block.genesis_mainnet();
	    assertTrue(genesis.serialized_size() == 285);
	    assertTrue(genesis.header().serialized_size() == 80);
	
	    // Save genesis block.
	    data_chunk raw_block = genesis.to_data();
	    assertTrue(raw_block.size() == 285);
	
	    // Reload genesis block.
	    block block = chain.block.factory(raw_block);
	
	    assertTrue(block.is_valid());
	    assertTrue(genesis.header().eq( block.header()));
	
	    // Verify merkle root from transactions.
	    assertTrue(genesis.header().merkle().eq(block.generate_merkle_root()));
	}
	
	public static void block__factory_2__genesis_mainnet__success()
	{
	    block genesis = bc.chain.block.genesis_mainnet();
	    assertTrue(genesis.serialized_size() == 285);
	    assertTrue(genesis.header().serialized_size() == 80);
	
	    // Save genesis block.
	    auto raw_block = genesis.to_data();
	    assertTrue(raw_block.size() == 285);
	
	    // Reload genesis block.
	    data_source stream = new data_source(raw_block);
	    block block = chain.block.factory(stream);
	
	    assertTrue(block.is_valid());
	    assertTrue(genesis.header().eq(block.header()));
	
	    // Verify merkle root from transactions.
	    assertTrue(genesis.header().merkle().eq(block.generate_merkle_root()));
	}
	
	public static void block__factory_3__genesis_mainnet__success)
	{
	    block genesis = bc.chain.block.genesis_mainnet();
	    assertTrue(genesis.serialized_size() == 285);
	    assertTrue(genesis.header().serialized_size() == 80);
	
	    // Save genesis block.
	    data_chunk raw_block = genesis.to_data();
	    assertTrue(raw_block.size() == 285);
	
	    // Reload genesis block.
	    data_source stream = new data_source(raw_block);
	    istream_reader reader = new istream_reader(stream);
	    block block = chain.block.factory(reader);
	
	    assertTrue(block.is_valid());
	    assertTrue(genesis.header().eq(block.header()));
	
	    // Verify merkle root from transactions.
	    assertTrue(genesis.header().merkle().eq(block.generate_merkle_root()));
	}
	
//	BOOST_AUTO_TEST_SUITE_END()
	
//	BOOST_AUTO_TEST_SUITE(block_generate_merkle_root_tests)
	
	public static void block__generate_merkle_root__block_with_zero_transactions__matches_null_hash()
	{
	    block empty = new block();
	    assertTrue(empty.generate_merkle_root().eq(null_hash));
	}
	
	public static void block__generate_merkle_root__block_with_multiple_transactions__matches_historic_data()
	{
	    // encodes the 100,000 block data.
	    data_chunk raw = to_chunk(base16_literal(
	        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a00"
	        +"000000005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4"
	        +"dc118244d67fb74c9d8e2f1bea5ee82a03010000000100000000000000000000"
	        +"00000000000000000000000000000000000000000000ffffffff07049d8e2f1b"
	        +"0114ffffffff0100f2052a0100000043410437b36a7221bc977dce712728a954"
	        +"e3b5d88643ed5aef46660ddcfeeec132724cd950c1fdd008ad4a2dfd354d6af0"
	        +"ff155fc17c1ee9ef802062feb07ef1d065f0ac000000000100000001260fd102"
	        +"fab456d6b169f6af4595965c03c2296ecf25bfd8790e7aa29b404eff01000000"
	        +"8c493046022100c56ad717e07229eb93ecef2a32a42ad041832ffe66bd2e1485"
	        +"dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1312634664bac46f36dd"
	        +"d35761edaae20cefb16f01410417e418ba79380f462a60d8dd12dcef8ebfd7ab"
	        +"1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583040b1bc34"
	        +"1b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914fc7b"
	        +"44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a914"
	        +"617f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122"
	        +"cd6da26eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb01301"
	        +"0000008c4930460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d157"
	        +"74594bfedc45c4f99e2f022100ae0135094a7d651801539df110a028d65459d2"
	        +"4bc752d7512bc8a9f78b4ab368014104a2e06c38dc72c4414564f190478e3b0d"
	        +"01260f09b8520b196c2f6ec3d06239861e49507f09b7568189efe8d327c3384a"
	        +"4e488f8c534484835f8020b3669e5aebffffffff0200ac23fc060000001976a9"
	        +"14b9a2c9700ff9519516b21af338d28d53ddf5349388ac00743ba40b00000019"
	        +"76a914eb675c349c474bec8dea2d79d12cff6f330ab48788ac00000000"));
	
	    block block100k = new block();
	    assertTrue(block100k.from_data(raw));
	    assertTrue(block100k.is_valid());
	
	    data_chunk serial = block100k.to_data();
	    assertTrue(raw.eq(serial));
	
	    header header = block100k.header();
	    TransactionVec transactions = block100k.transactions();
	    assertTrue(all_valid(transactions));
	    assertTrue(header.merkle().eq(block100k.generate_merkle_root()));
	}
	
	public static void block__header_accessor__always__returns_initialized_value()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    block instance = new block(header, transactions);
	    assertTrue(header.eq(instance.header()));
	}
	
	public static void block__header_setter_1__roundtrip__success()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    block instance = new block();
	    assertTrue(header.notEqual(instance.header()));
	    instance.set_header(header);
	    assertTrue(header.eq(instance.header()));
	}
	
	public static void block__header_setter_2__roundtrip__success)
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    // This must be non-const.
	    header dup_header = new header(header);
	
	    block instance = new block;
	    assertTrue(header.notEqual(instance.header()));
	    instance.set_header(dup_header);
	    assertTrue(header.eq( instance.header()));
	}
	
	public static void block__transactions_accessor__always__returns_initialized_value()
	{
	    header header = new header(10,
	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
	        531234,
	        6523454,
	        68644);
	
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    block instance = new block(header, transactions);
	    assertTrue(transactions.eq(instance.transactions()));
	}
	
	public static void block__transactions_setter_1__roundtrip__success()
	{
	    TransactionVec transactions = new TransactionVec(3);
	    transaction.set(0, new transaction(1, 48, {}, {}));
	    transaction.set(1, new transaction(2, 32, {}, {}));
	    transaction.set(2, new transaction(4, 16, {}, {}));
	
	    block instance = new block();
	    assertTrue(transactions.notEqual(instance.transactions()));
	    instance.set_transactions(transactions);
	    assertTrue(transactions.eq(instance.transactions()));
	}
	
	public static void block__transactions_setter_2__roundtrip__success()
	{
	    TransactionVec transactions = new TransactionVec(3);
	    transactions.set(0, new transaction(1, 48, {}, {}));
	    transactions.set(1, new transaction(2, 32, {}, {}));
	    transactions.set(2, new transaction(4, 16, {}, {}));
	
	    // This must be non-const.
	    TransactionVec dup_transactions = new TransactionVec(transactions);
	
	    block instance = new block();
	    assertTrue(transactions != instance.transactions());
	    instance.set_transactions(dup_transactions);
	    assertTrue(transactions == instance.transactions());
	}
	
//	public static void block__operator_assign_equals__always__matches_equivalent()
//	{
//	    const chain::header header(10,
//	        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
//	        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
//	        531234,
//	        6523454,
//	        68644);
//	
//	    const chain::transaction::list transactions
//	    {
//	        chain::transaction(1, 48, {}, {}),
//	        chain::transaction(2, 32, {}, {}),
//	        chain::transaction(4, 16, {}, {})
//	    };
//	
//	    // This must be non-const.
//	    chain::block value(header, transactions);
//	
//	    assertTrue(value.is_valid());
//	    block instance = new block();
//	    assertTrue(!instance.is_valid());
//	    instance = std::move(value);
//	    assertTrue(instance.is_valid());
//	    assertTrue(header == instance.header());
//	    assertTrue(transactions == instance.transactions());
//	}
//	
//	public static void block__operator_boolean_equals__duplicates__returns_true()
//	{
//	    const chain::block expected(
//	        chain::header(10u,
//	            hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
//	            hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
//	            531234u,
//	            6523454u,
//	            68644u),
//	        {
//	            chain::transaction(1, 48, {}, {}),
//	            chain::transaction(2, 32, {}, {}),
//	            chain::transaction(4, 16, {}, {})
//	        });
//	
//	    chain::block instance(expected);
//	    assertTrue(instance == expected);
//	}
//	
//	public static void block__operator_boolean_equals__differs__returns_false()
//	{
//	    const chain::block expected(
//	        chain::header(10u,
//	            hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
//	            hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
//	            531234u,
//	            6523454u,
//	            68644u),
//	        {
//	            chain::transaction(1, 48, {}, {}),
//	            chain::transaction(2, 32, {}, {}),
//	            chain::transaction(4, 16, {}, {})
//	        });
//	
//	
//	    block instance = new block();
//	    assertTrue(!(instance == expected));
//	}
//	
//	public static void block__operator_boolean_not_equals__duplicates__returns_false()
//	{
//	    const chain::block expected(
//	        chain::header(10u,
//	            hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
//	            hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
//	            531234u,
//	            6523454u,
//	            68644u),
//	        {
//	            chain::transaction(1, 48, {}, {}),
//	            chain::transaction(2, 32, {}, {}),
//	            chain::transaction(4, 16, {}, {})
//	        });
//	
//	    chain::block instance(expected);
//	    assertTrue(!(instance != expected));
//	}
//	
//	public static void block__operator_boolean_not_equals__differs__returns_true()
//	{
//	    const chain::block expected(
//	        chain::header(10u,
//	            hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
//	            hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
//	            531234u,
//	            6523454u,
//	            68644u),
//	        {
//	            chain::transaction(1, 48, {}, {}),
//	            chain::transaction(2, 32, {}, {}),
//	            chain::transaction(4, 16, {}, {})
//	        });
//	
//	    block instance = new block();
//	    assertTrue(instance != expected);
//	}
	
//	BOOST_AUTO_TEST_SUITE_END()
	
//	BOOST_AUTO_TEST_SUITE(block_is_distinct_transaction_set_tests)
	
	public static void block__distinct_transactions__empty__true()
	{
	    block value = new block();
	    assertTrue(value.is_distinct_transaction_set());
	}
	
//	public static void validate_block__is_distinct_tx_set__single__true()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} } });
//	    assertTrue(value.is_distinct_transaction_set());
//	}
//	
//	public static void validate_block__is_distinct_tx_set__duplicate__false()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 1, 0, {}, {} } });
//	    assertTrue(!value.is_distinct_transaction_set());
//	}
//	
//	public static void validate_block__is_distinct_tx_set__distinct_by_version__true()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 2, 0, {}, {} }, { 3, 0, {}, {} } });
//	    assertTrue(value.is_distinct_transaction_set());
//	}
//	
//	public static void validate_block__is_distinct_tx_set__partialy_distinct_by_version__false()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 2, 0, {}, {} }, { 2, 0, {}, {} } });
//	    assertTrue(!value.is_distinct_transaction_set());
//	}
//	
//	public static void validate_block__is_distinct_tx_set__partialy_distinct_not_adjacent_by_version__false()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 2, 0, {}, {} }, { 1, 0, {}, {} } });
//	    assertTrue(!value.is_distinct_transaction_set());
//	}
	
//	BOOST_AUTO_TEST_SUITE_END()
	
//	BOOST_AUTO_TEST_SUITE(block_is_forward_reference_tests)
	
	public static void block__is_forward_reference__no_transactions__false()
	{
	    block value = new block();
	    assertTrue(!value.is_forward_reference());
	}
	
//	public static void block__is_forward_reference__multiple_empty_transactions__false()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 2, 0, {}, {} } });
//	    assertTrue(!value.is_forward_reference());
//	}
//	
//	public static void block__is_forward_reference__backward_reference__false()
//	{
//	    chain::block value;
//	    chain::transaction before{ 2, 0, {}, {} };
//	    chain::transaction after{ 1, 0, { { { before.hash(), 0 }, {}, 0 } }, {} };
//	    value.set_transactions({ before, after });
//	    assertTrue(!value.is_forward_reference());
//	}
//	
//	public static void block__is_forward_reference__duplicate_transactions__false()
//	{
//	    chain::block value;
//	    value.set_transactions({ { 1, 0, {}, {} }, { 1, 0, {}, {} } });
//	    assertTrue(!value.is_forward_reference());
//	}
//	
//	public static void block__is_forward_reference__coinbase_and_multiple_empty_transactions__false()
//	{
//	    chain::block value;
//	    chain::transaction coinbase{ 1, 0, { { { null_hash, chain::point::null_index }, {}, 0 } }, {} };
//	    value.set_transactions({ coinbase, { 2, 0, {}, {} }, { 3, 0, {}, {} } });
//	    assertTrue(!value.is_forward_reference());
//	}
//	
//	public static void block__is_forward_reference__forward_reference__true()
//	{
//	    chain::block value;
//	    chain::transaction after{ 2, 0, {}, {} };
//	    chain::transaction before{ 1, 0, { { { after.hash(), 0 }, {}, 0 } }, {} };
//	    value.set_transactions({ before, after });
//	    assertTrue(value.is_forward_reference());
//	}
	
	public static void main(String[] args) {
		block__locator_size__zero_backoff__returns_top_plus_one();
		block__locator_size__positive_backoff__returns_log_plus_eleven();
		block__locator_heights__zero_backoff__returns_top_to_zero();
		block__locator_heights__positive_backoff__returns_top_plus_log_offset_to_zero();
		block__constructor_1__always__invalid();
		block__constructor_2__always__equals_params();
		assertEquals(TransactionVec expected, TransactionVec actual(); {
		block__constructor_3__always__equals_params();
		block__constructor_4__always__equals_params();
		block__constructor_5__always__equals_params();
		block__hash__always__returns_header_hash();
		block__is_valid_merkle_root__uninitialized__returns_true();
		block__is_valid_merkle_root__non_empty_tx_invalid_block__returns_false)
		block__is_valid_merkle_root__valid__returns_true();
		block__from_data__insufficient_bytes__failure();
		block__from_data__insufficient_transaction_bytes__failure();
		block__genesis__mainnet__valid_structure();
		block__genesis__testnet__valid_structure();
		block__genesis__regtest__valid_structure();
		block__factory_1__genesis_mainnet__success();
		block__factory_2__genesis_mainnet__success();
		block__factory_3__genesis_mainnet__success)
		block__generate_merkle_root__block_with_zero_transactions__matches_null_hash();
		block__generate_merkle_root__block_with_multiple_transactions__matches_historic_data();
		block__header_accessor__always__returns_initialized_value();
		block__header_setter_1__roundtrip__success();
		block__header_setter_2__roundtrip__success)
		block__transactions_accessor__always__returns_initialized_value();
		block__transactions_setter_1__roundtrip__success();
		block__transactions_setter_2__roundtrip__success();
//		block__operator_assign_equals__always__matches_equivalent();
//		block__operator_boolean_equals__duplicates__returns_true();
//		block__operator_boolean_equals__differs__returns_false();
//		block__operator_boolean_not_equals__duplicates__returns_false();
//		block__operator_boolean_not_equals__differs__returns_true();
		block__distinct_transactions__empty__true();
//		validate_block__is_distinct_tx_set__single__true();
//		validate_block__is_distinct_tx_set__duplicate__false();
//		validate_block__is_distinct_tx_set__distinct_by_version__true();
//		validate_block__is_distinct_tx_set__partialy_distinct_by_version__false();
//		validate_block__is_distinct_tx_set__partialy_distinct_not_adjacent_by_version__false();
		block__is_forward_reference__no_transactions__false();
//		block__is_forward_reference__multiple_empty_transactions__false();
//		block__is_forward_reference__backward_reference__false();
//		block__is_forward_reference__duplicate_transactions__false();
//		block__is_forward_reference__coinbase_and_multiple_empty_transactions__false();
//		block__is_forward_reference__forward_reference__true();

	}
}
