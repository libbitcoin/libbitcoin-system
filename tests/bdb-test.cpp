#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/network/network.hpp>
using namespace libbitcoin;

void show_block(const std::error_code& ec, const message::block& blk)
{
    log_debug() << "Fetch";
    log_debug() << pretty_hex(blk.merkle);
    log_debug() << blk.transactions.size();
    log_debug() << pretty_hex(hash_transaction(blk.transactions[0]));
}

void receive_inv(const std::error_code &ec, const message::inventory& packet,
    channel_ptr node)
{
    log_info() << "Received:";
    for (const message::inventory_vector& ivv: packet.inventories)
    {
        if (ivv.type != message::inventory_type::block)
            log_info() << "  --";
        else
            log_info() << "  " << pretty_hex(ivv.hash);
    }
    node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));
}

void handle_send_getblock(const std::error_code& ec)
{
    if (ec)
        log_error() << ec.message();
}

void ask_blocks(const std::error_code& ec, channel_ptr node,
    const message::block_locator& loc)
{
    //node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));

    //message::get_blocks packet;
    //packet.locator_start_hashes = loc;
    //packet.hash_stop.fill(0);
    //node->send(packet, std::bind(&handle_send_getblock, _1));
}

void recv_loc(const std::error_code& ec, const message::block_locator& loc)
{
    log_debug() << "LOC";
    for (hash_digest h: loc)
        log_debug() << pretty_hex(h);
    network_ptr net(new network);
    handshake_connect(net, "localhost", 8333,
        std::bind(&ask_blocks, _1, _2, loc));
}

int main()
{
    bdb_blockchain::setup("database/");
    log_debug() << "Setup finished";
    shared_ptr<bdb_blockchain> store(new bdb_blockchain("database/"));
    log_debug() << "Opened";
    store->fetch_block(0, show_block);
    store->fetch_block(
        hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                    0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                    0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                    0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f},
        show_block);
    store->fetch_block_locator(recv_loc);
    std::cin.get();
    return 0;
}

