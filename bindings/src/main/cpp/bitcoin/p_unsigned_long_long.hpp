#ifndef LIBBITCOIN_P_UNSIGNED_LONG_LONG_HPP
#define LIBBITCOIN_P_UNSIGNED_LONG_LONG_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_unsigned_long_long {

public:
  p_unsigned_long_long(size_t n) {
    p = new unsigned char[n];
    size = n;
  }
    
/*  p_unsigned_long_long(unsigned long long *value, size_t n) {
    p = value;
    size = n;
  }

  void set(unsigned long long *value, size_t n) {
    if (p) delete [] p;
    size = n;
    p = new unsigned char[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, unsigned long long value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  unsigned long long* cast() {
    return p;
  }

  unsigned long long get(size_t i) {
    return p[i];
  }

  size_t getSize() {
    return size;
  }

  ~p_unsigned_long_long() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  unsigned long long *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
