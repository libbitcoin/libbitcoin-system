#include <bitcoin/bitcoin.hpp>
using namespace bc;

blockchain* c = nullptr;
std::string addr;

void blockchain_started(const std::error_code& ec);
void history_fetched(const std::error_code& ec,
    const output_point_list& outpoints, const output_value_list& values,
    const input_point_list& inpoint);

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
    const output_point_list& outpoints, const output_value_list& values,
    const input_point_list& inpoints)
{
    if (ec)
    {
        log_error() << "Failed to fetch history: " << ec.message();
        return;
    }
    BITCOIN_ASSERT(outpoints.size() == values.size());
    BITCOIN_ASSERT(outpoints.size() == inpoints.size());
#define LOG_RESULT "result"
    uint64_t total_recv = 0, balance = 0;
    for (size_t i = 0; i < outpoints.size(); ++i)
    {
        uint64_t value = values[i];
        BITCOIN_ASSERT(value >= 0);
        total_recv += value;
        if (inpoints[i].hash == null_hash)
            balance += value;
    }
    log_debug(LOG_RESULT) << "Queried " << outpoints.size()
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
    chain.start("database", blockchain_started);
    pool.shutdown();
    pool.join();
    chain.stop();
    return 0;
}

