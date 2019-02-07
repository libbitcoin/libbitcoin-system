#ifndef LIBBITCOIN__WALLET_WIF_UNCOMPRESSED_HPP
#define LIBBITCOIN__WALLET_WIF_UNCOMPRESSED_HPP

#include <bitcoin/bitcoin/wallet/ec_private.hpp>

namespace libbitcoin {
namespace api {

class wallet_wif_uncompressed {
public:
	wallet::wif_uncompressed getValue() {
		return value;
	}

	void setValue(wallet::wif_uncompressed value) {
		this->value = value;
	}
private:
	wallet::wif_uncompressed value;
};

} // namespace api
} // namespace libbitcoin
#endif
