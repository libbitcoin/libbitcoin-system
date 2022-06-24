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

BOOST_AUTO_TEST_SUITE(program_tests)

BOOST_AUTO_TEST_CASE(program__construct__todo__todo)
{
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()

// Performance considerations.
// ----------------------------------------------------------------------------

// stdlib object sizes are subjsct to implementation (including debug builds).
#if defined(_MSC_VER) && defined(NDEBUG)

// std::vector<uint8_t> requires 3 pointers (front/back/size).
constexpr auto a1_ = sizeof(std::vector<uint8_t>*);
constexpr auto b1_ = sizeof(std::vector<uint8_t>&);
constexpr auto c1_ = sizeof(std::vector<uint8_t>);
static_assert(a1_ == 1 * sizeof(size_t));
static_assert(b1_ == 3 * sizeof(size_t));
static_assert(c1_ == 3 * sizeof(size_t));

// std::vector<std::vector<uint8_t>> does not increase this (per element) cost.
constexpr auto a2_ = sizeof(std::vector<std::vector<uint8_t>*>::value_type);
constexpr auto c2_ = sizeof(std::vector<std::vector<uint8_t>>::value_type);
constexpr auto d2_ = sizeof(std::vector<std::shared_ptr<std::vector<uint8_t>>>::value_type);
static_assert(a2_ == 1 * sizeof(size_t));
static_assert(c2_ == 3 * sizeof(size_t));
static_assert(d2_ == 2 * sizeof(size_t));

// std::vector<std::vector<uint8_t>> requires 3 pointers (singular cost).
constexpr auto a3 = sizeof(std::vector<std::vector<uint8_t>*>);
constexpr auto c3 = sizeof(std::vector<std::vector<uint8_t>>);
constexpr auto d3 = sizeof(std::vector<std::shared_ptr<std::vector<uint8_t>>>);
static_assert(a3 == 3 * sizeof(size_t));
static_assert(c3 == 3 * sizeof(size_t));
static_assert(d3 == 3 * sizeof(size_t));

// ----------------------------------------------------------------------------

// std::variant<bool, int64_t, std::vector<uint8_t>> object reference requires 4 pointers (max element + 1).
constexpr auto a4 = sizeof(std::variant<bool, int64_t, std::vector<uint8_t>*>);
constexpr auto c4 = sizeof(std::variant<bool, int64_t, std::vector<uint8_t>>);
static_assert(a4 == 2 * sizeof(int64_t));
static_assert(c4 == 2 * sizeof(size_t) + 2 * sizeof(int64_t));

// std::vector<std::variant<bool, int64_t, std::vector<uint8_t>>> does not increase this (per element) cost.
constexpr auto a5 = sizeof(std::vector<std::variant<bool, int64_t, std::vector<uint8_t>*>>::value_type);
constexpr auto c5 = sizeof(std::vector<std::variant<bool, int64_t, std::vector<uint8_t>>>::value_type);
static_assert(a5 == 2 * sizeof(int64_t));
static_assert(c5 == 2 * sizeof(size_t) + 2 * sizeof(int64_t));

// std::vector<std::variant<bool, int64_t, std::vector<uint8_t>>> requires 3 pointers (singular cost).
constexpr auto a6 = sizeof(std::vector<std::variant<bool, int64_t, std::vector<uint8_t>*>>);
constexpr auto c6 = sizeof(std::vector<std::variant<bool, int64_t, std::vector<uint8_t>>>);
static_assert(a6 == 3 * sizeof(size_t));
static_assert(c6 == 3 * sizeof(size_t));

// ----------------------------------------------------------------------------

// std::shared_ptr<std::vector<uint8_t>> requires 2 pointers (ptr/refcount).
constexpr auto a7 = sizeof(std::shared_ptr<std::vector<uint8_t>>::element_type*);
constexpr auto b7 = sizeof(std::shared_ptr<std::vector<uint8_t>>);
static_assert(a7 == 1 * sizeof(size_t));
static_assert(b7 == 2 * sizeof(size_t));

// std::variant<bool, int64_t, std::shared_ptr<std::vector<uint8_t>>> requires 3 pointers (max element + 1).
constexpr auto a8 = sizeof(std::variant<bool, int64_t, std::shared_ptr<std::vector<uint8_t>>>);
static_assert(a8 == 2 * sizeof(size_t) + 1 * sizeof(int64_t));

// std::variant<bool, int64_t, const std::vector<uint8_t>*> requires 2 pointers (max element + 1).
// This would be possible in the case where all stack data_chunks are externally owned.
// This is the case for all script/witness push data elements (but not for computed hashes).
// The program may need to hold a shared_ptr to the witness and script, though (like the tx) it
// could be made a requirement of the caller to retain the passed-by-reference objects in scope.
constexpr auto a9 = sizeof(std::variant<bool, int64_t, std::vector<uint8_t>*>);
constexpr auto c9 = sizeof(std::variant<bool, int64_t, std::shared_ptr<std::vector<uint8_t>>>);
constexpr auto d9 = sizeof(std::variant<bool, int64_t, std::vector<uint8_t>>);
static_assert(a9 == 2 * sizeof(int64_t));
static_assert(c9 == 2 * sizeof(size_t) + 1 * sizeof(int64_t));
static_assert(d9 == 2 * sizeof(size_t) + 2 * sizeof(int64_t));

// Computed hashes can be maintained in the variant by std::unique_ptr<std::vector<uint8_t>>.
// This constrols their lifetime without increasing the size of the union. But
// std::unique_ptr<std::vector<uint8_t>> can be used as a safer std::vector<uint8_t>*,
// so that's all that is needed.
constexpr auto a10 = sizeof(std::variant<bool, int64_t, std::unique_ptr<std::vector<uint8_t>>, std::vector<uint8_t>*>);
constexpr auto a11 = sizeof(std::variant<bool, int64_t, std::unique_ptr<std::vector<uint8_t>>>);
static_assert(a10 == 2 * sizeof(int64_t));
static_assert(a11 == 2 * sizeof(int64_t));

// Use of std::weak_ptr<data_chunk> vs. std::vector<uint8_t>* would expand the
// variant size, and use of std::unique_ptr<std::vector<uint8_t>> with the shared
// pointer's raw pointer would attempt to delete the shared-object's underlying
// data_chunk. So a C ponter is necessary for shared_ptr references, and their
// lifetimes must be externally controlled.
constexpr auto a0 = sizeof(std::weak_ptr<data_chunk>);
static_assert(a0 == 2 * sizeof(size_t));

#endif // MSC_VER_ && NDEBUG

// copy/move restrictions of unique_ptr:
////auto foo()
////{
////    auto unique = std::make_unique<data_chunk>(data_chunk{});
////    auto shared = std::make_shared<data_chunk>(data_chunk{});
////
////    std::unique_ptr<data_chunk> unique_ref_unique(unique.get());
////    std::unique_ptr<data_chunk> unique_ref_shared(shared.get());
////    ////std::shared_ptr<data_chunk> unique_copy_unique(unique);
////    std::shared_ptr<data_chunk> unique_copy_shared(shared);
////    std::unique_ptr<data_chunk> unique_move_unique(std::move(unique));
////    ////std::unique_ptr<data_chunk> unique_move_shared(std::move(shared));
////    unique = std::move(unique_move_unique);
////
////    std::shared_ptr<data_chunk> shared_ref_unique(unique.get());
////    std::shared_ptr<data_chunk> shared_ref_shared(shared.get());
////    std::shared_ptr<data_chunk> shared_copy_shared(shared);
////    ////std::shared_ptr<data_chunk> shared_copy_unique(unique);
////    std::shared_ptr<data_chunk> shared_move_unique(std::move(unique));
////    std::shared_ptr<data_chunk> shared_move_shared(std::move(shared));
////    shared = std::move(shared_move_unique);
////}

// Note that operation.data is nullptr (2 pointers vs. 5) for non-push data
// opcodes (and one byte for the overflow boolean). This is additional script
// overhead in order to provide shared pointers to avoid push data copy. If
// average data element byte length is < (2 * sizeof(pointer) + 1), such as
// predominantly non-puh data opcodes, this implies an increased average cost.
//
// std::vector<data_chunk> => 3 pointers per element.
// std::vector<std::shared_ptr<data_chunk>> => 5 pointers per element.
//
// The additional cost of stack storage for std::shared_ptr is 2 pointers per
// element, and shared pointer origination (i.e. by the tx) requires 2 more (4).
// This allows avoidance of a full vector copy of each data chunk pushed to the
// stack, instead copying its shared pointer from script/witness data. The
// vector copy would require 3 pointers for the copy construction of the new
// vector for each stack element, in addition to the element contents (up to 520
// bytes per element). So the net tx/stack storage cost of avoiding push data
// copy is 5-4=1 pointer storage/copy per element.
//
// std::vector<data_chunk> => 3
// std::vector<std::variant<boo, int64_t, std::shared_ptr<data_chunk>>> => 6
//
// This change to variant added a cost of 1 pointer per stack element (only) to
// the previous cost of 2 ponters per element, making the net cost 2 pointers
// per stack element (with no change to the operation stack). This enables
// strong typing of stack data as bool/int64_t (or chunk_cptr), eliminating
// a large number of chunk<->integral conversions, as well as 3 pointer vector
// constructions and value copies for all information pushed to the stack.
// 
// std::vector<data_chunk> => 3
// std::vector<std::variant<boo, int64_t, const data_chunk&>> => 2
//
// This eliminates the use of unnecessary shared pointer construction for all
// script/witness push data, limiting it to hash-constructed stack elements.
// This would cost 2 pointers per hash construction (as before) but eliminate
// 4 pointers for all other stack data. So in relation to the simple vactor of
// data_chunk, it is -1 for push data and +1 for hash constructions. There is
// typically far more push data than hash construction, so this is a clear win.

// pay-to-public-key-hash
// With average signature size and compressed public key, output/input:
// "[72] [33] : dup hash160 [32] equalverify checksig"

// script<uint8_t> constructions (two vectors):
// vector[72 + 33 + 1 + 1 + 32 + 1 + 1] => 2*3 + 141 = 147 bytes.
// vs:
// script<operation> constructions (overflow, opcode, shared_pointer):
// op(vector[72]) 1 + 1 + 16 + 3 + 72
// op(vector[33]) 1 + 1 + 16 + 3 + 33
// op(nullptr)    1 + 1 + 16
// op(nullptr)    1 + 1 + 16
// op(vector[32]) 1 + 1 + 16 + 3 + 32
// op(nullptr)    1 + 1 + 16
// op(nullptr)    1 + 1 + 16
// vector[7*18 + 3*3 + 72 + 33 + 32] => 2*3 + 272 = 278 bytes (278/147 = 189%).

// Stack vector<data_chunk> construction:
// vector[72]           24 + 72
// vector[33]           24 + 33
// vector[dup]          24 + 33
// vector[hash160]      24 + 32          (hash moved to new vector)
// vector[checksig]     24 + 1           (1 byte for success result)
// Stack copy: 5*24 + 72 + 2*33 + 32 + 1 = 291 bytes.
// Inclusive of script<operation> constructions: (291 + 278)/(291 + 147) = 130%.
// vs:
// Previous conversion to shared_ptr push data storage:
// Stack vector<shared_ptr<data_chunk>> construction:
// ptr[72]              16
// ptr[33]              16
// ptr[dup]             16
// ptr[hash160]         16 + 24 + 32     (hash moved to new shared_ptr[vector])
// ptr[checksig]        16 + 24 + 1      (1 byte for success result)
// Stack copy: 5*16 + 2*24 + 32 + 1 = 161 bytes (161/291 = 55%).
// Inclusive of script<operation> constructions: (161 + 278)/(291 + 147) = 100%.
// vs:
// Recent conversion to variant storage using shared_ptr:
// Stack vector<variant<bool, int64_t, shared_ptr<data_chunk>>> construction:
// var[ptr[vector[72]]] 24
// var[ptr[vector[33]]] 24
// var[ptr[dup]]]       24
// var[ptr[hash160]]]   24 + 24 + 32     (hash moved to new shared_ptr, vector)
// var[bool[checksig]]  24               (native bool union)
// Stack copy: 5*24 + 2*24 + 32 + 1 = 201 bytes (201/161 = 125%, 201/291 = 69%).
// Inclusive of script<operation> constructions: (201 + 278)/(291 + 147) = 109%.
// vs:
// Possible conversion to variant storage using data_chunk&:
// Stack vector<variant<bool, int64_t, data_chunk&>> construction:
// var[vector[72]&]     24
// var[vector[33]&]     24
// var[[dup]&]          24
// var[[hash160]&]      24 + 24 + 32     (hash move, heap pointer storage)
// var[bool[checksig]]  24               (native bool union)
// Stack copy: 5*16 + 24 + 32 = 136 bytes (136/201 = 68%, 136/161 = 84%, 136/291 = 47%).
// Inclusive of script<operation> constructions: (136 + 278)/(291 + 147) = 95%.
