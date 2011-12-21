#include <iostream>
#include <functional>
#include <memory>

#include <bitcoin/constants.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/util/logger.hpp>

using namespace libbitcoin;

class poller_application
  : public threaded_service,
    public std::enable_shared_from_this<poller_application>
{
public:
    poller_application();

    void start(std::string hostname, unsigned int port);

private:
    kernel_ptr kernel_;
    network_ptr network_;
    blockchain_ptr blockchain_;
};

typedef std::shared_ptr<poller_application> poller_application_ptr;

poller_application::poller_application()
  : kernel_(new kernel)
{
    network_.reset(new network);
    kernel_->register_network(network_);

    blockchain_.reset(new bdb_blockchain("database/"));
    kernel_->register_blockchain(blockchain_);
}

void poller_application::start(std::string hostname, unsigned int port)
{
    kernel_->connect(hostname, port);
}

int main(int argc, const char** argv)
{
    bdb_blockchain::setup("database/");

    poller_application_ptr app(new poller_application());
    app->start("localhost", 8333);
    while (true)
    {
        char n;
        std::cin >> n;
        if (std::cin.eof())
            break;
    }
    return 0;
}

