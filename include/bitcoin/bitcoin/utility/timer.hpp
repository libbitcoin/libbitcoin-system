/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_TIMER_HPP
#define LIBBITCOIN_TIMER_HPP

#include <chrono>
#include <bitcoin/bitcoin/utility/asio.hpp>

// boost::timer::auto_cpu_timer requires the boost timer lib dependency.

namespace libbitcoin {

// From: github.com/picanumber/bureaucrat/blob/master/time_lapse.h

/// Class to measure the execution time of a callable.
template <typename Time = asio::milliseconds, class Clock=asio::steady_clock> 
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

} // namespace libbitcoin

#endif
