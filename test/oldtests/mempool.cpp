#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;

void handle_confirm(const std::error_code& ec)
{
    if (ec)
        log_error() << "Error: " << ec.message();
    else
        log_debug() << "Added";
}

void handle_store(const std::error_code& ec, const index_list& unconfirmed)
{
    if (ec)
    {
        log_error()
            << "Error storing memory pool transaction " << ec.message();
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
    }
}

transaction_pool* txpool;

void receive_dat(const std::error_code& ec, const message::transaction& tx,
    channel_ptr node)
{
    log_debug() << "got " << hash_transaction(tx);
    node->subscribe_transaction(std::bind(&receive_dat, _1, _2, node));
    txpool->store(tx, handle_confirm, handle_store);
}

void receive_inv(const std::error_code &ec, const message::inventory& packet,
    channel_ptr node)
{
    log_info() << "Received:";
    for (const message::inventory_vector& ivv: packet.inventories)
    {
        if (ivv.type != message::inventory_type::transaction)
            log_info() << "  --";
        else
            log_info() << "  " << encode_hex(ivv.hash);
    }
    message::get_data getdat;
    getdat.inventories = packet.inventories;
    node->send(getdat, [](const std::error_code&) {});
    node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));
}

void connected(const std::error_code& ec, channel_ptr node)
{
    if (ec)
        log_error() << ec.message();
    node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));
    node->subscribe_transaction(std::bind(&receive_dat, _1, _2, node));
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        log_fatal() << "error: " << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

int main()
{
    threadpool pool(1), chain_pool(1);
    network net(pool);
    handshake hs(pool);
    connect(hs, net, "localhost", 8333,
        std::bind(connected, _1, _2));

    bdb_blockchain chain(chain_pool);
    chain.start("database", blockchain_started);

    txpool = new transaction_pool(pool, chain);
    txpool->start();

    std::cin.get();

    pool.stop();
    chain_pool.stop();
    pool.join();
    chain_pool.join();

    chain.stop();

    return 0;
}

