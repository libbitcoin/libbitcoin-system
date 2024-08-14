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
#include <memory_resource>
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

// Facilitates tracing memory allocations through polymorphic resource.
template <bool Report>
class reporting_arena
  : public arena
{
public:
    size_t inc_count{};
    size_t inc_bytes{};
    size_t dec_count{};
    size_t dec_bytes{};

    void* initialize() NOEXCEPT override
    {
        return nullptr;
    }

private:
    void* do_allocate(size_t bytes, size_t align) override
    {
        if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
            throw std::bad_alloc();

        BC_PUSH_WARNING(NO_NEW_OR_DELETE)
        const auto ptr = ::operator new(bytes);
        BC_POP_WARNING()
        report(ptr, bytes, true);
        ++inc_count;
        inc_bytes += bytes;
        return ptr;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t) NOEXCEPT override
    {
        BC_PUSH_WARNING(NO_NEW_OR_DELETE)
        ::operator delete(ptr);
        BC_POP_WARNING()
        report(ptr, bytes, false);
        ++dec_count;
        dec_bytes += bytes;
    }

    bool do_is_equal(const arena& other) const NOEXCEPT override
    {
        return &other == this;
    }

    void report(void* ptr, size_t bytes, bool allocate) const NOEXCEPT
    {
        if constexpr (Report)
        {
            using namespace libbitcoin::system;
            BC_PUSH_WARNING(NO_REINTERPRET_CAST)
            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            std::cout << (allocate ? "+ " : "- ") << bytes << " pmr "
                << encode_base16(to_big_endian(reinterpret_cast<uint64_t>(ptr)))
                << std::endl;
            BC_POP_WARNING()
            BC_POP_WARNING()
        }
    }
};

class mock_arena
  : public arena
{
public:
    size_t do_allocate_bytes{};
    size_t do_allocate_align{};
    void* do_deallocate_ptr{};
    size_t do_deallocate_bytes{};
    size_t do_deallocate_align{};
    mutable const arena* do_is_equal_address{};

    void* initialize() NOEXCEPT override
    {
        return nullptr;
    }

private:
    void* do_allocate(size_t bytes, size_t align) THROWS override
    {
        do_allocate_bytes = bytes;
        do_allocate_align = align;
        return this;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) NOEXCEPT override
    {
        do_deallocate_ptr = ptr;
        do_deallocate_bytes = bytes;
        do_deallocate_align = align;
    }

    bool do_is_equal(const arena& other) const NOEXCEPT override
    {
        do_is_equal_address = &other;
        return false;
    }
};

template <bool Report = false>
test::reporting_arena<Report>* get_test_resource() NOEXCEPT
{
    static test::reporting_arena<Report> resource{};
    return &resource;
}

inline arena* get_default_resource() NOEXCEPT
{
    return default_arena::get();
}

} // namespace test

#endif
