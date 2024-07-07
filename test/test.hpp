/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_HPP
#define LIBBITCOIN_SYSTEM_TEST_HPP

#include <boost/test/unit_test.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <vector>
#include <filesystem>
#include <bitcoin/system.hpp>

/// Have slow test execution (scrypt is slow by design).
////#define HAVE_SLOW_TESTS
////#define HAVE_PERFORMANCE_TESTS

#define TEST_NAME \
    boost::unit_test::framework::current_test_case().p_name.get()
#define SUITE_NAME \
    boost::unit_test::framework::current_auto_test_suite().p_name.get()
#define TEST_DIRECTORY \
    test::directory
#define TEST_PATH \
    TEST_DIRECTORY + "/" + TEST_NAME

#ifdef HAVE_MSC
    BC_DISABLE_WARNING(NO_ARRAY_INDEXING)
    BC_DISABLE_WARNING(NO_GLOBAL_INIT_CALLS)
    BC_DISABLE_WARNING(NO_UNUSED_LOCAL_SMART_PTR)
    BC_DISABLE_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
#endif

using namespace bc;
using namespace bc::system;

namespace std {

// data_slice -> base16(data)
std::ostream& operator<<(std::ostream& stream,
    const data_slice& slice) NOEXCEPT;

// std::vector<Type> -> join(<<Type)
template <typename Type>
std::ostream& operator<<(std::ostream& stream,
    const std::vector<Type>& values) NOEXCEPT
{
    // Ok when testing serialize because only used for error message out.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << serialize(values);
    BC_POP_WARNING()
    return stream;
}

// std_vector<Type> -> join(<<Type)
template <typename Type>
std::ostream& operator<<(std::ostream& stream,
    const std_vector<Type>& values) NOEXCEPT
{
    // Ok when testing serialize because only used for error message out.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << serialize(values);
    BC_POP_WARNING()
    return stream;
}

// array<Type, Size> -> join(<<Type)
template <typename Type, size_t Size>
std::ostream& operator<<(std::ostream& stream,
    const std_array<Type, Size>& values) NOEXCEPT
{
    // Ok when testing serialize because only used for error message out.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << serialize(values);
    BC_POP_WARNING()
    return stream;
}

} // namespace std

namespace test {

// Common directory for all test file creations.
// Subdirectories and/or files must be differentiated (i.e. by TEST_NAME).
// Total path length cannot exceed MAX_PATH in HAVE_MSC builds.
extern const std::string directory;

bool clear(const std::filesystem::path& directory) NOEXCEPT;
bool create(const std::filesystem::path& file_path) NOEXCEPT;
bool exists(const std::filesystem::path& file_path) NOEXCEPT;
bool remove(const std::filesystem::path& file_path) NOEXCEPT;

// Utility to convert a const reference instance to moveable.
template <typename Type>
Type move_copy(const Type& instance) NOEXCEPT
{
    auto copy = instance;
    return copy;
}

} // namespace test

#endif
