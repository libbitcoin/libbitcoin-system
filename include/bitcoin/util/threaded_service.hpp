#ifndef LIBBITCOIN_THREADED_SERVICE_H
#define LIBBITCOIN_THREADED_SERVICE_H

#include <thread>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class threaded_service
{
protected:
    threaded_service();
    ~threaded_service();
    service_ptr service();
private:
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
};

} // libbitcoin

#endif

