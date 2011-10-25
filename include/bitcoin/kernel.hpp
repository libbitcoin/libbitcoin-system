#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

class kernel 
  : public threaded_service,
    private boost::noncopyable, 
    public std::enable_shared_from_this<kernel>
{
public:
    kernel();

    void register_network(network_ptr net_comp);
    network_ptr get_network();
    // Callbacks from network component
    void send_failed(channel_handle chandle, const message::version& message);
    void send_failed(channel_handle chandle, const message::verack& message);
    void send_failed(channel_handle chandle, const message::getaddr& message);
    void send_failed(channel_handle chandle, const message::inv& message);
    void send_failed(channel_handle chandle, const message::getdata& message);
    void send_failed(channel_handle chandle, const message::getblocks& message);

    bool recv_message(channel_handle chandle, const message::version& message);
    bool recv_message(channel_handle chandle, const message::verack& message);
    bool recv_message(channel_handle chandle, const message::addr& message);
    bool recv_message(channel_handle chandle, const message::inv& message);
    bool recv_message(channel_handle chandle, const message::block& message);

    void handle_connect(channel_handle chandle);

    void register_storage(storage_ptr stor_comp);
    storage_ptr get_storage();
    void tween_blocks(const hash_list& block_hashes);

private:
    void start_initial_getblocks(channel_handle chandle);
    void request_initial_blocks(const std::error_code& ec,
        const message::block_locator& locator, channel_handle chandle);
    void request_next_blocks(const std::error_code& ec,
        const message::block_locator& locator, const hash_list& block_hashes);

    network_ptr network_component_;
    storage_ptr storage_component_;

    bool initial_getblocks_;
    std::multimap<hash_digest, channel_handle> inventory_tracker_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

