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

