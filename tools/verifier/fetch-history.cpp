#include <bitcoin/bitcoin.hpp>
using namespace bc;

blockchain* chain = nullptr;
size_t limit = 292127;
const std::string address = "1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P";
// Make sure nothing else is using the blockchain.
const std::string blockchain_path = "/mnt/disk1/blockchain/";

void history_fetched(const std::error_code& ec,
    const blockchain::history_list& history)
{
    if (ec)
    {
        log_error() << "Failed to fetch last height: " << ec.message();
        return;
    }
    std::cout << "{\"history\": [" << std::endl;
    bool first = true;
    for (const auto& row: history)
    {
        if (row.output_height > limit)
            continue;
        if (first)
            first = false;
        else
            std::cout << "," << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "  \"output\": [\"" << row.output.hash << "\", "
            << row.output.index << "]," << std::endl;
        std::cout << "  \"output_height\": " << row.output_height
            << "," << std::endl;
        std::cout << "  \"output_found\": false," << std::endl;
        std::cout << "  \"value\": " << row.value << "," << std::endl;
        std::cout << "  \"spend\": ";
        if (row.spend.hash == null_hash)
            std::cout << "null";
        else
            std::cout << "[\"" << row.spend.hash << "\", "
                << row.spend.index << "]";
        std::cout << "," << std::endl;
        std::cout << "  \"spend_height\": ";
        if (row.spend.hash == null_hash || row.spend_height > limit)
        {
            std::cout << "null";
            std::cout << "," << std::endl;
            std::cout << "  \"spend_found\": null" << std::endl;
        }
        else
        {
            std::cout << row.spend_height;
            std::cout << "," << std::endl;
            std::cout << "  \"spend_found\": false" << std::endl;
        }
        std::cout << "}";
    }
    std::cout << std::endl << "]}" << std::endl;
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
    {
        log_error() << "Blockchain failed to start: " << ec.message();
        return;
    }
    assert(chain);
    chain->fetch_history(payment_address(address), history_fetched);
}

int main()
{
    threadpool pool(1);
    leveldb_blockchain ldb_chain(pool);
    chain = &ldb_chain;
    ldb_chain.start(blockchain_path, blockchain_started);
    pool.shutdown();
    pool.join();
    ldb_chain.stop();
    return 0;
}


