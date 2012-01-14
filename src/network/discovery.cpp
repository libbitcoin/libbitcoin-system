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

void discovery::irc_discovery(irc_handler handler)
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
    tcp::resolver::iterator endpoint_iterator, irc_handler handler)
{
    if (ec)
    {
        handler(error::resolve_failed);
        return;
    }
    socket_ = std::make_shared<tcp::socket>(*threaded_->service());
    boost::asio::async_connect(*socket_, endpoint_iterator,
        std::bind(&discovery::irc_connect, shared_from_this(), _1, handler));

}

void discovery::irc_connect(const boost::system::error_code& ec,
    irc_handler handler)
{
    if (ec)
    {
        handler(error::network_unreachable);
        return;
    }

    rng_.seed(static_cast<unsigned int>(std::time(0)));
    boost::asio::async_read_until(*socket_, data_, "\r\n",
        std::bind(&discovery::irc_readline,
            shared_from_this(), _1, _2, handler));

    irc_identify();
//    handler(NULL);
}

void discovery::irc_readline(const boost::system::error_code& ec, size_t len,
    irc_handler handler)
{
    if (ec)
    {
        handler(error::channel_stopped);
        return;
    }
    std::ostringstream oss;
    oss << &data_;
    std::string line = oss.str();
    log_debug() << line;
    if (line.find(" 433 ") != std::string::npos)
        irc_identify();

    if (line.find(" 001 ") != std::string::npos)
        irc_join();

    if (line.find("PING :") != std::string::npos)
    {
        line.erase(0, 4);
        line.insert(0, "PONG");
        send_raw_line(line);
    }

    if (line.find(" JOIN :" + ircchan_) != std::string::npos &&
        line.find(ircnick_) != std::string::npos)
	    send_raw_line("WHO " + ircchan_);


    boost::asio::async_read_until(*socket_, data_, "\r\n",
        std::bind(&discovery::irc_readline,
            shared_from_this(), _1, _2, handler));
}

void discovery::irc_identify()
{
    boost::uniform_int<> dist(1,1000000000);
    uint32_t roll = dist(rng_);

    ircnick_ = "lbtc" + boost::lexical_cast<std::string>(roll);
    send_raw_line("NICK " + ircnick_);
    send_raw_line("USER " + ircnick_ + " 8 * :" + ircnick_);
}

void discovery::irc_join()
{
    boost::uniform_int<> dist(0,99);
    uint32_t roll = dist(rng_);

    ircchan_ = "#bitcoin";
    if (roll < 10)
        ircchan_ += "0";
    ircchan_ += boost::lexical_cast<std::string>(roll);

    send_raw_line(":source JOIN :" + ircchan_);
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
