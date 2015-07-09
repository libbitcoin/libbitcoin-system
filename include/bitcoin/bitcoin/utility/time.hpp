/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_TIME_HPP
#define LIBBITCOIN_TIME_HPP

#include <limits>
#include <bitcoin/bitcoin/define.hpp>

#if defined(__MACH__)
    #include <mach/clock.h>
    #include <mach/mach.h>
    #include <time.h>
    #include <sys/time.h>
#elif defined(_MSC_VER)
    #include <stdint.h>
    #include <time.h>
#endif

#if defined(__MACH__)

#define CLOCK_REALTIME 0

BC_API int clock_gettime(int clock_id, timespec* ts);

#elif defined(_MSC_VER)

#define CLOCK_REALTIME 0

struct BC_API timespec
{
    time_t tv_sec;
    int32_t tv_nsec;
};

BC_API int clock_gettime(int, timespec* ts);

#endif

#endif
