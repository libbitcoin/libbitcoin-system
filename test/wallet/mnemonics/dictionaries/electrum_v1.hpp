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
#ifndef LIBBITCOIN_SYSTEM_TEST_DICTIONARY_ELECTRUM_V1_HPP
#define LIBBITCOIN_SYSTEM_TEST_DICTIONARY_ELECTRUM_V1_HPP

#include <algorithm>
#include <cstddef>
#include <bitcoin/system.hpp>

// Avoid using namespace in shared headers, but okay here.
using namespace bc::system;
using namespace bc::system::wallet;

namespace test {
namespace dictionaries_electrum_v1 {

#ifdef WITH_ICU

////ptrdiff_t abnormals(const electrum_v1::dictionary::words& words)
////{
////    return std::count_if(words.begin(), words.end(), [&](const char test[])
////    {
////        std::string copy = test;
////        to_compatibility_decomposition(copy);
////        return test != copy;
////    });
////}

bool abnormal(const electrum_v1::dictionary::words& words)
{
    return std::all_of(words.begin(), words.end(), [&](const char test[])
    {
        std::string copy = test;
        to_compatibility_decomposition(copy);
        return test != copy;
    });
}

#endif

////static bool intersects(const electrum_v1::dictionary::words& left,
////    const electrum_v1::dictionary::words& right)
////{
////    return std::any_of(left.begin(), left.end(), [&](const char test[])
////    {
////        return std::find(right.begin(), right.end(), test) != right.end();
////    });
////}

static ptrdiff_t intersection(const electrum_v1::dictionary::words& left,
    const electrum_v1::dictionary::words& right)
{
    return std::count_if(left.begin(), left.end(), [&](const char test[])
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

static bool intersects(const electrum_v1::dictionary::words& left,
    const electrum::dictionary::words& right)
{
    return std::any_of(left.begin(), left.end(), [&](const char test[])
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

static ptrdiff_t intersection(const electrum_v1::dictionary::words& left,
    const electrum::dictionary::words& right)
{
    return std::count_if(left.begin(), left.end(), [&](const char test[])
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

} // dictionaries_electrum_v1
} // test

#endif
