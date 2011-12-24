#include <bitcoin/util/clock.hpp>
#include <bitcoin/util/logger.hpp>
using namespace libbitcoin;

int main()
{
    chrono_clock c;
    log_debug() << c.time();
    return 0;
}

