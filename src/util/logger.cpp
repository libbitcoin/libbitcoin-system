#include "bitcoin/util/logger.hpp"

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
    if (lev_ == LOG_ERROR || lev_ == LOG_FATAL)
        std::cerr << stream.str() << std::endl;
    else
        std::cout << stream.str() << std::endl;
}

logger_wrapper logger(logger_level lev)
{
    return logger_wrapper(lev);
}

} // libbitcoin

