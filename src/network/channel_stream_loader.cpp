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
#include <bitcoin/bitcoin/network/channel_stream_loader.hpp>

#include <istream>
#include <string>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel_loader_module.hpp>

namespace libbitcoin {
namespace network {

channel_stream_loader::channel_stream_loader()
{
}

channel_stream_loader::~channel_stream_loader()
{
    for (auto module: modules_)
        delete module.second;
}

code channel_stream_loader::load(const std::string& symbol,
    std::istream& stream) const
{
    code status(bc::error::bad_stream);

    auto it = modules_.find(symbol);
    if (it != modules_.end())
        status = it->second->attempt_load(stream);

    return status;
}

} // namespace network
} // namespace libbitcoin
