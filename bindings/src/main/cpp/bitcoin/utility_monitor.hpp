/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_MONITOR_HPP
#define LIBBITCOIN__UTILITY_MONITOR_HPP

//#include <atomic>
//#include <cstddef>
//#include <memory>
//#include <string>
//#include <utility>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/monitor.hpp>
#include <utility_monitor_count_ptr.hpp>
#include <p_std_string.hpp>

// libbitcoin defines the log and tracking but does not use them.
// These are defined in bc so that they can be used in network and blockchain.

namespace libbitcoin {
namespace api {

/// A reference counting wrapper for closures placed on the asio work heap.
class BC_API utility_monitor
{
public:
//    typedef std::atomic<size_t> count;
//    typedef std::shared_ptr<count> count_ptr;

    utility_monitor(libbitcoin::api::utility_monitor_count_ptr counter, p_std_string name);
    virtual ~utility_monitor();

    template <typename Handler>
    void invoke(Handler handler) const
    {
        ////trace(*counter_, "*");
        handler();
    }

    void trace(size_t, const std::string&) const
    {
        ////#ifndef NDEBUG
        ////    LOG_DEBUG(LOG_SYSTEM)
        ////        << action << " " << name_ << " {" << count << "}";
        ////#endif
    }

public:
    monitor getValue() {
        return value;
    }

    void setValue(monitor value) {
        this->value = value;
    }
private:
    monitor value;
//    count_ptr counter_;
//    const std::string name_;
};

} // namespace api
} // namespace libbitcoin

#endif
