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
    async_service service(10);
    blockchain_ptr chain =
        kyoto_blockchain::create(service, "kydb", blockchain_started);
    std::cin.get();
    return 0;
}

