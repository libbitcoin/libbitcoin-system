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
#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

void show_last(const std::error_code& ec, size_t last_height)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_info() << "Last block height: " << last_height;
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        log_error() << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

int main()
{
    threadpool pool(1);
    leveldb_blockchain chain(pool);
    chain.start("database", blockchain_started);
    chain.fetch_last_height(show_last);
    std::cin.get();
    pool.stop();
    pool.join();
    chain.stop();
    return 0;
}

