#ifndef LIBBITCOIN_CLOCK_H
#define LIBBITCOIN_CLOCK_H

#include <boost/date_time/posix_time/posix_time.hpp>

namespace libbitcoin {

using boost::posix_time::ptime;
using boost::posix_time::hours;

class chrono_clock
{
public:
    void add_data(ptime time_data);
    const ptime time() const;

private:
    ptime offset_;
};

} // namespace libbitcoin

#endif

