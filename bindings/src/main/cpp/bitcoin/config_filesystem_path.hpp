#ifndef LIBBITCOIN__CONFIG_FILESYSTEM_PATH_HPP
#define LIBBITCOIN__CONFIG_FILESYSTEM_PATH_HPP

#include <boost/filesystem/path.hpp>

namespace libbitcoin {
namespace api {

class config_filesystem_path {
public:
	boost::filesystem::path* getValue() const {
		return value;
	}

	void setValue(boost::filesystem::path* value) {
		this->value = value;
	}
private:
	boost::filesystem::path* value;

};

} // namespace api
} // namespace libbitcoin
#endif
