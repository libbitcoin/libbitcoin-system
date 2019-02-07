#ifndef LIBBITCOIN_P_UINT32_T_HPP
#define LIBBITCOIN_P_UINT32_T_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_uint32_t {

public:
  p_uint32_t(size_t n) {
    p = new uint32_t[n];
    size = n;
  }
    
/*  p_uint32_t(uint32_t *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(uint32_t *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new uint32_t[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, uint32_t value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  uint32_t* cast() {
    return p;
  }

  uint32_t get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_uint32_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  uint32_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
