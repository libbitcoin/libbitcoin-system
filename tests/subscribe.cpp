#include <memory>
#include <boost/asio.hpp>
using boost::asio::io_service;
#include <bitcoin/utility/logger.hpp>
//#include <bitcoin/utility/subscriber.hpp>
#include "../include/bitcoin/utility/subscriber.hpp"
#include <bitcoin/async_service.hpp>
using namespace libbitcoin;

namespace libbitcoin {

struct abc
{
    abc(size_t x)
      : x(x)
    {
        log_error() << "abc()";
    }
    ~abc()
    {
        log_error() << "~abc()";
    }
    size_t x;
};

}

void foo(abc_ptr a)
{
    log_debug() << "fii " << (a ? "y" : "n");
    log_debug() << "Weeeee #" << a->x;
}

typedef std::function<void (abc_ptr)> example_handler_type;
typedef std::shared_ptr<subscriber<abc_ptr>> subscribe_ver_ptr;

void defer_sub(subscribe_ver_ptr s, example_handler_type f)
{
    s->subscribe(f);
    s->subscribe(f);
}

void callall(subscribe_ver_ptr s)
{
    abc_ptr a = std::make_shared<abc>(110);
    s->relay(a);
}

int main()
{
    async_service service(1);
    io_service::strand str(service.get_service());
    auto sub = std::make_shared<subscriber<abc_ptr>>(str);
    str.post(std::bind(defer_sub, sub, foo));
    str.post(std::bind(callall, sub));
    std::cin.get();
    return 0;
}

