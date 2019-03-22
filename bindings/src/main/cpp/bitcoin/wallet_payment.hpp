#ifndef LIBBITCOIN__WALLET_PAYMENT_HPP
#define LIBBITCOIN__WALLET_PAYMENT_HPP

#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace api {

class wallet_payment {
public:
	wallet::payment getValue() {
		return value;
	}

	void setValue(wallet::payment value) {
		this->value = value;
	}
private:
	wallet::payment value;
};

} // namespace api
} // namespace libbitcoin
#endif
