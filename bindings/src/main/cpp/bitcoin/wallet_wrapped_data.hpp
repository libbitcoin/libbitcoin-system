#ifndef LIBBITCOIN__WALLET_PAYMENT_ADDRESS_LIST_HPP
#define LIBBITCOIN__WALLET_PAYMENT_ADDRESS_LIST_HPP

#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace api {

class wallet_wrapped_data
{
public:
    wallet::wrapped_data getValue() {
		return value;
	}

	void setValue(wallet::wrapped_data value) {
		this->value = value;
	}
private:
	wallet::wrapped_data value;
};

} // namespace api
} // namespace libbitcoin
#endif
