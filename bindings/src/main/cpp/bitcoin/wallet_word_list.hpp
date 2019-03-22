#ifndef LIBBITCOIN__WALLET_WORD_LIST_HPP
#define LIBBITCOIN__WALLET_WORD_LIST_HPP

#include <bitcoin/bitcoin/wallet/mnemonic.hpp>

namespace libbitcoin {
namespace api {

class wallet_word_list
{
public:
	wallet::word_list getValue() {
		return value;
	}

	void setValue(wallet::word_list value) {
		this->value = value;
	}
private:
	wallet::word_list value;
};

} // namespace api
} // namespace libbitcoin
#endif
