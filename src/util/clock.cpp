#include <bitcoin/util/clock.hpp>


namespace libbitcoin {

const ptime clock::get_time()
{
    return boost::posix_time::second_clock::universal_time();
}

} // libbitcoin

