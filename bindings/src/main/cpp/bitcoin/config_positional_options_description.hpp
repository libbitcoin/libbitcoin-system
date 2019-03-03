#ifndef LIBBITCOIN__CONFIG_POSITIONAL_OPTIONS_DESCRIPTION_HPP
#define LIBBITCOIN__CONFIG_POSITIONAL_OPTIONS_DESCRIPTION_HPP

#include <cstdint>
#include <boost/program_options.hpp>

namespace libbitcoin {
namespace api {

class config_positional_options_description {
public:
	boost::program_options::positional_options_description getValue() {
        return value;
    }

    void setValue(boost::program_options::positional_options_description value) {
        this->value = value;
    }
private:
    boost::program_options::positional_options_description value;
};

} // namespace api
} // namespace libbitcoin
#endif
