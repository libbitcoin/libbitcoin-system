#include <bitcoin/constants.hpp>

namespace libbitcoin {

uint64_t block_value(size_t depth)
{
    uint64_t subsidy = coin_price(block_reward);
    subsidy >>= (depth / reward_interval);
    return subsidy;
}

big_number max_target()
{
    big_number max_target;
    max_target.set_compact(max_bits);
    return max_target;
}

} // libbitcoin

