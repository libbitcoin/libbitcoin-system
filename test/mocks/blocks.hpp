/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_MOCKS_BLOCKS
#define LIBBITCOIN_SYSTEM_TEST_MOCKS_BLOCKS

#include "../test.hpp"

namespace test {

using block_data = system::data_array<215>;
using header_data = system::data_array<80>;

extern const system::hash_digest block0_hash;
extern const system::hash_digest block1_hash;
extern const system::hash_digest block2_hash;
extern const system::hash_digest block3_hash;
extern const system::hash_digest block4_hash;
extern const system::hash_digest block5_hash;
extern const system::hash_digest block6_hash;
extern const system::hash_digest block7_hash;
extern const system::hash_digest block8_hash;
extern const system::hash_digest block9_hash;

extern const system::hash_digest root01;
extern const system::hash_digest root23;
extern const system::hash_digest root03;
extern const system::hash_digest root45;
extern const system::hash_digest root67;
extern const system::hash_digest root47;
extern const system::hash_digest root07;
extern const system::hash_digest root82;
extern const system::hash_digest root84;
extern const system::hash_digest root88;
extern const system::hash_digest root08;

extern const header_data header0_data;
extern const header_data header1_data;
extern const header_data header2_data;
extern const header_data header3_data;
extern const header_data header4_data;
extern const header_data header5_data;
extern const header_data header6_data;
extern const header_data header7_data;
extern const header_data header8_data;
extern const header_data header9_data;

extern const block_data block1_data;
extern const block_data block2_data;
extern const block_data block3_data;
extern const block_data block4_data;
extern const block_data block5_data;
extern const block_data block6_data;
extern const block_data block7_data;
extern const block_data block8_data;
extern const block_data block9_data;

extern const system::chain::block genesis;
extern const system::chain::block block1;
extern const system::chain::block block2;
extern const system::chain::block block3;
extern const system::chain::block block4;
extern const system::chain::block block5;
extern const system::chain::block block6;
extern const system::chain::block block7;
extern const system::chain::block block8;
extern const system::chain::block block9;
extern const system::chain::block mock_block10;
extern const system::chain::block mock_block11;
extern const system::chain::block mock_block12;

extern const system::chain::block mock_block_a;
extern const system::chain::block block1a;
extern const system::chain::block block2a;
extern const system::chain::block block3a;
extern const system::chain::transaction tx4;
extern const system::chain::transaction tx5;
extern const system::chain::block block1b;
extern const system::chain::block block2b;
extern const system::chain::transaction tx2b;
extern const system::chain::block mock_block;

} // namespace test

#endif
