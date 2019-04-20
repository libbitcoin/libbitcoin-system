#ifndef LIBBITCOIN__P_UINT64_T_VECTOR_HPP
#define LIBBITCOIN__P_UINT64_T_VECTOR_HPP

#include <cstdint>
#include <vector>

namespace libbitcoin {
namespace api {

class p_uint64_t_vector {
public:
//	  p_uint64_t_vector() : value_(new std::vector<std::uint64_t>&&){
	p_uint64_t_vector() :
			value_(new std::vector<std::uint64_t>) {
	}

	void set(std::vector<std::uint64_t> *value) {
		value_ = value;
	}

	const std::vector<std::uint64_t> *cast() const {
		return value_;
	}

	std::vector<std::uint64_t> *get() {
		return value_;
	}

	~p_uint64_t_vector() {
		delete value_;
	}

private:
	std::vector<std::uint64_t> *value_;
};

} // namespace api
} // namespace libbitcoin
#endif
