/*
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
#include <bitcoin/bitcoin/compat.hpp>

#ifdef __MACH__
    // Mac clock_gettime from gist.github.com/jbenet/1087739
    #include <mach/clock.h>
    #include <mach/mach.h>
    #include <time.h>
    #include <sys/time.h>

    int clock_gettime(int clock_id, timespec* ts)
    {
        // The clock_id specified is not supported on this system.
        if (clock_id != CLOCK_REALTIME)
        {
            errno = EINVAL;
            return -1;
        }

        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts->tv_sec = mts.tv_sec;
        ts->tv_nsec = mts.tv_nsec;
        return 0;
    }
#endif

// THIS SECTION IS UNTESTED
// Windows clock_gettime from: http://stackoverflow.com/a/5404467/1172329
#ifdef _MSC_VER
    #include <errno.h>
    #include <windows.h>

    LARGE_INTEGER getFILETIMEoffset()
    {
        FILETIME filetime;
        SYSTEMTIME systime;
        LARGE_INTEGER time;
        systime.wYear = 1970;
        systime.wMonth = 1;
        systime.wDay = 1;
        systime.wHour = 0;
        systime.wMinute = 0;
        systime.wSecond = 0;
        systime.wMilliseconds = 0;
        SystemTimeToFileTime(&systime, &filetime);
        time.QuadPart = filetime.dwHighDateTime;
        time.QuadPart <<= 32;
        time.QuadPart |= filetime.dwLowDateTime;
        return time;
    }

    int clock_gettime(int clock_id, timespec* ts)
    {
        // The clock_id specified is not supported on this system.
        if (clock_id != CLOCK_REALTIME)
        {
            errno = EINVAL;
            return -1;
        }

        static bool initialized = false;
        static bool usePerformanceCounter = false;
        static double frequencyToNanoseconds;
        static LARGE_INTEGER offset;

        // Initialize statics
        if (!initialized) 
        {
            initialized = true;

            LARGE_INTEGER performanceFrequency;
            usePerformanceCounter =
                QueryPerformanceFrequency(&performanceFrequency) != FALSE;

            if (usePerformanceCounter) 
            {
                QueryPerformanceCounter(&offset);
                frequencyToNanoseconds = 
                    static_cast<double>(performanceFrequency.QuadPart) /
                    1000000000.0;
            }
            else 
            {
                offset = getFILETIMEoffset();

                // A file time is a 64-bit value that represents the number of
                // 100-nanosecond intervals. This was a factor of 10 for 
                // conversion to ms so must divide by 100 to convert to ns.
                frequencyToNanoseconds = .01;
            }
        }

        FILETIME ft;
        LARGE_INTEGER time;

        if (usePerformanceCounter)
            QueryPerformanceCounter(&time);
        else 
        {
            GetSystemTimeAsFileTime(&ft);
            time.QuadPart = ft.dwHighDateTime;
            time.QuadPart <<= 32;
            time.QuadPart |= ft.dwLowDateTime;
        }

        time.QuadPart -= offset.QuadPart;
        double nanoseconds = static_cast<double>(time.QuadPart) / 
            frequencyToNanoseconds;
        ts->tv_sec = static_cast<time_t>(nanoseconds / 1000000000);
        ts->tv_nsec = static_cast<int32_t>
            (static_cast<int64_t>(nanoseconds) % 1000000000);

        return 0;
    }
#endif