/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TIMER_HPP
#define LIBBITCOIN_SYSTEM_TIMER_HPP

#include <chrono>
#include <cstddef>
#include <time.h>
#include <string>
#include <bitcoin/system/utility/asio.hpp>

namespace libbitcoin {
namespace system {

/// Current zulu (utc) time using the wall clock.
/// BUGBUG: en.wikipedia.org/wiki/Year_2038_problem
inline std::time_t zulu_time()
{
    using wall_clock = std::chrono::system_clock;
    const auto now = wall_clock::now();
    return wall_clock::to_time_t(now);
}

/// Current local time using the wall clock, false and empty string on failure.
inline bool local_time(tm& out_local, std::time_t zulu)
{
    // localtime not threadsafe due to static buffer return, use localtime_s.
#ifdef _MSC_VER
    // proprietary msvc implemention, parameters swapped, returns errno_t.
    return localtime_s(&out_local, &zulu) == 0;
#else
    // C++11 implemention returns parameter pointer, nullptr implies failure.
    return localtime_r(&zulu, &out_local) != nullptr;
#endif
}

/// Standard date-time string, e.g. Sun Oct 17 04:41:13 2010, locale dependent.
inline std::string local_time()
{
    tm out_local{};
    if (!local_time(out_local, zulu_time()))
        return "";

    // %c writes standard date and time string, e.g.
    // Sun Oct 17 04:41:13 2010 (locale dependent)
    static const auto format = "%c";
    static constexpr size_t size = 25;
    char buffer[size];

    // std::strftime is required because gcc doesn't implement std::put_time.
    // Returns number of characters, zero implies failure and undefined buffer.
    return std::strftime(buffer, size, format, &out_local) == 0 ? "" : buffer;
}

// From: github.com/picanumber/bureaucrat/blob/master/time_lapse.h
// boost::timer::auto_cpu_timer requires the boost timer lib dependency.

/// Class to measure the execution time of a callable.
template <typename Time=asio::milliseconds, class Clock=asio::steady_clock>
struct timer
{
    /// Returns the quantity (count) of the elapsed time as TimeT units.
    template <typename Function, typename ...Args>
    static typename Time::rep execution(Function func, Args&&... args)
    {
        return duration(func, std::forward<Args>(args)...).count();
    }

    /// Returns the duration (in chrono's type system) of the elapsed time.
    template <typename Function, typename... Args>
    static Time duration(Function func, Args&&... args)
    {
        auto start = Clock::now();
        func(std::forward<Args>(args)...);
        return std::chrono::duration_cast<Time>(Clock::now() - start);
    }
};

} // namespace system
} // namespace libbitcoin

#endif
