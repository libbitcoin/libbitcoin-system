#ifndef LIBBITCOIN__WALLET_URI_QUERY_MAP_HPP
#define LIBBITCOIN__WALLET_URI_QUERY_MAP_HPP

#include <bitcoin/bitcoin/wallet/uri.hpp>

namespace libbitcoin {
namespace api {

class wallet_uri_query_map {
public:
	wallet::uri::query_map getValue() {
		return value;
	}

	void setValue(wallet::uri::query_map value) {
		this->value = value;
	}
private:
	wallet::uri::query_map value;
};

} // namespace api
} // namespace libbitcoin
#endif
