/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/network/discovery.hpp>
#include <bitcoin/error.hpp>

void handler(const std::error_code& ec, const std::string hostname)
{
    if (ec)
    {
        libbitcoin::log_fatal() << ec.message();
        exit(-1);
    }

    libbitcoin::log_info() << "Got new client: " << hostname;
}

int main()
{
    std::shared_ptr<libbitcoin::discovery> disc = std::make_shared<libbitcoin::discovery>();
    disc->irc_discovery(&handler);
    while (std::cin.get() != -1);
}
