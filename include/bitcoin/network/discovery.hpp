#ifndef LIBBITCOIN_NET_DISCOVERY_H
#define LIBBITCOIN_NET_DISCOVERY_H

#include <bitcoin/types.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class discovery
 : public std::enable_shared_from_this<discovery>
{
    typedef std::function<void (const std::error_code&)> error_handler;

public:
    discovery();
    ~discovery();

    void clear();
    uint16_t count();
    void irc_discovery(error_handler handler);
    void resolve_handler(const boost::system::error_code& ec,
                  tcp::resolver::iterator endpoint_iterator,
                  error_handler handler);
    void irc_connect(const boost::system::error_code& ec);
    void irc_readline(const boost::system::error_code& ec, size_t len);
    void irc_identify();
    void irc_join();
    void send_raw_line(const std::string& message);
    void handle_send(const boost::system::error_code& ec);

private:
    boost::array<uint8_t, 500> talk_buffer_;
    std::vector<node_address> addresses_;
    socket_ptr socket_;
    boost::asio::streambuf data_;

    thread_core_ptr threaded_;
    strand_ptr strand_;
};

}

#endif
