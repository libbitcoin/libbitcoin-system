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
#include "../test.hpp"
#include "hash.hpp"

BOOST_AUTO_TEST_SUITE(functions_tests)

// TODO: add tests for all wrapper functions.

////// null hash vectors.
////constexpr auto rmd_half128 = base16_array("c11f675df95fe3f7f00b6825368bce48");
////constexpr auto rmd_full128 = base16_array("082bfa9b829ef3a9e220dcc54e4c6383");
////constexpr auto rmd_half160 = base16_array("d1a70126ff7a149ca6f9b638db084480440ff842");
////constexpr auto rmd_full160 = base16_array("9b8ccc2f374ae313a914763cc9cdfb47bfe1c229");
////
////// null hash vectors.
////constexpr auto sha_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
////constexpr auto sha_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
////constexpr auto sha_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
////constexpr auto sha_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
////constexpr auto sha_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
////constexpr auto sha_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");

BOOST_AUTO_TEST_CASE(functions__djb2_hash__alphanumeric__expected)
{
    const std::string alpha{ "01234567890abcdefghijklmnopqrstuvwxyz" };
    const auto hash = djb2_hash(alpha);

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 3781598209_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 9646636626660989953_u64);
    }
}

BOOST_AUTO_TEST_CASE(functions__hash_combine__same_values__expected)
{
    static_assert(hash_combine(3781598209_size, 3781598209_size) == 4893418499_size);
}

BOOST_AUTO_TEST_SUITE_END()
