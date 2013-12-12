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
#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

threadpool s;
hosts_ptr h = std::make_shared<hosts>(s);

void handle_store(const std::error_code& ec)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_info() << "fone";
}

void handle_fetch(const std::error_code& ec,
    const message::network_address& netaddr)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_debug() << encode_hex(netaddr.ip) << ' ' << netaddr.port;
}

int main()
{
    //for (uint8_t i1 = 0; i1 < 10; ++i1)
    //{
    //    for (uint8_t i2 = 0; i2 < 10; ++i2)
    //    {
    //        for (uint8_t i3 = 0; i3 < 10; ++i3)
    //        {
    //            for (uint8_t i4 = 0; i4 < 26; ++i4)
    //            {
    //                message::network_address netaddr;
    //                netaddr.ip =
    //    message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //                        0x00, 0x00, 0xff, 0xff, i1, i2, i3, i4};
    //                netaddr.port = 8444;
    //                h->store(netaddr, handle_store);
    //            }
    //        }
    //    }
    //}
    h->load("hosts", handle_store);
    log_info() << "hello";
    h->fetch_address(handle_fetch);
    sleep(2);
    h->save("hosts1", handle_store);
    std::cin.get();
    return 0;
}

