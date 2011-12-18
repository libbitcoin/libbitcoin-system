#include <bitcoin/constants.hpp>

namespace libbitcoin {

big_number max_target()
{
    big_number max_target;
    max_target.set_compacted(max_bits);
    return max_target;
}

} // libbitcoin

