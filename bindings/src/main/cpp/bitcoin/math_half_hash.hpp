#ifndef LIBBITCOIN__MATH_HALF_HASH_HPP
#define LIBBITCOIN__MATH_HALF_HASH_HPP

//#include <cstddef>
//#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class math_half_hash : public half_hash {

//public:
//  math_half_hash() : hash(new half_hash) {
//  }
//
///*  math_half_hash(unsigned char *value, unsigned int n) {
//    p = value;
//    size = n;
//  }
//
//  void set(unsigned char *value, unsigned int n) {
//    if (p) delete [] p;
//    size = n;
//    p = new unsigned char[size];
//    for (int i=0; i<size; i++) {
//      p[i] = value[i];
//    }
//  }
//*/
//  void set(size_t i, uint8_t value) {
//    if (i<half_hash_size) {
//      hash[i] = value;
//    } else {
//    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << half_hash_size << ")\n";
//    }
//  }
//
//  half_hash cast() {
//    return hash;
//  }
//
//  uint8_t get(size_t i) {
//    return hash[i];
//  }
//
//  size_t getSize() {
//    return half_hash_size;
//  }
//
//  ~math_half_hash() {
//	  delete hash;
//  }
//
//private:
//  half_hash hash;
};

} // namespace api
} // namespace libbitcoin
#endif
