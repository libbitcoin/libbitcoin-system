#include <bitcoin/utility/clock.hpp>

namespace libbitcoin {

void chrono_clock::add_data(ptime time_data)
{
    // Lets take 1 clock to sea
}

const ptime chrono_clock::time() const
{
    return boost::posix_time::second_clock::universal_time();
}

} // namespace libbitcoin

