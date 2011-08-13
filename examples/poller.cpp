#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/net/network.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/postbind.hpp>

using namespace libbitcoin;
using std::placeholders::_1;
using std::placeholders::_2;

using boost::asio::deadline_timer;
using boost::posix_time::seconds;
typedef std::shared_ptr<deadline_timer> deadline_timer_ptr;

class poller_application
  : public threaded_service,
    public std::enable_shared_from_this<poller_application>
{
public:
    poller_application();

    void start();
private:
    typedef std::vector<net::channel_handle> channels_list;

    void handle_connect(std::error_code ec, net::channel_handle channel);
    void reset_timer();

    void fetch_locator(const boost::system::error_code& ec);
    void request_blocks(std::error_code ec, message::block_locator locator);

    kernel_ptr kernel_;
    network_ptr network_;
    storage_ptr storage_;

    deadline_timer_ptr poll_blocks_timer_;
    channels_list channels_;
};

typedef std::shared_ptr<poller_application> poller_application_ptr;

poller_application::poller_application()
  : kernel_(new kernel)
{
    network_.reset(new net::network_impl(kernel_));
    kernel_->register_network(network_);

    storage_.reset(new postgresql_storage("bitcoin", "genjix"));
    kernel_->register_storage(storage_);

    poll_blocks_timer_.reset(new deadline_timer(*service()));
}

void poller_application::start()
{
    network_->connect("localhost", 8333, 
        postbind<std::error_code, net::channel_handle>(strand(), std::bind(
            &poller_application::handle_connect, shared_from_this(), _1, _2)));
}

void poller_application::handle_connect(
    std::error_code ec, net::channel_handle channel)
{
    if (ec)
    {
        log_error() << "Connect: " << ec.message();
        return;
    }
    reset_timer();
    channels_.push_back(channel);
}

void poller_application::reset_timer()
{
    poll_blocks_timer_->cancel();
    poll_blocks_timer_->expires_from_now(seconds(5));
    poll_blocks_timer_->async_wait(
        postbind<const boost::system::error_code>(strand(), std::bind(
            &poller_application::fetch_locator, shared_from_this(), _1)));
}

void poller_application::fetch_locator(const boost::system::error_code& ec)
{
    if (ec)
    {
        log_error() << "Poll timer: " << ec.message();
        return;
    }
    storage_->organize_block_chain();
    storage_->fetch_block_locator(
        postbind<std::error_code, message::block_locator>(strand(), std::bind(
            &poller_application::request_blocks, shared_from_this(), _1, _2)));
}

void poller_application::request_blocks(
        std::error_code ec, message::block_locator locator)
{
    if (ec)
    {
        log_error() << "Block locator: " << ec.message();
        return;
    }
    message::getblocks getblocks;
    getblocks.locator_start_hashes = locator;
    getblocks.hash_stop = null_hash;
    // Do a broadcast
    for (net::channel_handle channel: channels_)
        network_->send(channel, getblocks);
    reset_timer();
}

int main()
{
    poller_application_ptr app(new poller_application);
    app->start();
    std::cin.get();
    return 0;
}

