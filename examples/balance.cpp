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

// THIS FILE IS CURRENTLY BUILD DISABLED CAUSE OF EXTERNAL BREAKING CHANGES.

using namespace bc;

blockchain* c = nullptr;
std::string addr;

void blockchain_started(const std::error_code& ec);
void history_fetched(const std::error_code& ec,
    const blockchain::history_list& history);

void blockchain_started(const std::error_code& ec)
{
    if (ec)
    {
        log_error() << "Blockchain failed to start: " << ec.message();
        return;
    }
    log_info() << "Blockchain started.";
    payment_address payaddr;
    if (!payaddr.set_encoded(addr))
    {
        log_fatal() << "Invalid address";
        return;
    }
    c->fetch_history(payaddr, history_fetched);
}

void history_fetched(const std::error_code& ec,
    const blockchain::history_list& history)
{
    if (ec)
    {
        log_error() << "Failed to fetch history: " << ec.message();
        return;
    }
#define LOG_RESULT "result"
    uint64_t total_recv = 0, balance = 0;
    for (const auto& row: history)
    {
        uint64_t value = row.value;
        BITCOIN_ASSERT(value >= 0);
        total_recv += value;
        if (row.spend.hash == null_hash)
            balance += value;
    }
    log_debug(LOG_RESULT) << "Queried " << history.size()
        << " outpoints, values and their spends.";
    log_debug(LOG_RESULT) << "Total received: " << total_recv;
    log_debug(LOG_RESULT) << "Balance: " << balance;
    log_info(LOG_RESULT) << "History fetched";
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        log_info() << "Usage: balance ADDRESS";
        return -1;
    }
    addr = argv[1];
    threadpool pool(1);
    leveldb_blockchain chain(pool);
    c = &chain;
    chain.start("blockchain", blockchain_started);
    pool.shutdown();
    pool.join();
    chain.stop();
    return 0;
}

