#ifndef LIBBITCOIN_THREADED_SERVICE_H
#define LIBBITCOIN_THREADED_SERVICE_H

#include <thread>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class thread_core
{
public:
    thread_core();
    ~thread_core();
    service_ptr service();
    strand_ptr create_strand();
private:
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
};

typedef std::shared_ptr<thread_core> thread_core_ptr;

class threaded_service
{
protected:
    threaded_service();
    service_ptr service();
    strand_ptr strand();
private:
    thread_core core_;
    strand_ptr strand_;
};

} // libbitcoin

#endif

