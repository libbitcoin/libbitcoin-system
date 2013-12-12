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
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/verify.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/exporter.hpp>
#include <iostream>

using std::shared_ptr;
using libbitcoin::verify_block;
typedef shared_ptr<verify_block> verify_block_ptr;
using libbitcoin::postgresql_storage;
using libbitcoin::satoshi_exporter;
using libbitcoin::storage_ptr;
using libbitcoin::exporter_ptr;

using std::placeholders::_1;
using std::placeholders::_2;

void block_status(std::error_code ec, bool is_good)
{
    if (ec)
        libbitcoin::log_error() << "Verifying block: " << ec.message() << "\n";
    std::cout << "block is " << (is_good ? "good" : "bad") << "\n";
    exit(0);
}

void recv_block(std::error_code ec, libbitcoin::message::block block, verify_block_ptr verif)
{
    if (ec)
    {
        std::cerr << ec.message() << "\n";
        return;
    }
    verif->start(block, block_status);
}

int main()
{
    storage_ptr psql(new postgresql_storage("bitcoin", "genjix", ""));
    exporter_ptr exporter(new satoshi_exporter);
    verify_block_ptr verif(new verify_block(psql, exporter));
    psql->fetch_block_by_height(2, std::bind(recv_block, _1, _2, verif));
    std::cin.get();
    return 0;
}

