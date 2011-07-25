#ifndef LIBBITCOIN_LOGGER_H
#define LIBBITCOIN_LOGGER_H

#include <sstream>

namespace libbitcoin {

enum class logger_level
{
    debug,
    info,
    warning,
    error,
    fatal
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

logger_wrapper log_debug();
logger_wrapper log_info();
logger_wrapper log_warning();
logger_wrapper log_error();
logger_wrapper log_fatal();

} // libbitcoin

#endif

