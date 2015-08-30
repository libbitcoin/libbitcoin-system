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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_NETWORK_STREAM_LOADER_HPP
#define LIBBITCOIN_NETWORK_STREAM_LOADER_HPP

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/loader.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace network {

class BC_API stream_loader
{
public:
    stream_loader();

    /// This class is not copyable.
    stream_loader(const stream_loader&) = delete;
    void operator=(const stream_loader&) = delete;

    template <typename Message>
    void add(typename loader<Message>::handler handler)
    {
        // loader isn't copyable, so we use pointers here.
        auto module = std::make_shared<loader<Message>>(handler);
        modules_[module->command()] = module;
    }

    // TODO: implement remove() so that a channel can dynamically start/stop
    // processing messages via protocol attach/detach.

    code load(const std::string& command, std::istream& stream) const;

private:
    typedef std::map<std::string, loader_base::ptr> module_list;

    module_list modules_;
};

} // namespace network
} // namespace libbitcoin

#endif

