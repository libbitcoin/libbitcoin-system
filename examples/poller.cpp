#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/postbind.hpp>

using namespace libbitcoin;
using std::placeholders::_1;
using std::placeholders::_2;

using boost::asio::deadline_timer;
using boost::posix_time::seconds;
typedef std::shared_ptr<deadline_timer> deadline_timer_ptr;

class poller_application
  : public threaded_service,
    public std::enable_shared_from_this<poller_application>
{
public:
    poller_application(std::string dbname, 
            std::string dbuser, std::string dbpass);

    void start(std::string hostname, unsigned int port);

private:
    kernel_ptr kernel_;
    network_ptr network_;
    storage_ptr storage_;
};

typedef std::shared_ptr<poller_application> poller_application_ptr;

poller_application::poller_application(std::string dbname, 
        std::string dbuser, std::string dbpass)
  : kernel_(new kernel)
{
    network_.reset(new network_impl);
    kernel_->register_network(network_);

    storage_.reset(new postgresql_storage(kernel_, dbname, dbuser, dbpass));
    kernel_->register_storage(storage_);
}

void poller_application::start(std::string hostname, unsigned int port)
{
    kernel_->connect(hostname, port);
}

int main(int argc, const char** argv)
{
    if (argc < 5)
    {
        log_info() << "poller [DBNAME] [DBUSER] [DBPASSWORD] [HOST:PORT] ...";
        return -1;
    }
    log_info() << "Starting poller... CTRL-D (EOF) to exit.";
    std::string dbname = argv[1], dbuser = argv[2], dbpass = argv[3];
    poller_application_ptr app(new poller_application(dbname, dbuser, dbpass));
    for (int hosts_iter = 4; hosts_iter < argc; ++hosts_iter)
    {
        std::vector<std::string> args;
        boost::split(args, argv[hosts_iter], boost::is_any_of(":"));
        if (args.size() == 1)
            app->start(args[0], 8333);
        else
            app->start(args[0], boost::lexical_cast<unsigned int>(args[1]));
    }
    while (true)
    {
        char n;
        std::cin >> n;
        if (std::cin.eof())
            break;
    }
    return 0;
}

