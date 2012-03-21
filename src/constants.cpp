#include <bitcoin/constants.hpp>

namespace libbitcoin {

big_number max_target()
{
    big_number max_target;
    max_target.set_compact(max_bits);
    return max_target;
}

} // namespace libbitcoin

