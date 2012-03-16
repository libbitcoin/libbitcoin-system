#ifndef LIBBITCOIN_LOGGER_H
#define LIBBITCOIN_LOGGER_H

#include <sstream>
#include <map>
#include <vector>

namespace libbitcoin {

enum class log_domain
{
    normal,
    network,
    storage,
    validation,
    protocol,
    poller,
    session
};

enum class log_level
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
    logger_wrapper(log_level lev, log_domain domain); 
    logger_wrapper(const logger_wrapper& other);
    ~logger_wrapper();

    template <typename T>
    logger_wrapper& operator<<(T const& value) 
    {
        stream << value;
        return *this;
    }

    void alias(log_level lev, log_level map_lev);
    // Can add optional argument later to allow re-enabling
    void filter(log_domain domain);
private:
    typedef std::map<log_level, log_level> alias_mapping;
    typedef std::map<log_domain, std::vector<log_level>> filters_map;
    static alias_mapping aliases_;
    static filters_map filters_;
    std::ostringstream stream;
    log_level lev_;
    log_domain domain_;
};

logger_wrapper log_debug(log_domain domain=log_domain::normal);
logger_wrapper log_info(log_domain domain=log_domain::normal);
logger_wrapper log_warning(log_domain domain=log_domain::normal);
logger_wrapper log_error(log_domain domain=log_domain::normal);
logger_wrapper log_fatal(log_domain domain=log_domain::normal);

} // libbitcoin

#endif

