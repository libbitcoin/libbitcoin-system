#include <bitcoin/constants.hpp>

namespace libbitcoin {

big_number max_target()
{
    big_number max_target;
    max_target.set_compact(max_bits);
    return max_target;
}

uint32_t magic_value()
{
	#ifdef ENABLE_TESTNET
	return 0x0709110b;
	#else
	return 0xd9b4bef9;
	#endif
}
} // namespace libbitcoin

