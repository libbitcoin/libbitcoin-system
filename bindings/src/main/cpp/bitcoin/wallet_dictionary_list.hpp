#ifndef LIBBITCOIN__WALLET_DICTIONARY_LIST_HPP
#define LIBBITCOIN__WALLET_DICTIONARY_LIST_HPP

#include <bitcoin/bitcoin/wallet/dictionary.hpp>

namespace libbitcoin {
namespace api {

class wallet_dictionary_list {
public:
	wallet::dictionary_list* getValue() {
		return value_;
	}

	void setValue(wallet::dictionary_list* value) {
		value_ = value;
	}
private:
	 wallet::dictionary_list* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
