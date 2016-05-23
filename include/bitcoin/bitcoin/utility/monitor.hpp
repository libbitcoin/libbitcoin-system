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
#ifndef LIBBITCOIN_MONITOR_HPP
#define LIBBITCOIN_MONITOR_HPP

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

/// A reference counting wrapper for closures placed on the asio work heap.
class BC_API monitor
{
public:
    typedef std::atomic<size_t> count;
    typedef std::shared_ptr<count> count_ptr;

    monitor(count_ptr counter, const std::string& name);
    ~monitor();

    template <typename Handler>
    void invoke(Handler handler) const
    {
        ////trace(*counter_, "*");
        handler();
    }

    void trace(size_t count, const std::string& action) const;

private:
    count_ptr counter_;
    const std::string name_;
};

} // namespace libbitcoin

#endif
