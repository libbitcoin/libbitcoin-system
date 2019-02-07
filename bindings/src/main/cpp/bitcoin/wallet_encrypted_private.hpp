#ifndef LIBBITCOIN__WALLET_ENCRYPTED_PRIVATE_HPP
#define LIBBITCOIN__WALLET_ENCRYPTED_PRIVATE_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_encrypted_private {
public:
	wallet::encrypted_private getValue() {
		return value;
	}

	void setValue(wallet::encrypted_private value) {
		this->value = value;
	}
private:
	wallet::encrypted_private value;
};

} // namespace api
} // namespace libbitcoin
#endif
