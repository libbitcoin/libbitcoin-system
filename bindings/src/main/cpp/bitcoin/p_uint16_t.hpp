#ifndef LIBBITCOIN_P_UINT16_T_HPP
#define LIBBITCOIN_P_UINT16_T_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_uint16_t {

public:
  p_uint16_t(size_t n) {
    p = new uint16_t[n];
    size = n;
  }
    
/*  p_uint16_t(uint16_t *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(uint16_t *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new uint16_t[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, uint16_t value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  uint16_t* cast() {
    return p;
  }

  uint16_t get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_uint16_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  uint16_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
