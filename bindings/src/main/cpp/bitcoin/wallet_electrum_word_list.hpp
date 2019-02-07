#ifndef LIBBITCOIN__WALLET_ELECTRUM_WORD_LIST_HPP
#define LIBBITCOIN__WALLET_ELECTRUM_WORD_LIST_HPP

#include <bitcoin/bitcoin/wallet/electrum.hpp>

namespace libbitcoin {
namespace api {

class wallet_electrum_word_list
{
public:
	wallet::electrum::word_list getValue() {
		return value;
	}

	void setValue(wallet::electrum::word_list value) {
		this->value = value;
	}
private:
	wallet::electrum::word_list value;
};

} // namespace api
} // namespace libbitcoin
#endif
