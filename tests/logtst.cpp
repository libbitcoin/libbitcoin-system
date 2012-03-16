#include <bitcoin/utility/logger.hpp>
using namespace libbitcoin;

int main()
{
    log_info().filter(log_domain::protocol);
    log_debug() << "Debug";
    log_debug().alias(log_level::debug, log_level::null);
    log_debug() << "Debug";
    log_debug().filter(log_domain::protocol);
    log_info() << "Info";
    log_info(log_domain::protocol) << "Info protocol";
    return 0;
}

