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
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/timed_section.hpp>

#ifdef __MACH__
// Mac clock_gettime
#include <mach/clock.h>
#include <mach/mach.h>
#define CLOCK_REALTIME 0

void clock_gettime(int ign, struct timespec* ts)
{
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
}
#endif

#ifdef _WIN32
// THIS SECTION IS UNTESTED
// Windows clock_gettime from: http://stackoverflow.com/a/5404467/1172329
#define CLOCK_REALTIME 0

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

void clock_gettime(int ign, timespec* ts)
{
    static bool initialized = false;
    static bool usePerformanceCounter = false;
    static double frequencyToNanoseconds;
    static LARGE_INTEGER offset;

    // Initialize statics
    if (!initialized) {
        initialized = true;

        LARGE_INTEGER performanceFrequency;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency) != FALSE;

        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToNanoseconds = (double)performanceFrequency.QuadPart / 1000000000.0;
        } else {
            offset = getFILETIMEoffset();

            // A file time is a 64-bit value that represents the number of 100-nanosecond intervals.
            // This was a factor of 10 for conversion to ms so must divide by 100 to convert to ns.
            frequencyToNanoseconds = .01;
        }
    }

    FILETIME ft;
    LARGE_INTEGER time;

    if (usePerformanceCounter) 
        QueryPerformanceCounter(&time);
    else {
        GetSystemTimeAsFileTime(&ft);
        time.QuadPart = ft.dwHighDateTime;
        time.QuadPart <<= 32;
        time.QuadPart |= ft.dwLowDateTime;
    }

    time.QuadPart -= offset.QuadPart;
    double nanoseconds = (double)time.QuadPart / frequencyToNanoseconds;
    ts->tv_sec = (time_t)(nanoseconds / 1000000000);
    ts->tv_nsec = (int32_t)(((int64_t)nanoseconds) % 1000000000);
}
#endif

namespace libbitcoin {

timed_section::timed_section(
    const std::string& context, const std::string& section)
  : context_(context), section_(section)
{
    clock_gettime(CLOCK_REALTIME, &start_);
}

timed_section::~timed_section()
{
    log_debug(context_) << section_ << " " << std::fixed << elapsed();
}

double timed_section::elapsed() const
{
    timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    return 1e3 * (end.tv_sec - start_.tv_sec) +
        1e-6 * (end.tv_nsec - start_.tv_nsec);
}

} // namespace libbitcoin

