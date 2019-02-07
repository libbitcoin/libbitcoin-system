#ifndef LIBBITCOIN__UTILITY_SIZE_TYPE_HPP
#define LIBBITCOIN__UTILITY_SIZE_TYPE_HPP

#include <cstdint>

namespace libbitcoin {
namespace api {

class utility_size_type {
public:
	std::size_t getValue() {
		return value;
	}

	void setValue(std::size_t value) {
		this->value = value;
	}
private:
	std::size_t value;
};

} // namespace api
} // namespace libbitcoin
#endif
