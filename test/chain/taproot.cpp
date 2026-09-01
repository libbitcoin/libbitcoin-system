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

BOOST_AUTO_TEST_SUITE(taproot_tests)

using namespace system::chain;

class taproot_accessor
  : public taproot
{
public:
    using taproot::merkle_root;
    using taproot::sorted_branch_hash;
    using taproot::branch_hash;
    using taproot::tweak_hash;
};

// leaf_hash

BOOST_AUTO_TEST_CASE(taproot__leaf_hash__single_leaf_tree__expected)
{
    const script leaf(base16_chunk("20d85a959b0290bf19bb89ed43c916be835475d013da4b362117393e25a48229b8ac"), false);
    const auto hash = taproot::leaf_hash(tapscript_version, leaf);
    BOOST_REQUIRE_EQUAL(hash, base16_array("5b75adecf53548f3ec6ad7d78383bf84cc57b55a3127c72b9a2481752dd88b21"));
}

BOOST_AUTO_TEST_CASE(taproot__leaf_hash__two_leaf_tree_first__expected)
{
    const script leaf(base16_chunk("20387671353e273264c495656e27e39ba899ea8fee3bb69fb2a680e22093447d48ac"), false);
    const auto hash = taproot::leaf_hash(tapscript_version, leaf);
    BOOST_REQUIRE_EQUAL(hash, base16_array("8ad69ec7cf41c2a4001fd1f738bf1e505ce2277acdcaa63fe4765192497f47a7"));
}

BOOST_AUTO_TEST_CASE(taproot__leaf_hash__version_250_leaf__expected)
{
    const script leaf(base16_chunk("06424950333431"), false);
    const auto hash = taproot::leaf_hash(250, leaf);
    BOOST_REQUIRE_EQUAL(hash, base16_array("f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a"));
}

// branch hashing

BOOST_AUTO_TEST_CASE(taproot__sorted_branch_hash__argument_order__invariant)
{
    constexpr auto left = base16_array("ba982a91d4fc552163cb1c0da03676102d5b7a014304c01f0c77b2b8e888de1c");
    constexpr auto right = base16_array("9e31407bffa15fefbf5090b149d53959ecdf3f62b1246780238c24501d5ceaf6");
    const auto forward = taproot_accessor::sorted_branch_hash(left, right);
    const auto reversed = taproot_accessor::sorted_branch_hash(right, left);
    BOOST_REQUIRE_EQUAL(forward, reversed);
}

BOOST_AUTO_TEST_CASE(taproot__sorted_branch_hash__three_leaf_tree__expected_root)
{
    constexpr auto leaf0 = base16_array("2645a02e0aac1fe69d69755733a9b7621b694bb5b5cde2bbfc94066ed62b9817");
    constexpr auto leaf1 = base16_array("ba982a91d4fc552163cb1c0da03676102d5b7a014304c01f0c77b2b8e888de1c");
    constexpr auto leaf2 = base16_array("9e31407bffa15fefbf5090b149d53959ecdf3f62b1246780238c24501d5ceaf6");
    const auto inner = taproot_accessor::sorted_branch_hash(leaf1, leaf2);
    const auto root = taproot_accessor::sorted_branch_hash(leaf0, inner);
    BOOST_REQUIRE_EQUAL(root, base16_array("ccbd66c6f7e8fdab47b3a486f59d28262be857f30d4773f2d5ea47f7761ce0e2"));
}

// merkle_root

BOOST_AUTO_TEST_CASE(taproot__merkle_root__empty_path__tapleaf_hash)
{
    const tapscript::keys_t keys{};
    constexpr auto tapleaf = base16_array("5b75adecf53548f3ec6ad7d78383bf84cc57b55a3127c72b9a2481752dd88b21");
    BOOST_REQUIRE_EQUAL(taproot_accessor::merkle_root(keys, 0, tapleaf), tapleaf);
}

BOOST_AUTO_TEST_CASE(taproot__merkle_root__single_element_path__expected)
{
    tapscript::keys_t keys{};
    keys.at(0) = base16_array("f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a");
    constexpr auto tapleaf = base16_array("8ad69ec7cf41c2a4001fd1f738bf1e505ce2277acdcaa63fe4765192497f47a7");
    const auto root = taproot_accessor::merkle_root(keys, 1, tapleaf);
    BOOST_REQUIRE_EQUAL(root, base16_array("6c2dc106ab816b73f9d07e3cd1ef2c8c1256f519748e0813e4edd2405d277bef"));
}

// tweak_hash

BOOST_AUTO_TEST_CASE(taproot__tweak_hash__single_leaf_tree__expected)
{
    constexpr auto internal = base16_array("187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27");
    constexpr auto merkle = base16_array("5b75adecf53548f3ec6ad7d78383bf84cc57b55a3127c72b9a2481752dd88b21");
    const auto tweak = taproot_accessor::tweak_hash(internal, merkle);
    BOOST_REQUIRE_EQUAL(tweak, base16_array("cbd8679ba636c1110ea247542cfbd964131a6be84f873f7f3b62a777528ed001"));
}

// verify_commit

BOOST_AUTO_TEST_CASE(taproot__verify_commit__single_leaf_tree__true)
{
    const auto control = to_shared(base16_chunk("c1187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"));
    const tapscript script_path{ control };
    constexpr ec_xonly out_key = base16_array("147c9c57132f6e7ecddba9800bb0c4449251c92a1e60371ee77557b6620f3ea3");
    constexpr auto leaf = base16_array("5b75adecf53548f3ec6ad7d78383bf84cc57b55a3127c72b9a2481752dd88b21");
    BOOST_REQUIRE(taproot::verify_commit(script_path, out_key, leaf));
}

BOOST_AUTO_TEST_CASE(taproot__verify_commit__wrong_parity__false)
{
    const auto control = to_shared(base16_chunk("c0187791b6f712a8ea41c8ecdd0ee77fab3e85263b37e1ec18a3651926b3a6cf27"));
    const tapscript script_path{ control };
    constexpr ec_xonly out_key = base16_array("147c9c57132f6e7ecddba9800bb0c4449251c92a1e60371ee77557b6620f3ea3");
    constexpr auto leaf = base16_array("5b75adecf53548f3ec6ad7d78383bf84cc57b55a3127c72b9a2481752dd88b21");
    BOOST_REQUIRE(!taproot::verify_commit(script_path, out_key, leaf));
}

BOOST_AUTO_TEST_CASE(taproot__verify_commit__two_leaf_tree_with_path__true)
{
    const auto control = to_shared(base16_chunk("c0ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a"));
    const tapscript script_path{ control };
    constexpr ec_xonly out_key = base16_array("712447206d7a5238acc7ff53fbe94a3b64539ad291c7cdbc490b7577e4b17df5");
    constexpr auto leaf = base16_array("8ad69ec7cf41c2a4001fd1f738bf1e505ce2277acdcaa63fe4765192497f47a7");
    BOOST_REQUIRE(taproot::verify_commit(script_path, out_key, leaf));
}

BOOST_AUTO_TEST_CASE(taproot__verify_commit__wrong_leaf__false)
{
    const auto control = to_shared(base16_chunk("c0ee4fe085983462a184015d1f782d6a5f8b9c2b60130aff050ce221ecf3786592f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a"));
    const tapscript script_path{ control };
    constexpr ec_xonly out_key = base16_array("712447206d7a5238acc7ff53fbe94a3b64539ad291c7cdbc490b7577e4b17df5");
    constexpr auto leaf = base16_array("f224a923cd0021ab202ab139cc56802ddb92dcfc172b9212261a539df79a112a");
    BOOST_REQUIRE(!taproot::verify_commit(script_path, out_key, leaf));
}

// drop_annex

BOOST_AUTO_TEST_CASE(taproot__drop_annex__annex_pattern__dropped)
{
    chunk_cptrs stack
    {
        to_shared(base16_chunk("0102")),
        to_shared(base16_chunk("500102"))
    };

    BOOST_REQUIRE(taproot::drop_annex(stack));
    BOOST_REQUIRE_EQUAL(stack.size(), 1u);
    BOOST_REQUIRE_EQUAL(*stack.front(), base16_chunk("0102"));
}

BOOST_AUTO_TEST_CASE(taproot__drop_annex__no_annex__unchanged)
{
    chunk_cptrs stack
    {
        to_shared(base16_chunk("0102")),
        to_shared(base16_chunk("510102"))
    };

    BOOST_REQUIRE(!taproot::drop_annex(stack));
    BOOST_REQUIRE_EQUAL(stack.size(), 2u);
}

BOOST_AUTO_TEST_SUITE_END()
