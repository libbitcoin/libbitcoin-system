#ifndef LIBBITCOIN_P_BOOL_HPP
#define LIBBITCOIN_P_BOOL_HPP

#include <cstddef>
#include <iostream>

namespace libbitcoin {
namespace api {

class p_bool {

public:
  p_bool(size_t n) {
    p = new bool[n];
    size = n;
  }
    
/*  p_bool(bool *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(bool *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new bool[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, bool value) {
    if (i<size) {
      p[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;" << size << ")\n";
    }
  }

  bool* cast() {
    return p;
  }

  bool get(size_t i) {
    return p[i];
  }

  unsigned int getSize() {
    return size;
  }

  ~p_bool() {
	std::cout << "p=" << (long long) p << "\n";
//    if (p) delete [] p;
    p = NULL;
	std::cout << "p=" << (long long) p << "\n";
    size = 0;
  }

private:
  bool *p;
  size_t size;  
};

} // namespace api
} // namespace libbitcoin
#endif
