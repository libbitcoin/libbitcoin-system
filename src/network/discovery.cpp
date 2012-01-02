#include <bitcoin/network/discovery.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

namespace libbitcoin {

discovery::discovery()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
}

discovery::~discovery()
{
}

void discovery::clear()
{
    addresses_.clear();
}

uint16_t discovery::count()
{
    return addresses_.size();
}

void discovery::irc_discovery(node_address ircserv, discovery_handler handler)
{
    socket_ptr socket(new tcp::socket(*threaded_->service()));
    tcp::resolver resolver(*threaded_->service());
    tcp::resolver::query query(ircserv.hostname,
        boost::lexical_cast<std::string>(ircserv.port));
    tcp::endpoint endpoint = *resolver.resolve(query);
    socket->async_connect(endpoint,
                          std::bind(&discovery::irc_handler, shared_from_this(),
                              _1, ircserv, socket));

}

void discovery::irc_handler(const boost::system::error_code& ec,
                                node_address ircserv, socket_ptr socket)
{
    std::string str;

    if (ec)
    {
        log_error() << "Connecting to IRC server " << ircserv.hostname
            << ": " << ec.message();
        //handle_connect(error::system_network_error, nullptr);
        return;
    }

    read(*socket, boost::asio::buffer(talk_buffer_));

    log_debug() << "IRC sayz:";
    log_debug() << talk_buffer_.data();

    boost::mt19937 rng;
    boost::uniform_int<> dist(1,1000000000);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        die(rng, dist);
    uint32_t roll = die();

    str = "NICK " + boost::lexical_cast<std::string>(roll);
    write(*socket, boost::asio::buffer(str));
    str = "USER " + boost::lexical_cast<std::string>(roll) + " 8 * : "
        + boost::lexical_cast<std::string>(roll) + "\n";



    //write(SyncWriteStream & s, const ConstBufferSequence & buffers);
}

}
