#ifndef LIBBITCOIN__WALLET_EK_FLAG_HPP
#define LIBBITCOIN__WALLET_EK_FLAG_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

enum wallet_ek_flag : wallet::ek_flag
{
    lot_sequence_key = 1 << 2,
    ec_compressed_key = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,

    /// Two bits are used to represent "not multiplied".
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};

} // namespace api
} // namespace libbitcoin
#endif
