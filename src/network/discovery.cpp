#include <bitcoin/network/network.hpp>
#include <bitcoin/network/discovery.hpp>
#include <boost/lexical_cast.hpp>
#include <ctime>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

data_chunk as_chunk(const std::string& s)
{
    return data_chunk(s.begin(), s.end());
}

discovery::discovery()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
    rng_.seed(static_cast<unsigned int>(std::time(0)));
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

void discovery::irc_discovery(error_handler handler)
{
    resolver_ptr resolver =
        std::make_shared<tcp::resolver>(*threaded_->service());
    query_ptr query = std::make_shared<tcp::resolver::query>("irc.lfnet.org",
        boost::lexical_cast<std::string>(6667));
    resolver->async_resolve(*query,
        std::bind(&discovery::resolve_handler, shared_from_this(),
        _1, _2, handler));
}

void discovery::resolve_handler(const boost::system::error_code& ec,
    tcp::resolver::iterator endpoint_iterator, error_handler handler)
{
    if (ec)
    {
        handler(error::resolve_failed);
        return;
    }
    socket_ = std::make_shared<tcp::socket>(*threaded_->service());
    boost::asio::async_connect(*socket_, endpoint_iterator,
        std::bind(&discovery::irc_connect, shared_from_this(), _1));

}

void discovery::irc_connect(const boost::system::error_code& ec)
{
    if (ec)
    {
        log_error() << "Connecting to IRC server: " << ec.message();
        return;
    }

    boost::asio::async_read_until(*socket_, data_, "\r\n",
        std::bind(&discovery::irc_readline, shared_from_this(), _1, _2));

    irc_identify();
}

void discovery::irc_readline(const boost::system::error_code& ec, size_t len)
{
    if (ec)
    {
//        handler(error::resolve_failed);
        return;
    }
    std::ostringstream oss;
    oss << &data_;
    const std::string line = oss.str();
    log_debug() << line;
    if (line.find(" 433 ") != std::string::npos)
        irc_identify();

    if (line.find(" 001 ") != std::string::npos)
        irc_join();

    boost::asio::async_read_until(*socket_, data_, "\r\n",
        std::bind(&discovery::irc_readline, shared_from_this(), _1, _2));
}

void discovery::irc_identify()
{
    std::string str;
    boost::uniform_int<> dist(1,1000000000);
    uint32_t roll = dist(rng_);

    str = "NICK lbtc" + boost::lexical_cast<std::string>(roll);
    send_raw_line(str);

    str = "USER x" + boost::lexical_cast<std::string>(roll) + " 8 * :lbtc"
        + boost::lexical_cast<std::string>(roll);
    send_raw_line(str);
}

void discovery::irc_join()
{
    std::string str;
    boost::uniform_int<> dist(0,99);
    uint32_t roll = dist(rng_);

    str = ":source JOIN :#bitcoin";
    if (roll < 10)
        str += "0";
    str += boost::lexical_cast<std::string>(roll);

    send_raw_line(str);
}

void discovery::send_raw_line(const std::string& message)
{
    shared_const_buffer buff(as_chunk(message + "\r\n"));
    async_write(*socket_, buff,
        std::bind(&discovery::handle_send, shared_from_this(), _1));
}

void discovery::handle_send(const boost::system::error_code& ec)
{
//    if (ec)
//        handler(ec.message());
}

}
