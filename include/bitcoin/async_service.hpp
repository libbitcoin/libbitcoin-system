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
    ~async_service();

    async_service(const async_service&) = delete;
    void operator=(const async_service&) = delete;

    void spawn();
    void shutdown();

    io_service& get_service();
    const io_service& get_service() const;

private:
    io_service service_;
    io_service::work* work_;
    std::vector<std::thread> threads_;
};

} // namespace libbitcoin

#endif

