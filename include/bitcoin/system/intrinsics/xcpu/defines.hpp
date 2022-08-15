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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_DEFINES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_DEFINES_HPP

#include <bitcoin/system/define.hpp>

#if defined(HAVE_XCPU)
    #include <immintrin.h>
    #if defined(HAVE_X64)
        #include <nmmintrin.h>
    #endif
    #if defined(HAVE_MSC)
        #include <intrin.h>
    #endif
#endif

// TODO: ARM is unverified.
#if defined(HAVE_ARM)
    #include <arm_acle.h>
    #if defined(HAVE_NEON)
        #include <arm_neon.h>
    #endif
#endif

#endif
