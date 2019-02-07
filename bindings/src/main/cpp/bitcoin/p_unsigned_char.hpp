#ifndef LIBBITCOIN_P_UNSIGNED_CHAR_HPP
#define LIBBITCOIN_P_UNSIGNED_CHAR_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_unsigned_char {

public:
  p_unsigned_char(size_t n) {
    p = new unsigned char[n];
    size = n;
  }
    
/*  p_unsigned_char(unsigned char *value, unsigned int n) {
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
  void set(size_t i, unsigned char value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  unsigned char* cast() {
    return p;
  }

  unsigned char get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_unsigned_char() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  unsigned char *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
