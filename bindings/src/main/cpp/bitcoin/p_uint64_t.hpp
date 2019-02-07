#ifndef LIBBITCOIN_P_UINT64_T_HPP
#define LIBBITCOIN_P_UINT64_T_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_uint64_t {

public:
  p_uint64_t(size_t n) {
    p = new uint64_t[n];
    size = n;
  }
    
/*  p_uint64_t(uint64_t *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(uint64_t *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new uint64_t[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, uint64_t value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  uint64_t* cast() {
    return p;
  }

  uint64_t get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_uint64_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  uint64_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
