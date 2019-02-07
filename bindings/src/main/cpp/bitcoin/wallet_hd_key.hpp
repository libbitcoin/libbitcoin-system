#ifndef LIBBITCOIN__WALLET_HD_KEY_HPP
#define LIBBITCOIN__WALLET_HD_KEY_HPP

#include <bitcoin/bitcoin/wallet/hd_public.hpp>

namespace libbitcoin {
namespace api {

class wallet_hd_key {
public:
	wallet::hd_key getValue() {
		return value;
	}

	void setValue(wallet::hd_key value) {
		this->value = value;
	}
private:
	wallet::hd_key value;
};

} // namespace api
} // namespace libbitcoin
#endif
