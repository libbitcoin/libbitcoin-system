#ifndef LIBBITCOIN__MATH_LONG_HASH_HPP
#define LIBBITCOIN__MATH_LONG_HASH_HPP

//#include <cstddef>
//#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_long_hash {

public:
	math_long_hash() :
			value_(new long_hash) {
	}

	math_long_hash(long_hash hash) :
			value_(&hash) {
	}

	/*  math_long_hash(unsigned char *value, unsigned int n) {
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
		if (i < long_hash_size) {
			(*value_)[i] = value;
		} else {
			std::cerr << "SEVERE: index " << i << " is out of range [0;"
					<< long_hash_size << ")\n";
		}
	}

	long_hash* cast() {
		return value_;
	}

	uint8_t get(size_t i) {
		return (*value_)[i];
	}

	size_t getSize() {
		return long_hash_size;
	}

	~math_long_hash() {
		delete value_;
	}

	long_hash* getValue() {
		return value_;
	}

	void setValue(long_hash* value) {
		value_ = value;
	}
private:
	long_hash* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
