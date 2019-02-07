#ifndef LIBBITCOIN_P_UINT8_T_HPP
#define LIBBITCOIN_P_UINT8_T_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_uint8_t {

public:
  p_uint8_t(size_t n) {
    p = new uint8_t[n];
    size = n;
  }
    
/*  p_uint8_t(unsigned char *value, unsigned int n) {
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
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  uint8_t* cast() {
    return p;
  }

  uint8_t get(size_t i) {
    return p[i];
  }

  size_t getSize() {
    return size;
  }

  ~p_uint8_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  uint8_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
