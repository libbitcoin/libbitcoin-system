#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/network/network.hpp>
using namespace libbitcoin;

std::mutex mutex;
std::condition_variable condition;
bool finished = false;

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
                log_debug() << "bad";
                exit(0);
                break;
        }
    }
    log_debug() << "added " << info.depth;
    if (info.depth == 400)
    {
        chain->fetch_block(
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x83, 0x9a, 0x8e, 0x68,
                        0x86, 0xab, 0x59, 0x51, 0xd7, 0x6f, 0x41, 0x14,
                        0x75, 0x42, 0x8a, 0xfc, 0x90, 0x94, 0x7e, 0xe3,
                        0x20, 0x16, 0x1b, 0xbf, 0x18, 0xeb, 0x60, 0x48},
            show_block);

        //std::unique_lock<std::mutex> lock(mutex);
        //finished = true;
        //condition.notify_one();
    }
}

void recv_blk(const std::error_code& ec, const message::block& packet,
    channel_ptr node, blockchain_ptr chain)
{
    if (ec)
        log_error() << ec.message();
    node->subscribe_block(std::bind(recv_blk, _1, _2, node, chain));
    // store block in bdb
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
    network_ptr net(new network);
    handshake_connect(net, "localhost", 8333,
        std::bind(&ask_blocks, _1, _2, loc, chain, null_hash));
}

int main()
{
    //bdb_blockchain::setup("database/");
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
    store->fetch_block_locator(std::bind(recv_loc, _1, _2, store));

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return finished; });
    return 0;
}

