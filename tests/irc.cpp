#include <bitcoin/network/discovery.hpp>
#include <bitcoin/error.hpp>

void handler(const std::error_code& ec)
{
    libbitcoin::log_fatal() << ec.message();
    exit(-1);
}

int main()
{
    std::shared_ptr<libbitcoin::discovery> disc = std::make_shared<libbitcoin::discovery>();
    disc->irc_discovery(&handler);
    while (std::cin.get() != -1);
}
