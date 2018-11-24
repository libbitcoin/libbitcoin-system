/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <ctime>
#include <string>
#include <bitcoin/system/compat.hpp>
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

/// Standard date-time string, e.g. Sun Oct 17 04:41:13 2010, locale dependent.
inline std::string local_time()
{
    static BC_CONSTEXPR size_t size = 25;
    char buffer[size];
    const auto time = zulu_time();

    // std::strftime is required because gcc doesn't implement std::put_time.
    auto result = std::strftime(buffer, size, "%c", std::localtime(&time));

    // If count was reached before the entire string could be stored, zero is
    // returned and contents are undefined, so do not return result.
    return result == 0 ? "" : buffer;
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
        const auto start = Clock::now();
        func(std::forward<Args>(args)...);
        const auto difference = Clock::now() - start;
        const auto duration = std::chrono::duration_cast<Time>(difference);
        return duration.count();
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
