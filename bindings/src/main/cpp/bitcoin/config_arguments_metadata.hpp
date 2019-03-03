#ifndef LIBBITCOIN__CONFIG_ARGUMENTS_METADATA_HPP
#define LIBBITCOIN__CONFIG_ARGUMENTS_METADATA_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parser.hpp>

namespace libbitcoin {
namespace api {

class config_arguments_metadata {
public:
	arguments_metadata getValue() {
		return value;
	}

	void setValue(arguments_metadata value) {
		this->value = value;
	}
private:
	arguments_metadata value;

};

} // namespace api
} // namespace libbitcoin
#endif
