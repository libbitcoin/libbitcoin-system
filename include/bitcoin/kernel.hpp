#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/util/threads.hpp>

namespace libbitcoin {

typedef std::pair<hash_digest, hash_digest> hash_pair;
typedef std::vector<hash_pair> hash_pair_list;

class kernel 
 : public threaded_service, public std::enable_shared_from_this<kernel>
{
public:
    kernel();

    kernel(const kernel&) = delete;
    void operator=(const kernel&) = delete;

    void register_network(network_ptr net_comp);
    network_ptr get_network();

    void connect(std::string hostname, uint16_t port);

    void receive_inventory(const std::error_code& ec,
        const message::inventory& packet, channel_ptr node);
    void receive_block(const std::error_code& ec,
        const message::block& packet, channel_ptr node);

    void register_blockchain(blockchain_ptr stor_comp);
    blockchain_ptr get_blockchain();
    void tween_blocks(const hash_pair_list& block_hashes);

private:
    void handle_connect(const std::error_code& ec, channel_ptr node);
    void handle_block_stored(const std::error_code& ec, block_info info,
        hash_digest block_hash);

    void start_initial_getblocks(channel_ptr node);
    void request_initial_blocks(const std::error_code& ec,
        const message::block_locator& locator, channel_ptr node);
    void request_next_blocks(const std::error_code& ec,
        const message::block_locator& locator, 
        const hash_pair_list& block_hashes);

    network_ptr network_component_;
    blockchain_ptr blockchain_component_;

    bool initial_getblocks_;
    std::multimap<hash_digest, channel_ptr> inventory_tracker_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

