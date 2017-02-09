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
using namespace bc;

void bb(const std::error_code& ec, const message::block& blkhead)
{
    log_debug() << ec.message();
    log_debug() << hash_block_header(blkhead);
}

void blockchain_started(const std::error_code& ec, blockchain_ptr chain, bool isnew)
{
    if (ec)
        log_error() << ec.message();
    else
        log_info() << "Blockchain initialized! " << isnew;
    chain->fetch_block_header(0, bb);
}

int main()
{
    threadpool pool(10);
    blockchain_ptr chain =
        kyoto_blockchain::create(pool, "kydb", blockchain_started);
    std::cin.get();
    return 0;
}

