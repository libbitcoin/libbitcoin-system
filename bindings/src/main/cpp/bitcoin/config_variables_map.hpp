#ifndef LIBBITCOIN__CONFIG_VARIABLES_MAP_HPP
#define LIBBITCOIN__CONFIG_VARIABLES_MAP_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parser.hpp>

namespace libbitcoin {
namespace api {

class config_variables_map {
public:
	variables_map getValue() {
		return value;
	}

	void setValue(variables_map value) {
		this->value = value;
	}
private:
	variables_map value;
};

} // namespace api
} // namespace libbitcoin
#endif
