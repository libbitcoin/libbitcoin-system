#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

typedef std::pair<hash_digest, hash_digest> hash_pair;
typedef std::vector<hash_pair> hash_pair_list;

class kernel 
  : public threaded_service,
    private boost::noncopyable, 
    public std::enable_shared_from_this<kernel>
{
public:
    kernel();

    void register_network(network_ptr net_comp);
    network_ptr get_network();

    void connect(std::string hostname, unsigned int port);

    void receive_inv(channel_handle chandle, const message::inv& packet);
    void receive_block(channel_handle chandle, const message::block& packet);

    void register_storage(storage_ptr stor_comp);
    storage_ptr get_storage();
    void tween_blocks(const hash_pair_list& block_hashes);

private:
    void handle_connect(const std::error_code& ec, channel_handle chandle);
    void handle_block_stored(const std::error_code& ec, block_status status,
        hash_digest block_hash);

    void start_initial_getblocks(channel_handle chandle);
    void request_initial_blocks(const std::error_code& ec,
        const message::block_locator& locator, channel_handle chandle);
    void request_next_blocks(const std::error_code& ec,
        const message::block_locator& locator, 
        const hash_pair_list& block_hashes);

    network_ptr network_component_;
    storage_ptr storage_component_;

    bool initial_getblocks_;
    std::multimap<hash_digest, channel_handle> inventory_tracker_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

