#ifndef LIBBITCOIN__P_STD_TIME_T_HPP
#define LIBBITCOIN__P_STD_TIME_T_HPP

#include <ctime>

namespace libbitcoin {
namespace api {

class p_std_time_t {
public:
	p_std_time_t() :
			value(new std::time_t) {
	}

	void setValue(std::time_t *value) {
		this->value = value;
	}

	std::time_t *getValue() const {
		return value;
	}

	~p_std_time_t() {
		delete value;
	}

private:
	std::time_t *value;
};

} // namespace api
} // namespace libbitcoin
#endif
