#ifndef LIBBITCOIN_ASYNC_SERVICE_H
#define LIBBITCOIN_ASYNC_SERVICE_H

#include <functional>
#include <thread>

#include <boost/asio.hpp>

namespace libbitcoin {

using boost::asio::io_service;

class async_service
{
public:
    async_service();
    async_service(size_t number_threads);

    async_service(const async_service&) = delete;
    void operator=(const async_service&) = delete;

    void spawn();

    void stop();
    void shutdown();
    void join();

    io_service& get_service();
    const io_service& get_service() const;

private:
    io_service ios_;
    io_service::work* work_;
    std::vector<std::thread> threads_;
};

class async_strand
{
public:
    async_strand(async_service& service);

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

