/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include "test.hpp"

#if defined(NDEBUG)
    static_assert(!build_checked);
#else
    static_assert(build_checked);
#endif

#if defined(HAVE_X32) || defined(HAVE_ARM32)
    static_assert(have_32b);
#else
    static_assert(!have_32b);
#endif

#if  defined(HAVE_X64) || defined(HAVE_ARM64)
    static_assert(have_64b);
#else
    static_assert(!have_64b);
#endif

#ifdef HAVE_XCPU
    static_assert(have_xcpu);
#else
    static_assert(!have_xcpu);
#endif

#ifdef HAVE_ARM
    static_assert(have_arm);
#else
    static_assert(!have_arm);
#endif

#if defined(HAVE_128)
    static_assert(have_128);
#else
    static_assert(!have_128);
#endif

#if defined(HAVE_256)
    static_assert(have_256);
#else
    static_assert(!have_256);
#endif

#if defined(HAVE_512)
    static_assert(have_512);
#else
    static_assert(!have_512);
#endif

#if defined(HAVE_SHA)
    static_assert(have_sha);
#else
    static_assert(!have_sha);
#endif