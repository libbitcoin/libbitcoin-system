#ifndef LIBBITCOIN__WALLET_HD_CHAIN_CODE_HPP
#define LIBBITCOIN__WALLET_HD_CHAIN_CODE_HPP

#include <bitcoin/bitcoin/wallet/hd_public.hpp>

namespace libbitcoin {
namespace api {

class wallet_hd_chain_code {
public:
	wallet::hd_chain_code getValue() {
		return value;
	}

	void setValue(wallet::hd_chain_code value) {
		this->value = value;
	}
private:
	wallet::hd_chain_code value;
};

} // namespace api
} // namespace libbitcoin
#endif
