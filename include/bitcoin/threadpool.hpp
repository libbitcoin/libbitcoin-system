#ifndef LIBBITCOIN_THREADPOOL_HPP
#define LIBBITCOIN_THREADPOOL_HPP

#include <functional>
#include <thread>

#include <boost/asio.hpp>

namespace libbitcoin {

using boost::asio::io_service;

class threadpool
{
public:
    threadpool();
    threadpool(size_t number_threads);
    ~threadpool();

    threadpool(const threadpool&) = delete;
    void operator=(const threadpool&) = delete;

    void spawn();

    void stop();
    void shutdown();
    void join();

    io_service& service();
    const io_service& service() const;

private:
    io_service ios_;
    io_service::work* work_;
    std::vector<std::thread> threads_;
};

class async_strand
{
public:
    async_strand(threadpool& pool);

    template <typename Handler>
    void queue(Handler handler)
    {
        ios_.post(strand_.wrap(handler));
    }

private:
    io_service& ios_;
    io_service::strand strand_;
};

} // namespace libbitcoin

#endif

