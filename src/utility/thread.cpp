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
#include <bitcoin/system/utility/thread.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <thread>

#ifdef _MSC_VER
    #include <windows.h>
#else
    #include <unistd.h>
    #include <pthread.h>
    #include <sys/resource.h>
    #include <sys/types.h>
    #ifndef PRIO_MAX
        #define PRIO_MAX 20
    #endif
    #define THREAD_PRIORITY_ABOVE_NORMAL (-2)
    #define THREAD_PRIORITY_NORMAL 0
    #define THREAD_PRIORITY_BELOW_NORMAL 2
    #define THREAD_PRIORITY_LOWEST PRIO_MAX
#endif

namespace libbitcoin {
namespace system {

// Privately map the class enum thread priority value to an interger.
static int get_priority(thread_priority priority)
{
    switch (priority)
    {
        case thread_priority::high:
            return THREAD_PRIORITY_ABOVE_NORMAL;
        case thread_priority::normal:
            return THREAD_PRIORITY_NORMAL;
        case thread_priority::low:
            return THREAD_PRIORITY_BELOW_NORMAL;
        case thread_priority::lowest:
            return THREAD_PRIORITY_LOWEST;
        default:
            throw std::invalid_argument("priority");
    }
}

// Set the thread priority (or process if thread priority is not available).
void set_priority(thread_priority priority)
{
    const auto prioritization = get_priority(priority);

#if defined(_MSC_VER)
    SetThreadPriority(GetCurrentThread(), prioritization);
#elif defined(PRIO_THREAD)
    setpriority(PRIO_THREAD, pthread_self(), prioritization);
#else
    setpriority(PRIO_PROCESS, getpid(), prioritization);
#endif
}

thread_priority priority(bool priority)
{
    return priority ? thread_priority::high : thread_priority::normal;
}

inline size_t cores()
{
    // Cores must be at least 1 (guards against irrational API return).
    return std::max(std::thread::hardware_concurrency(), 1u);
}

// This is used to default the number of threads to the number of cores and to
// ensure that no less than one thread is configured.
size_t thread_default(size_t configured)
{
    if (configured == 0)
        return cores();

    // Configured but no less than 1.
    return std::max(configured, size_t(1));
}

// This is used to ensure that threads does not exceed cores in the case of
// parallel work distribution, while allowing the user to reduce parallelism so
// as not to monopolize the processor. It also makes optimal config easy (0).
size_t thread_ceiling(size_t configured)
{
    if (configured == 0)
        return cores();

    // Cores/1 but no more than configured.
    return std::min(configured, cores());
}

// This is used to ensure that at least a minimum required number of threads is
// allocated, so that thread starvation does not occur. It also allows the user
// to increase threads above minimum. It always ensures at least core threads.
size_t thread_floor(size_t configured)
{
    if (configured == 0)
        return cores();

    // Configured but no less than cores/1.
    return std::max(configured, cores());
}

} // namespace system
} // namespace libbitcoin
