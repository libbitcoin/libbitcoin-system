#ifndef LIBBITCOIN_LOGGER_H
#define LIBBITCOIN_LOGGER_H

#include <sstream>

namespace libbitcoin {

// LOG  = normal
// DLOG = debug log
// VLOG = verbose log
// SYSLOG = system log
enum logger_level
{
    LOG_INFO = 0,
    DLOG_INFO
};

class logger_wrapper
{
public:
    logger_wrapper(logger_level lev); 
    logger_wrapper(const logger_wrapper& other);
    ~logger_wrapper();

    template <typename T>
    logger_wrapper& operator<<(T const& value) 
    {
        stream << value;
        return *this;
    }
private:
    std::ostringstream stream;
    logger_level lev_;
};

logger_wrapper logger(logger_level lev);

} // libbitcoin

#endif

