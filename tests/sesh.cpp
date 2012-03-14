#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

#include <bitcoin/blockchain/bdb_blockchain.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "session: " << message;
    exit(status);
}

void handle_start(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

void handle_stop(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    exit(0);
}

session_params p;

void recv_transaction(const std::error_code& ec,
    const message::transaction& tx, channel_ptr node);
void monitor_tx(channel_ptr node);
void handle_mempool_store(const std::error_code& ec,
    const hash_digest& tx_hash);

void monitor_tx(channel_ptr node)
{
    node->subscribe_transaction(std::bind(&recv_transaction, _1, _2, node));
    p.protocol->subscribe_channel(monitor_tx);
}

void recv_transaction(const std::error_code& ec,
    const message::transaction& tx, channel_ptr node)
{
    if (ec)
    {
        log_error() << "transaction: " << ec.message();
        return;
    }
    p.transaction_pool->store(tx,
        std::bind(&handle_mempool_store, _1, hash_transaction(tx)));
    node->subscribe_transaction(std::bind(recv_transaction, _1, _2, node));
}

void handle_mempool_store(const std::error_code& ec,
    const hash_digest& tx_hash)
{
    if (ec)
    {
        log_error()
            << "Error storing memory pool transaction "
            << pretty_hex(tx_hash) << ": " << ec.message();
    }
    else
    {
        log_info(log_domain::session)
            << "Accepted transaction " << pretty_hex(tx_hash);
    }
}

int main()
{
    async_service network_service_(1), disk_service_(1), mempool_service_(1);
    p.hosts = std::make_shared<hosts>(network_service_);
    p.handshake = std::make_shared<handshake>(network_service_);
    p.network = std::make_shared<network>(network_service_);
    p.protocol = std::make_shared<protocol>(
        network_service_, p.hosts, p.handshake, p.network);
    p.protocol->subscribe_channel(monitor_tx);

    p.blockchain = std::make_shared<bdb_blockchain>(disk_service_, "database");
    p.poller = std::make_shared<poller>(p.blockchain);

    p.transaction_pool = transaction_pool::create(mempool_service_, p.blockchain);

    //bdb_blockchain::setup("database");
    session s(p);
    s.start(handle_start);
    std::cin.get();
    s.stop(handle_stop);
    log_debug() << "Exiting...";
    while (1)
        sleep(100);
    return 0;
}

