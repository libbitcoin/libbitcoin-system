#include <bitcoin/network/discovery.hpp>
#include <bitcoin/error.hpp>

void handler(const std::error_code& ec, const std::string hostname)
{
    if (ec)
    {
        libbitcoin::log_fatal() << ec.message();
        exit(-1);
    }

    libbitcoin::log_info() << "Got new client: " << hostname;
}

int main()
{
    std::shared_ptr<libbitcoin::discovery> disc = std::make_shared<libbitcoin::discovery>();
    disc->irc_discovery(&handler);
    while (std::cin.get() != -1);
}
