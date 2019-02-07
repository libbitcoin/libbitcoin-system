#ifndef LIBBITCOIN__WALLET_EK_SEED_HPP
#define LIBBITCOIN__WALLET_EK_SEED_HPP

#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace api {

class wallet_ek_seed {
public:
	wallet::ek_seed getValue() {
		return value;
	}

	void setValue(wallet::ek_seed value) {
		this->value = value;
	}
private:
	wallet::ek_seed value;
};

} // namespace api
} // namespace libbitcoin
#endif
