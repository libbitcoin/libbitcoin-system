#include <bitcoin/bitcoin.hpp>
using namespace bc;

#include <bitcoin/blockchain/bdb_blockchain.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

void handle_store(const std::error_code& ec)
{
    if (ec)
        log_error() << "Error: " << ec.message();
    else
        log_debug() << "Added";
}

void start_polling(const std::error_code& ec, channel_ptr node,
    poller_ptr poll)
{
    if (ec)
    {
        log_fatal() << "error: " << ec.message();
        return;
    }
    poll->query(node);
}

int main()
{
    std::string arg1 = "01000000038294ae37c2da6b5743338bdc12d428fa7762b3e33b0389d7645df49627fcb279230000008c493046022100da386a835b49c6576b4f643f2c95ed811a8e398a072b850879c2cdbbac14a255022100f96a3f7ffd661d56b2aa151ed0133abbf30b754eb3b05af7e957e64920c5b461014104469a8a569f856550f7a59aabc59ea34b5c8524e0f3fe3e6a1023a93ac9671c3aa869239cc756c38223a4289938da85ad5cef63b018bbae3d3843ff834bd2817affffffffe4ee4f8923715133a90833bf27bec61bc7516970fe5ab7a3d3e1a6df928220821b0000008b483045022100b03b3591683110514b523e85bfb77ce3e4e16d1a71257a67a5fb554682b3ca7702207f325e4b7a7dd9b88eea348aa72492f567404f1a66f5dbdb4f9ac366a8d6054c0141046e998d60e8b2c53a68a6924e2e2e4ec20d7b8745b75bbb1efae15b9ee9beda6c1a5b2786aea148beba13b7e83f446e6757c840b5aac2715c2aecb567ad6ae866ffffffffe643cfd7af52048b6ad7cfad0d2c10388fa17920e80654507c0f898ccfe99fad000000008b4830450220165d34eb1168fa5c6cb28cefec0dce85dc6039dbe142fdbbda8d4ab6955a13a8022100a2a90260c1ab855bdd78c322ab260d6cdd70d2a4e94294d1e192ac85bccfd8be014104658016be5607bb3ce4b5b3fa241d40d215a408e2426a7494217522a197876e4438df967cba545e1715dafa5ff9cfcf23975cc1fb2f08a22126c34628a08f1efcffffffff01404b4c00000000001976a914c70b227fef187ecd2273c34dce7898d8ce166ce288ac00000000";
    satoshi_exporter ex;
    auto tx = ex.load_transaction(bytes_from_pretty(arg1));
    log_info() << pretty_hex(hash_transaction(tx));

    async_service service(2);
    auto chain = std::make_shared<bdb_blockchain>(service, "database");
    auto tx_pool = transaction_pool::create(service, chain);
    tx_pool->store(tx, handle_store);
    std::cin.get();

    async_service s2(1);
    poller_ptr poll = std::make_shared<poller>(chain);
    network_ptr net = std::make_shared<network>(s2);
    handshake_ptr hs = std::make_shared<handshake>(s2);
    hs->connect(net, "localhost", 8333,
        std::bind(start_polling, _1, _2, poll));
    std::cin.get();
    return 0;
}

