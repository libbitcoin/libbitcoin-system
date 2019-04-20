#ifndef LIBBITCOIN__MATH_MINI_HASH_HPP
#define LIBBITCOIN__MATH_MINI_HASH_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_mini_hash {

public:
	math_mini_hash() :
			value_(new mini_hash) {
	}

	math_mini_hash(mini_hash hash) :
			value_(&hash) {
	}

	/*  math_mini_hash(unsigned char *value, unsigned int n) {
	 p = value;
	 size = n;
	 }

	 void set(unsigned char *value, unsigned int n) {
	 if (p) delete [] p;
	 size = n;
	 p = new unsigned char[size];
	 for (int i=0; i<size; i++) {
	 p[i] = value[i];
	 }
	 }
	 */
	void set(size_t i, uint8_t value) {
		if (i < mini_hash_size) {
			(*value_)[i] = value;
		} else {
			std::cerr << "SEVERE: index " << i << " is out of range [0;"
					<< mini_hash_size << ")\n";
		}
	}

	mini_hash* cast() {
		return value_;
	}

	uint8_t get(size_t i) {
		return (*value_)[i];
	}

	size_t getSize() {
		return mini_hash_size;
	}

	~math_mini_hash() {
		delete value_;
	}

public:
	mini_hash* getValue() {
		return value_;
	}

	void setValue(mini_hash *value) {
		value_ = value;
	}
private:
	mini_hash* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
