#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

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
}

void handle_confirm(const std::error_code& ec)
{
    if (ec)
        log_error() << "Confirm error: " << ec.message();
    else
        log_error() << "Confirmed.";
}

session_params* p;

void recv_transaction(const std::error_code& ec,
    const message::transaction& tx, channel_ptr node);
void monitor_tx(channel_ptr node);
void handle_mempool_store(
    const std::error_code& ec, const index_list& unconfirmed,
    const message::transaction& tx, channel_ptr node);

void monitor_tx(channel_ptr node)
{
    node->subscribe_transaction(std::bind(&recv_transaction, _1, _2, node));
    p->protocol_.subscribe_channel(monitor_tx);
}

void recv_transaction(const std::error_code& ec,
    const message::transaction& tx, channel_ptr node)
{
    if (ec)
    {
        log_error() << "transaction: " << ec.message();
        return;
    }
    p->transaction_pool_.store(tx, handle_confirm,
        std::bind(&handle_mempool_store, _1, _2, tx, node));
    node->subscribe_transaction(std::bind(recv_transaction, _1, _2, node));
}

void depends_requested(const std::error_code& ec)
{
    if (ec)
        log_error() << "depends_requested: " << ec.message();
}

void handle_mempool_store(
    const std::error_code& ec, const index_list& unconfirmed,
    const message::transaction& tx, channel_ptr node)
{
    const hash_digest& tx_hash = hash_transaction(tx);
    // Decided against this. Spammers can abuse us more easily.
    /*if (ec == error::input_not_found)
    {
        BITCOIN_ASSERT(unconfirmed.size() == 1);
        BITCOIN_ASSERT(unconfirmed[0] < tx.inputs.size());
        const auto& prevout = tx.inputs[unconfirmed[0]].previous_output;
        log_info() << "Requesting dependency " << pretty_hex(prevout.hash)
            << " for " << pretty_hex(tx_hash);
        message::get_data getdat;
        getdat.inventories.push_back(
            {message::inventory_type::transaction, prevout.hash});
        //getdat.inventories.push_back(
        //    {message::inventory_type::transaction, tx_hash});
        node->send(getdat, depends_requested);
    }
    else if (ec)*/
    if (ec)
    {
        //BITCOIN_ASSERT(unconfirmed.size() == 0);
        log_error()
            << "Error storing memory pool transaction "
            << pretty_hex(tx_hash) << ": " << ec.message();
    }
    else
    {
        auto l = log_info();
        l << "Accepted transaction ";
        if (!unconfirmed.empty())
        {
            l << "(Unconfirmed inputs";
            for (auto idx: unconfirmed)
                l << " " << idx;
            l << ") ";
        }
        l << pretty_hex(tx_hash);
    }
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    else
        log_info() << "Blockchain initialized!";
}

int main()
{
    async_service network_service(1), disk_service(1), mempool_service(1);
    hosts hsts(network_service);
    handshake hs(network_service);
    network net(network_service);
    protocol prot(network_service, hsts, hs, net);
    prot.subscribe_channel(monitor_tx);

    bdb_blockchain chain(disk_service);
    chain.start("database", blockchain_started);
    poller poll(mempool_service, chain);

    transaction_pool txpool(mempool_service, chain);
    txpool.start();

    p = new session_params{hs, prot, chain, poll, txpool};
    session sesh(network_service, *p);
    sesh.start(handle_start);

    std::cin.get();

    sesh.stop(handle_stop);

    network_service.stop();
    disk_service.stop();
    mempool_service.stop();
    network_service.join();
    disk_service.join();
    mempool_service.join();

    chain.stop();
    log_debug() << "Exiting...";

    return 0;
}

