#include <bitcoin/utility/logger.hpp>

#include <iostream>

namespace libbitcoin {

std::string level_repr(log_level level)
{
    switch (level)
    {
        case log_level::null:
            return "NULL";
        case log_level::debug:
            return "DEBUG";
        case log_level::info:
            return "INFO";
        case log_level::warning:
            return "WARNING";
        case log_level::error:
            return "ERROR";
        case log_level::fatal:
            return "FATAL";
    }
    return "";
}

void output_ostream(std::ostream& ostr, log_level level,
    const std::string& domain, const std::string& body)
{
    ostr << level_repr(level);
    if (!domain.empty())
        ostr << " [" << domain << "]";
    ostr << ": " << body << std::endl;
}

void ignore_output(log_level,
    const std::string&, const std::string&)
{
}
void output_cout(log_level level,
    const std::string& domain, const std::string& body)
{
    output_ostream(std::cout, level, domain, body);
}
void output_cerr(log_level level,
    const std::string& domain, const std::string& body)
{
    output_ostream(std::cerr, level, domain, body);
}

logger_wrapper::destination_map logger_wrapper::dests_{
#ifdef DEBUG
    std::make_pair(log_level::debug, output_cout),
#else
    std::make_pair(log_level::debug, ignore_output),
#endif
    std::make_pair(log_level::info, output_cout),
    std::make_pair(log_level::warning, output_cerr),
    std::make_pair(log_level::error, output_cerr),
    std::make_pair(log_level::fatal, output_cerr)
};

logger_wrapper::logger_wrapper(log_level lev, const std::string& domain)
  : level_(lev), domain_(domain)
{
}
// g++ bug in initializer list.
// It should be: stream_(std::move(other.stream_))
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=54316
logger_wrapper::logger_wrapper(logger_wrapper&& other)
  : level_(other.level_), domain_(std::move(other.domain_)),
    stream_(other.stream_.str())
{
}
logger_wrapper::~logger_wrapper()
{
    if (!dests_.count(level_))
        return;
    auto& outfunc = dests_[level_];
    outfunc(level_, domain_, stream_.str());
}

void logger_wrapper::set_output_function(logger_output_func outfunc)
{
    dests_[level_] = outfunc;
}

logger_wrapper log_debug(const std::string& domain)
{
    return logger_wrapper(log_level::debug, domain);
}

logger_wrapper log_info(const std::string& domain)
{
    return logger_wrapper(log_level::info, domain);
}

logger_wrapper log_warning(const std::string& domain)
{
    return logger_wrapper(log_level::warning, domain);
}

logger_wrapper log_error(const std::string& domain)
{
    return logger_wrapper(log_level::error, domain);
}

logger_wrapper log_fatal(const std::string& domain)
{
    return logger_wrapper(log_level::fatal, domain);
}

} // namespace libbitcoin

