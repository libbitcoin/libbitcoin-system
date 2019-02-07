#ifndef LIBBITCOIN_P_INT32_T_HPP
#define LIBBITCOIN_P_INT32_T_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_int32_t {

public:
  p_int32_t(size_t n) {
    p = new int32_t[n];
    size = n;
  }
    
/*  p_int32_t(int32_t *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(int32_t *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new int32_t[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, int32_t value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  int32_t* cast() {
    return p;
  }

  int32_t get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_int32_t() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  int32_t *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
