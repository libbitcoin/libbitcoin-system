#ifndef LIBBITCOIN__WALLET_ENCRYPTED_PUBLIC_HPP
#define LIBBITCOIN__WALLET_ENCRYPTED_PUBLIC_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_encrypted_public {
public:
	wallet::encrypted_public getValue() {
		return value;
	}

	void setValue(wallet::encrypted_public value) {
		this->value = value;
	}
private:
	wallet::encrypted_public value;
};

} // namespace api
} // namespace libbitcoin
#endif
