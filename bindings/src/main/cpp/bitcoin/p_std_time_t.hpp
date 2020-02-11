#ifndef LIBBITCOIN__P_STD_TIME_T_HPP
#define LIBBITCOIN__P_STD_TIME_T_HPP

#include <ctime>

namespace libbitcoin {
namespace api {

class p_std_time_t {
public:
//	p_std_time_t() :
//			value(0) {
//	}

	p_std_time_t(std::time_t* value) :
			value_(value) {
	}

	void setValue(std::time_t* value) {
		value_ = value;
	}

	std::time_t* getValue() const {
		return value_;
	}

private:
	std::time_t* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
