/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_DICTIONARY_MNEMONIC_HPP
#define LIBBITCOIN_SYSTEM_TEST_DICTIONARY_MNEMONIC_HPP

#include <algorithm>
#include <cstddef>
#include <bitcoin/system.hpp>

// Avoid using namespace in shared headers, but okay here.
using namespace bc::system;
using namespace bc::system::wallet;

namespace test {
namespace dictionaries_mnemonic {

static ptrdiff_t intersection(const mnemonic::dictionary::words& left,
    const mnemonic::dictionary::words& right)
{
    return std::count_if(left.begin(), left.end(), [&](const char test[])
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

static bool intersects(const mnemonic::dictionary::words& left,
    const mnemonic::dictionary::words& right)
{
    return std::any_of(left.begin(), left.end(), [&](const char test[])
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

static ptrdiff_t deviation(const mnemonic::dictionary::words& left,
    const mnemonic::dictionary::words& right)
{
    ptrdiff_t count = 0;

    for (auto outer = left.begin(); outer != left.end(); ++outer)
    {
        const auto inner = std::find(right.begin(), right.end(), *outer);
        if (inner != right.end())
        {
            const auto inner_index = std::distance(right.begin(), inner);
            const auto outer_index = std::distance(left.begin(), outer);
            count += (inner_index == outer_index ? 0 : 1);
        }
    }

    return count;
}

////static bool deviates(const mnemonic::dictionary::words& left,
////    const mnemonic::dictionary::words& right)
////{
////    for (auto outer = left.begin(); outer != left.end(); ++outer)
////    {
////        const auto inner = std::find(right.begin(), right.end(), *outer);
////        if (inner != right.end())
////        {
////            const auto inner_index = std::distance(right.begin(), inner);
////            const auto outer_index = std::distance(left.begin(), outer);
////            if (inner_index != outer_index) return true;
////        }
////    }
////
////    return false;
////}

#ifdef WITH_ICU

////static ptrdiff_t abnormals(const mnemonic::dictionary::words& words)
////{
////    return std::count_if(words.begin(), words.end(), [&](const char test[])
////    {
////        std::string copy = test;
////        to_compatibility_decomposition(copy);
////        to_lower(copy);
////        return test != copy;
////    });
////}

static bool abnormal(const mnemonic::dictionary::words& words)
{
    return std::all_of(words.begin(), words.end(), [&](const char test[])
    {
        std::string copy = test;
        to_compatibility_decomposition(copy);
        to_lower(copy);
        return test != copy;
    });
}

#endif

} // dictionaries_mnemonic
} // test

#endif
