#ifndef LIBBITCOIN__WALLET_ENCRYPTED_TOKEN_HPP
#define LIBBITCOIN__WALLET_ENCRYPTED_TOKEN_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_encrypted_token {
public:
	wallet::encrypted_token getValue() {
		return value;
	}

	void setValue(wallet::encrypted_token value) {
		this->value = value;
	}
private:
	wallet::encrypted_token value;
};

} // namespace api
} // namespace libbitcoin
#endif
