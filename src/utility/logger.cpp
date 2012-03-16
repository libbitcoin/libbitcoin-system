#include <bitcoin/utility/logger.hpp>

#include <iostream>

namespace libbitcoin {

logger_wrapper::alias_mapping logger_wrapper::aliases_;
logger_wrapper::filters_map logger_wrapper::filters_;

logger_wrapper::logger_wrapper(log_level lev, log_domain domain)
  : lev_(lev), domain_(domain)
{
}
logger_wrapper::logger_wrapper(const logger_wrapper& other)
  : stream(other.stream.str())
{
}
logger_wrapper::~logger_wrapper()
{
    if (aliases_.find(lev_) != aliases_.end())
        lev_ = aliases_[lev_];
    if (filters_.find(domain_) != filters_.end())
        for (log_level filter_level: filters_[domain_])
            if (filter_level == lev_)
                return;
    if (lev_ == log_level::null)
        return;
    else if (lev_ == log_level::error || lev_ == log_level::fatal)
        std::cerr << stream.str() << std::endl;
    else
        std::cout << stream.str() << std::endl;
}

void logger_wrapper::alias(log_level lev, log_level map_lev)
{
    lev_ = log_level::null;
    aliases_[lev] = map_lev;
}
void logger_wrapper::filter(log_domain domain)
{
    lev_ = log_level::null;
    if (filters_.find(domain) == filters_.end())
        filters_[domain] = std::vector<log_level>{lev_};
    else
        filters_[domain].push_back(lev_);
}

logger_wrapper log_debug(log_domain domain)
{
    return logger_wrapper(log_level::debug, domain);
}

logger_wrapper log_info(log_domain domain)
{
    return logger_wrapper(log_level::info, domain);
}

logger_wrapper log_warning(log_domain domain)
{
    return logger_wrapper(log_level::warning, domain);
}

logger_wrapper log_error(log_domain domain)
{
    return logger_wrapper(log_level::error, domain);
}

logger_wrapper log_fatal(log_domain domain)
{
    return logger_wrapper(log_level::fatal, domain);
}

} // libbitcoin

