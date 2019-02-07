#ifndef LIBBITCOIN__WALLET_WIF_COMPRESSED_HPP
#define LIBBITCOIN__WALLET_WIF_COMPRESSED_HPP

#include <bitcoin/bitcoin/wallet/ec_private.hpp>

namespace libbitcoin {
namespace api {

class wallet_wif_compressed {
public:
	wallet::wif_compressed getValue() {
		return value;
	}

	void setValue(wallet::wif_compressed value) {
		this->value = value;
	}
private:
	wallet::wif_compressed value;
};

} // namespace api
} // namespace libbitcoin
#endif
