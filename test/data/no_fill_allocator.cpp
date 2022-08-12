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

BOOST_AUTO_TEST_SUITE(no_fill_allocator_tests)

BOOST_AUTO_TEST_CASE(no_fill_allocator__construct__default__unassigned)
{
    constexpr auto count = 3_u8;
    constexpr auto fill = 42_u8;
    constexpr auto size = count * hash_size;
    constexpr auto hash_fill = hash_digest{ fill };

    // no_fill_allocator must be incorporated into container type.
    // These tests depedent upon assumptions of default memory fill, so disabled.

    ////std::vector<uint8_t, no_fill_allocator<uint8_t>> bytes(size);
    ////BOOST_CHECK_NE(bytes.front(), 0x00_u8);

    ////std::vector<hash_digest, no_fill_allocator<hash_digest>> hashes(count);
    ////BOOST_CHECK_NE(hashes.front(), hash_digest{});

    // no_fill_allocator forwards explicit fill to type copy constructor.

    std::vector<uint8_t, no_fill_allocator<uint8_t>> filled_bytes1(size, fill);
    BOOST_CHECK_EQUAL(filled_bytes1.front(), fill);

    std::vector<hash_digest, no_fill_allocator<hash_digest>> filled_hashes1(count, hash_fill);
    BOOST_CHECK_EQUAL(filled_hashes1.front(), hash_fill);
}

// Some extra stuff
// ----------------------------------------------------------------------------

template <class Type, bool Test = true>
struct noalloc_allocator
{
    using value_type = Type;
 
    noalloc_allocator() = default;

    template <class T>
    constexpr noalloc_allocator(const noalloc_allocator<T>&) noexcept
    {
    }
 
    // std lib uses [[nodiscard]] on allocator::allocate members.
    // en.cppreference.com/w/cpp/language/attributes/nodiscard
    NODISCARD 
    Type* allocate(size_t count) noexcept(false)
    {
        if (count > std::numeric_limits<size_t>::max() / sizeof(Type))
        {
            throw std::bad_array_new_length();
        }
 
        if constexpr (is_same_type<Type, hash_digest>)
        {
            report(nullptr, count, true, true);

            // This must be combined with no_fill_allocator.
            // But no_fill_allocator must also bypass fill construction.
            return nullptr;
        }
        else
        {
            if (auto ptr = static_cast<Type*>(std::malloc(count * sizeof(Type))))
            {
                report(ptr, count, true, false);
                return ptr;
            }

            throw std::bad_alloc();
        }
    }
 
    void deallocate(Type* ptr, size_t count) noexcept(false)
    {
        if constexpr (is_same_type<Type, hash_digest>)
        {
            report(ptr, count, false, true);
        }
        else
        {
            report(ptr, count, false, false);
            std::free(ptr);
        }
    }
 
private:
    void report(Type*, size_t count, bool alloc, bool fake) const noexcept(false)
    {
        std::cout
            << (fake ? "faked_" : "real__")
            << (alloc ? "alloc___: " : "dealloc_: ")
            << (sizeof(Type) * count) << " bytes at " << std::hex
            ////<< std::showbase << reinterpret_cast<void*>(ptr) << std::dec
            << " for " << typeid(Type).name() << "[" << count << "]."
            << std::endl;
    }
};


BOOST_AUTO_TEST_SUITE_END()
