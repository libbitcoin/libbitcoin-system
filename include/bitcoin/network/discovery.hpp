#ifndef LIBBITCOIN_NET_DISCOVERY_H
#define LIBBITCOIN_NET_DISCOVERY_H

#include <bitcoin/types.hpp>
#include <bitcoin/network/channel.hpp>
#include <boost/random.hpp>

namespace libbitcoin {

class discovery
 : public std::enable_shared_from_this<discovery>
{
    typedef std::function<void (const std::error_code&, const std::string hostname)> irc_handler;

public:
    discovery();
    ~discovery();

    void irc_discovery(irc_handler handler);

private:
    void resolve_handler(const boost::system::error_code& ec,
                  tcp::resolver::iterator endpoint_iterator,
                  irc_handler handler);
    void irc_connect(const boost::system::error_code& ec, irc_handler handler);
    void irc_readline(const boost::system::error_code& ec, size_t len,
        irc_handler handler);
    void irc_identify();
    void irc_join();
    void send_raw_line(const std::string& message);
    void handle_send(const boost::system::error_code& ec);

    socket_ptr socket_;
    boost::asio::streambuf data_;
    std::string ircchan_;
    std::string ircnick_;
    std::string ircmyhost_;
    boost::mt19937 rng_;

    thread_core_ptr threaded_;
    strand_ptr strand_;
};

}

#endif
