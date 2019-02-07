#ifndef LIBBITCOIN__WALLET_EK_SALT_HPP
#define LIBBITCOIN__WALLET_EK_SALT_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_ek_salt {
public:
	wallet::ek_salt getValue() {
		return value;
	}

	void setValue(wallet::ek_salt value) {
		this->value = value;
	}
private:
	wallet::ek_salt value;
};

} // namespace api
} // namespace libbitcoin
#endif
