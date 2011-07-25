#include <bitcoin/util/logger.hpp>

#include <iostream>

namespace libbitcoin {

logger_wrapper::logger_wrapper(logger_level lev)
  : lev_(lev)
{
}
logger_wrapper::logger_wrapper(const logger_wrapper& other)
  : stream(other.stream.str())
{
}
logger_wrapper::~logger_wrapper()
{
    if (lev_ == logger_level::error || lev_ == logger_level::fatal)
        std::cerr << stream.str() << std::endl;
    else
        std::cout << stream.str() << std::endl;
}

logger_wrapper log_debug()
{
    return logger_wrapper(logger_level::debug);
}

logger_wrapper log_info()
{
    return logger_wrapper(logger_level::info);
}

logger_wrapper log_warning()
{
    return logger_wrapper(logger_level::warning);
}

logger_wrapper log_error()
{
    return logger_wrapper(logger_level::error);
}

logger_wrapper log_fatal()
{
    return logger_wrapper(logger_level::fatal);
}

} // libbitcoin

