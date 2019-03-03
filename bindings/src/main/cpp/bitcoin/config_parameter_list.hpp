#ifndef LIBBITCOIN__CONFIG_PARAMETER_LIST_HPP
#define LIBBITCOIN__CONFIG_PARAMETER_LIST_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/parameter.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

class config_parameter_list {
public:
	config::parameter_list getValue() {
        return value;
    }

    void setValue(config::parameter_list value) {
        this->value = value;
    }
private:
    config::parameter_list value;

};

} // namespace api
//} // namespace config
} // namespace libbitcoin
#endif
