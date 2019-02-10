#ifndef LIBBITCOIN__OPTION_METADATA_HPP
#define LIBBITCOIN__OPTION_METADATA_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parser.hpp>

namespace libbitcoin {
namespace api {

class config_option_metadata {
public:
	option_metadata getValue() {
		return value;
	}

	void setValue(option_metadata value) {
		this->value = value;
	}
private:
	option_metadata value;

};

} // namespace api
} // namespace libbitcoin
#endif
