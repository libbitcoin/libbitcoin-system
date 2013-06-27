#include <bitcoin/utility/clock.hpp>
#include <bitcoin/utility/logger.hpp>
using namespace libbitcoin;

int main()
{
    chrono_clock c;
    log_debug() << c.time();
    return 0;
}

