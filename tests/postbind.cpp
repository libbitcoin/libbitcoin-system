#include <bitcoin/util/postbind.hpp>
#include <thread>
#include <iostream>
using namespace libbitcoin;
using namespace boost::asio;

typedef std::shared_ptr<io_service::work> work_ptr;

void run(service_ptr service)
{
    service->run();
}

void foo(char a, int x)
{
    std::cout << "this thread: " << std::this_thread::get_id() << "\n"
            << x << "\n";
}

using std::placeholders::_1;

int main()
{
    service_ptr service(new io_service);
    strand_ptr strand(new io_service::strand(*service));
    work_ptr work(new io_service::work(*service));
    std::thread t(std::bind(run, service));
    std::cout << "main thread: " << std::this_thread::get_id() << "\n";
    auto func = postbind<int>(strand, std::bind(foo, 'a', _1));
    func(99);
    t.join();
}

