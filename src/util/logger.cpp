#include "util/logger.hpp"

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
    std::cout << stream.str();
}

} // libbitcoin

