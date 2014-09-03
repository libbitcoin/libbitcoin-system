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
/*
  Stealth query.
*/
#include <bitcoin/bitcoin.hpp>

// THIS FILE IS CURRENTLY BUILD DISABLED CAUSE OF EXTERNAL BREAKING CHANGES.

using namespace bc;

blockchain* chain = nullptr;

// Completion handler for when the blockchain has finished initializing.
void blockchain_started(const std::error_code& ec);
// Results for stealth query passed here.
void stealth_fetched(const std::error_code& ec,
    const blockchain::stealth_list& stealth_results);

void blockchain_started(const std::error_code& ec)
{
    // std::error_code's can be tested like bools, and
    // compared against specific error enums.
    // See <bitcoin/error.hpp> for a full list of them.
    if (ec)
    {
        log_error() << "Blockchain failed to start: " << ec.message();
        return;
    }
    // Blockchain has safely started.
    log_info() << "Blockchain started.";
    // chain should've been set inside main().
    assert(chain);
    // Construct stealth query prefix.
    stealth_prefix prefix{2, 0x691cf48b};
    // Begin fetching the stealth results.
    chain->fetch_stealth(prefix, stealth_fetched);
}

void stealth_fetched(const std::error_code& ec,
    const blockchain::stealth_list& stealth_results)
{
    if (ec)
    {
        log_error() << "Stealth fetch failed: " << ec.message();
        return;
    }
    for (const blockchain::stealth_row& row: stealth_results)
        log_debug() << "ephemkey: " << row.ephemkey
            << " address: " << row.address.encoded()
            << " tx_hash: " << row.transaction_hash;
}

int main()
{
    // Define a threadpool with 1 thread.
    threadpool pool(1);
    // Create a LevelDB blockchain.
    leveldb_blockchain ldb_chain(pool);
    // Initialize our global 'chain' pointer from above.
    chain = &ldb_chain;
    // Start the database using its implementation specific method.
    ldb_chain.start("blockchain", blockchain_started);
    // Don't wait after all current operations have completed.
    pool.shutdown();
    // Join them one by one.
    pool.join();
    // Finally stop the blockchain safely now everything has stopped.
    ldb_chain.stop();
    return 0;
}

