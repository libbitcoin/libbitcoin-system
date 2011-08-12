#include <bitcoin/constants.hpp>

namespace libbitcoin {

big_number max_target()
{
    big_number max_target;
    max_target.set_compact(0x1d00ffff);
    return max_target;
}

} // libbitcoin

