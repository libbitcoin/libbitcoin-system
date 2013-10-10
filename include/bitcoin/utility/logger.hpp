#ifndef LIBBITCOIN_LOGGER_HPP
#define LIBBITCOIN_LOGGER_HPP

#include <functional>
#include <sstream>
#include <map>

namespace libbitcoin {

#define LOG_NETWORK     "network"
#define LOG_BLOCKCHAIN  "blockchain"
#define LOG_VALIDATE    "validate"
#define LOG_PROTOCOL    "protocol"
#define LOG_POLLER      "poller"
#define LOG_SESSION     "session"
#define LOG_SCRIPT      "script"
#define LOG_TXPOOL      "transaction_pool"
#define LOG_TXIDX       "transaction_indexer"

enum class log_level
{
    null,
    debug,
    info,
    warning,
    error,
    fatal
};

std::string level_repr(log_level level);

class logger_wrapper
{
public:
    typedef std::function<void (
        log_level, const std::string&, const std::string&)> logger_output_func;

    logger_wrapper(log_level lev, const std::string& token);
    logger_wrapper(logger_wrapper&& other);
    ~logger_wrapper();

    template <typename T>
    logger_wrapper& operator<<(T const& value) 
    {
        stream_ << value;
        return *this;
    }

    void set_output_function(logger_output_func outfunc);

private:
    typedef std::map<log_level, logger_output_func> destination_map;

    static destination_map dests_;

    log_level level_;
    std::string domain_;
    std::ostringstream stream_;
};

logger_wrapper log_debug(const std::string& domain="");
logger_wrapper log_info(const std::string& domain="");
logger_wrapper log_warning(const std::string& domain="");
logger_wrapper log_error(const std::string& domain="");
logger_wrapper log_fatal(const std::string& domain="");

} // namespace libbitcoin

#endif

