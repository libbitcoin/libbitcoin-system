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

int start_time;

void blockchain_started(const std::error_code& ec, blockchain_ptr)
{
    if (ec)
        log_error() << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

void handle_fetch(const std::error_code& ec,
    const message::output_point_list& outpoints)
{
    log_info() << "Fetch: " << ec.message()
        << " (" << (time(nullptr) - start_time) << " seconds)";
}

int main()
{
    threadpool pool(1);
    blockchain_ptr chain =
        bdb_blockchain::create(pool, "database", blockchain_started);
    payment_address address("1VayNert3x1KzbpzMGt2qdqrAThiRovi8");
    start_time = time(nullptr);
    chain->fetch_outputs(address, handle_fetch);
    std::cin.get();
    return 0;
}

