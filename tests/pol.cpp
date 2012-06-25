#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;

void start_polling(const std::error_code& ec, channel_ptr node,
    poller& poll)
{
    if (ec)
    {
        log_fatal() << "error: " << ec.message();
        return;
    }
    poll.query(node);
    poll.monitor(node);
}

void blockchain_started(const std::error_code& ec, blockchain*)
{
    if (ec)
        log_fatal() << "error: " << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

int main()
{
    //bdb_blockchain::setup("database");
    async_service s1(1), s2(1);
    bdb_blockchain chain(s1);
    chain.start("database", blockchain_started);
    poller poll(s2, chain);
    network_ptr net = std::make_shared<network>(s2);
    handshake_ptr hs = std::make_shared<handshake>(s2);
    connect(hs, net, "localhost", 8333,
        std::bind(start_polling, _1, _2, std::ref(poll)));
    std::cin.get();
    return 0;
}

