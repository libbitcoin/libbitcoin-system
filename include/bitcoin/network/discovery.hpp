#ifndef LIBBITCOIN_NET_DISCOVERY_H
#define LIBBITCOIN_NET_DISCOVERY_H

#include <bitcoin/types.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class discovery
 : public std::enable_shared_from_this<discovery>
{
    typedef std::function<void (const std::error_code&)> discovery_handler;

public:
    discovery();
    ~discovery();

    void clear();
    uint16_t count();
    void irc_discovery(node_address ircserv, discovery_handler handler);
    void irc_handler(const boost::system::error_code& ec, node_address ircserv,
                         socket_ptr socket);

private:
    boost::array<uint8_t, 500> talk_buffer_;
    std::vector<node_address> addresses_;

    thread_core_ptr threaded_;
    strand_ptr strand_;
};

}

#endif
