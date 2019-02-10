#ifndef LIBBITCOIN__OPTIONS_METADATA_HPP
#define LIBBITCOIN__OPTIONS_METADATA_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parser.hpp>

namespace libbitcoin {
namespace api {

class config_options_metadata {
public:
	options_metadata getValue() {
		return value;
	}

	void setValue(options_metadata value) {
		this->value = value;
	}
private:
	options_metadata value;

};

} // namespace api
} // namespace libbitcoin
#endif
