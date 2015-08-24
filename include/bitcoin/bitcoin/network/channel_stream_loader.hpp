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
#ifndef LIBBITCOIN_NETWORK_CHANNEL_STREAM_LOADER_HPP
#define LIBBITCOIN_NETWORK_CHANNEL_STREAM_LOADER_HPP

#include <map>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel_loader_module.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace network {

class BC_API channel_stream_loader
{
public:
    channel_stream_loader();
    ~channel_stream_loader();

    /// This class is not copyable.
    channel_stream_loader(const channel_stream_loader&) = delete;
    void operator=(const channel_stream_loader&) = delete;

    template <typename Message>
    void add(typename channel_loader_module<Message>::load_handler handler)
    {
        // channel_loader_module isn't copyable, so we use pointers here.
        auto module = new channel_loader_module<Message>(handler);
        modules_[module->lookup_symbol()] = module;
    }

    code load(const std::string& symbol, std::istream& stream) const;


private:
    typedef std::map<std::string, channel_loader_module_base*> module_list;

    module_list modules_;
};

} // namespace network
} // namespace libbitcoin

#endif

