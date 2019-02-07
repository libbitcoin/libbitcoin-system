#ifndef LIBBITCOIN__WALLET_EK_ENTROPY_HPP
#define LIBBITCOIN__WALLET_EK_ENTROPY_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_ek_entropy {
public:
	wallet::ek_entropy getValue() {
		return value;
	}

	void setValue(wallet::ek_entropy value) {
		this->value = value;
	}
private:
	wallet::ek_entropy value;
};

} // namespace api
} // namespace libbitcoin
#endif
