#ifndef LIBBITCOIN__CONFIG_ARGUMENT_LIST_HPP
#define LIBBITCOIN__CONFIG_ARGUMENT_LIST_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parameter.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

class config_argument_list {
public:
	config::argument_list* getValue() const {
		return value_;
	}

	void setValue(config::argument_list* value) {
		value_ = value;
	}
private:
	config::argument_list* value_;
};

} // namespace api
//} // namespace config
} // namespace libbitcoin
#endif
