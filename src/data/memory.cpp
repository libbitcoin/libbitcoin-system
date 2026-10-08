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
#include <bitcoin/system/data/memory.hpp>

#include <bitcoin/system/define.hpp>

#if defined(HAVE_VALGRIND)
    #include <valgrind/memcheck.h>
#endif

namespace libbitcoin {
namespace system {

void classify([[maybe_unused]] const void* data,
    [[maybe_unused]] size_t size) NOEXCEPT
{
#if defined(HAVE_VALGRIND)
    VALGRIND_MAKE_MEM_UNDEFINED(data, size);
#endif
}

void declassify([[maybe_unused]] const void* data,
    [[maybe_unused]] size_t size) NOEXCEPT
{
#if defined(HAVE_VALGRIND)
    VALGRIND_MAKE_MEM_DEFINED(data, size);
#endif
}

} // namespace system
} // namespace libbitcoin
