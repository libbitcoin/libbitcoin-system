#include <bitcoin/network/network.hpp>
#include <bitcoin/network/discovery.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

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

void discovery::irc_discovery(node_address ircserv, error_handler handler)
{
    socket_ptr socket(new tcp::socket(*threaded_->service()));
    resolver_ptr res = std::make_shared<tcp::resolver>(*threaded_->service());
    log_debug() << "Querying resolver...";
    query_ptr query = std::make_shared<tcp::resolver::query>(ircserv.hostname,
                           boost::lexical_cast<std::string>(ircserv.port));
    res->async_resolve(*query, std::bind(&discovery::resolve_handler,
                           shared_from_this(), _1, _2, ircserv, handler));

}

void discovery::irc_handler(const boost::system::error_code& ec,
                                node_address ircserv)
{
    std::string str;

    if (ec)
    {
        log_error() << "Connecting to IRC server " << ircserv.hostname
            << ": " << ec.message();
        return;
    }

    boost::array<char, 128> buf;
    size_t len;
    boost::system::error_code error;
    log_debug() << "IRC sayz:";
    while(len = socket_->read_some(boost::asio::buffer(buf), error))
        std::cout.write(buf.data(), len);

    /*read(*socket, boost::asio::buffer(talk_buffer_));

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
        + boost::lexical_cast<std::string>(roll) + "\n";*/

}

void discovery::resolve_handler(const boost::system::error_code& ec,
    tcp::resolver::iterator endpoint_iterator,
	libbitcoin::node_address ircserv, error_handler handler)
{
    if (ec)
    {
        handler(error::resolve_failed);
        return;
    }
    socket_ = std::make_shared<tcp::socket>(*threaded_->service());
    boost::asio::async_connect(*socket_, endpoint_iterator,
        std::bind(&discovery::irc_handler, shared_from_this(),
        _1, ircserv));

}

}
