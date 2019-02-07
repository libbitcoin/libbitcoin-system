#ifndef LIBBITCOIN__WALLET_PAYMENT_ADDRESS_LIST_HPP
#define LIBBITCOIN__WALLET_PAYMENT_ADDRESS_LIST_HPP

#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace api {

class wallet_payment_address_list {
public:
	wallet::payment_address::list getValue() {
		return value;
	}

	void setValue(wallet::payment_address::list value) {
		this->value = value;
	}
private:
	wallet::payment_address::list value;
};

} // namespace api
} // namespace libbitcoin
#endif
