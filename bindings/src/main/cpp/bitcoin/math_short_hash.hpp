#ifndef LIBBITCOIN__MATH_SHORT_HASH_HPP
#define LIBBITCOIN__MATH_SHORT_HASH_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_short_hash {

public:
	math_short_hash() :
			value(new short_hash) {
	}

	/*  math_short_hash(unsigned char *value, unsigned int n) {
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
		if (i < short_hash_size) {
			value[i] = value;
		} else {
			std::cerr << "SEVERE: index " << i << " is out of range [0;"
					<< short_hash_size << ")\n";
		}
	}

	short_hash cast() {
		return value;
	}

	uint8_t get(size_t i) {
		return value[i];
	}

	size_t getSize() {
		return short_hash_size;
	}

	~math_short_hash() {
		delete value;
	}

	short_hash getValue() {
		return value;
	}

	void setValue(short_hash value) {
		this->value = value;
	}
private:
	short_hash value;
};

} // namespace api
} // namespace libbitcoin
#endif
