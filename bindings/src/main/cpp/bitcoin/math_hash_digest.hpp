#ifndef LIBBITCOIN__MATH_HASH_DIGEST_HPP
#define LIBBITCOIN__MATH_HASH_DIGEST_HPP

//#include <cstddef>
//#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_hash_digest {

public:
	math_hash_digest() :
			value(new hash_digest) {
	}

	/*  math_hash_digest(unsigned char *value, unsigned int n) {
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
		if (i < hash_size) {
			value[i] = value;
		} else {
			std::cerr << "SEVERE: index " << i << " is out of range [0;"
					<< hash_size << ")\n";
		}
	}

	hash_digest cast() {
		return value;
	}

	uint8_t get(size_t i) {
		return value[i];
	}

	hash_digest getValue() {
		return value;
	}

	void setValue(hash_digest value) {
		this->value = value;
	}

	size_t getSize() {
		return hash_size;
	}

	~math_hash_digest() {
		delete value;
	}

private:
	hash_digest value;
};

} // namespace api
} // namespace libbitcoin
#endif
