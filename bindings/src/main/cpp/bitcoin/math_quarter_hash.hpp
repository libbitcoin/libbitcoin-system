#ifndef LIBBITCOIN__MATH_quarter_hash_HPP
#define LIBBITCOIN__MATH_quarter_hash_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_quarter_hash {

public:
  math_quarter_hash() : hash(new quarter_hash) {
  }
    
/*  math_quarter_hash(unsigned char *value, unsigned int n) {
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
    if (i<quarter_hash_size) {
      hash[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << quarter_hash_size << ")\n";
    }
  }

  quarter_hash cast() {
    return hash;
  }

  uint8_t get(size_t i) {
    return hash[i];
  }

  size_t getSize() {
    return quarter_hash_size;
  }

  ~math_quarter_hash() {
	  delete hash;
  }

private:
  quarter_hash hash;
};

} // namespace api
} // namespace libbitcoin
#endif
