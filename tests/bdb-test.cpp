#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/transaction_pool.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/handshake.hpp>
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;

bool stop = false;

std::mutex mutex;
std::condition_variable condition;
bool finished = false;

network_ptr net = std::make_shared<network>();
handshake_ptr hs = std::make_shared<handshake>();

void handle_send_packet(const std::error_code& ec)
{
    if (ec)
        log_error() << ec.message();
}

void ask_blocks2(const std::error_code& ec, channel_ptr node,
    const message::block_locator& loc, blockchain_ptr chain,
    const hash_digest& hash_stop)
{
    if (ec)
        log_error() << ec.message();
    message::get_blocks packet;
    packet.locator_start_hashes = loc;
    packet.hash_stop = hash_stop;
    node->send(packet, std::bind(&handle_send_packet, _1));
}

void show_block(const std::error_code& ec, const message::block& blk)
{
    if (ec)
    {
        log_error() << "show_block " << ec.message();
        return;
    }
    log_debug() << "Fetch";
    log_debug() << pretty_hex(blk.merkle);
    log_debug() << blk.transactions.size();
    log_debug() << pretty_hex(hash_transaction(blk.transactions[0]));
}

transaction_pool_ptr tx_pool;
message::transaction tx;
void test_mem_pool(const message::block& blk)
{
    log_debug() << "num tx +" << blk.transactions.size();
    tx = blk.transactions[1];
}

void handle_txpl(const std::error_code& ec)
{
    if (ec)
    {
        log_error() << "tx pool: " << ec.message();
        return;
    }
    log_debug() << "fin";
    sleep(4);
    std::unique_lock<std::mutex> lock(mutex);
    finished = true;
    condition.notify_one();
}

void show_tx(const std::error_code& ec, const message::transaction& tx,
    size_t parent_depth, size_t index)
{
    if (ec)
    {
        log_error() << "showTx: " << ec.message();
        return;
    }
    log_debug() << "tx depth is " << parent_depth << " w idx " << index;
    log_debug() << pretty(tx);
}

void handle_store(const std::error_code& ec, block_info info,
    channel_ptr node, blockchain_ptr chain, const hash_digest& block_hash)
{
    if (ec)
    {
        log_error() << "handle store " << ec.message() << " for " << pretty_hex(block_hash);
        return;
    }
    else if (info.status != block_status::confirmed)
    {
        log_debug() << "block not added";
        switch (info.status)
        {
            case block_status::orphan:
                log_debug() << "orphan";
                chain->fetch_block_locator(std::bind(ask_blocks2, _1, node, _2, chain, block_hash));
                break;

            case block_status::rejected:
                log_debug() << "bad: " << pretty_hex(block_hash);
                exit(0);
                break;
        }
    }
    log_debug() << "added " << info.depth;
    if (info.depth == 170 - 1)
    {
        log_debug() << "proc tx";
        //tx.outputs[0].value += coin_price(1);
        tx_pool->store(tx, handle_txpl);
    }
    else if (info.depth == 20)
    {
        chain->fetch_transaction(
            hash_digest{0xdf, 0x2b, 0x06, 0x0f, 0xa2, 0xe5, 0xe9, 0xc8, 
                        0xed, 0x5e, 0xaf, 0x6a, 0x45, 0xc1, 0x37, 0x53, 
                        0xec, 0x8c, 0x63, 0x28, 0x2b, 0x26, 0x88, 0x32, 
                        0x2e, 0xba, 0x40, 0xcd, 0x98, 0xea, 0x06, 0x7a},
            show_tx);
    }
    if (info.depth == 400)
    {
        chain->fetch_block(
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x83, 0x9a, 0x8e, 0x68,
                        0x86, 0xab, 0x59, 0x51, 0xd7, 0x6f, 0x41, 0x14,
                        0x75, 0x42, 0x8a, 0xfc, 0x90, 0x94, 0x7e, 0xe3,
                        0x20, 0x16, 0x1b, 0xbf, 0x18, 0xeb, 0x60, 0x48},
            show_block);
    }
    if (stop)
    {
        std::unique_lock<std::mutex> lock(mutex);
        finished = true;
        condition.notify_one();
    }
}

void recv_blk(const std::error_code& ec, const message::block& packet,
    channel_ptr node, blockchain_ptr chain)
{
    static bool stop_inserts = false;
    if (ec)
        log_error() << ec.message();
    node->subscribe_block(std::bind(recv_blk, _1, _2, node, chain));
    // store block in bdb
    if (hash_block_header(packet) ==
        hash_digest{0x00, 0x00, 0x00, 0x00, 0xd1, 0x14, 0x57, 0x90,
                    0xa8, 0x69, 0x44, 0x03, 0xd4, 0x06, 0x3f, 0x32,
                    0x3d, 0x49, 0x9e, 0x65, 0x5c, 0x83, 0x42, 0x68,
                    0x34, 0xd4, 0xce, 0x2f, 0x8d, 0xd4, 0xa2, 0xee})
    {
        test_mem_pool(packet);
        stop_inserts = true;
    }
    else if (!stop_inserts)
        chain->store(packet, std::bind(handle_store, _1, _2, node, chain, hash_block_header(packet)));
}

void recv_inv(const std::error_code &ec, const message::inventory& packet,
    channel_ptr node)
{
    if (ec)
        log_error() << ec.message();
    message::get_data getdata;
    for (const message::inventory_vector& ivv: packet.inventories)
    {
        if (ivv.type != message::inventory_type::block)
            continue;
        getdata.inventories.push_back(ivv);
    }
    node->send(getdata, handle_send_packet);
    node->subscribe_inventory(std::bind(&recv_inv, _1, _2, node));
}

void ask_blocks(const std::error_code& ec, channel_ptr node,
    const message::block_locator& loc, blockchain_ptr chain,
    const hash_digest& hash_stop)
{
    if (ec)
        log_error() << ec.message();
    node->subscribe_inventory(std::bind(recv_inv, _1, _2, node));
    node->subscribe_block(std::bind(recv_blk, _1, _2, node, chain));

    message::get_blocks packet;
    packet.locator_start_hashes = loc;
    packet.hash_stop = hash_stop;
    node->send(packet, std::bind(&handle_send_packet, _1));
}

void recv_loc(const std::error_code& ec, const message::block_locator& loc,
    blockchain_ptr chain)
{
    hs->connect(net, "localhost", 8333,
        std::bind(&ask_blocks, _1, _2, loc, chain, null_hash));
}

int main()
{
    bdb_blockchain::setup("database/");
    log_debug() << "Setup finished";
    blockchain_ptr store(new bdb_blockchain("database/"));
    log_debug() << "Opened";
    store->fetch_block(0, show_block);
    store->fetch_block(
        hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                    0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                    0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                    0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f},
        show_block);
    tx_pool = transaction_pool::create(store);
    store->fetch_block_locator(std::bind(recv_loc, _1, _2, store));

    //std::cin.get();
    //stop = true;
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return finished; });
    return 0;
}

