#ifndef LIBBITCOIN_P_UINT256_T_HPP
#define LIBBITCOIN_P_UINT256_T_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace api {

class p_uint256_t {

public:
  p_uint256_t(size_t n) {
    p = new libbitcoin::uint256_t[n];
    size = n;
  }
    
/*  p_uint256_t(unsigned char *value, unsigned int n) {
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
  void set(size_t i, libbitcoin::uint256_t value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  libbitcoin::uint256_t* cast() const {
    return p;
  }

  libbitcoin::uint256_t get(size_t i) const {
    return p[i];
  }

  size_t getSize() const {
    return size;
  }

  ~p_uint256_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  libbitcoin::uint256_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
