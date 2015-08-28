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
#ifndef LIBBITCOIN_NETWORK_LOADER_HPP
#define LIBBITCOIN_NETWORK_LOADER_HPP

#include <functional>
#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace network {

class BC_API loader_base
{
public:
    typedef std::shared_ptr<loader_base> ptr;

    virtual code try_load(std::istream& stream) const = 0;
    virtual const std::string command() const = 0;
};

template <typename Message>
class loader
  : public loader_base
{
public:
    typedef std::function<void (const code&, const Message&)> handler;

    loader(handler handler)
      : handle_load_(handler)
    {
    }

    /// This class is not copyable.
    loader(const loader&) = delete;
    void operator=(const loader&) = delete;

    code try_load(std::istream& stream) const
    {
        code status(bc::error::success);
        Message result;

        if (!result.from_data(stream))
        {
            status = bc::error::bad_stream;
            result = Message();
        }

        handle_load_(status, result);
        return status;
    }

    const std::string command() const
    {
        return Message::command;
    }

private:
    handler handle_load_;
};

} // namespace network
} // namespace libbitcoin

#endif

