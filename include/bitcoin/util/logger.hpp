#ifndef LIBBITCOIN_LOGGER_H
#define LIBBITCOIN_LOGGER_H

#include <sstream>

namespace libbitcoin {

enum class log_domain
{
    normal,
    network,
    storage,
    validation
};

enum class logger_level
{
    null,
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

logger_wrapper log_debug(log_domain domain=log_domain::normal);
logger_wrapper log_info(log_domain domain=log_domain::normal);
logger_wrapper log_warning(log_domain domain=log_domain::normal);
logger_wrapper log_error(log_domain domain=log_domain::normal);
logger_wrapper log_fatal(log_domain domain=log_domain::normal);

} // libbitcoin

#endif

