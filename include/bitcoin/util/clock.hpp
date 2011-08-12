#ifndef LIBBITCOIN_CLOCK_H
#define LIBBITCOIN_CLOCK_H

#include <boost/date_time/posix_time/posix_time.hpp>

namespace libbitcoin {

using boost::posix_time::ptime;
using boost::posix_time::hours;

class clock
{
public:
    const ptime get_time();
};

} // libbitcoin

#endif

