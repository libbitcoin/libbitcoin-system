#ifndef LIBBITCOIN_UNSIGNED_CHAR_VECTOR_HPP
#define LIBBITCOIN_UNSIGNED_CHAR_VECTOR_HPP

#include <cstddef>
#include <vector>

namespace libbitcoin {
namespace api {

using namespace std;

class unsigned_char_vector {

public:
  unsigned_char_vector() : vector_(new std::vector<unsigned char>){
  }
    
/*  unsigned_char_vector(std::vector<unsigned char>value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(std::vector<unsigned char>value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new unsigned char[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, unsigned char value) {
      (*vector_)[i] = value;
  }

  const std::vector<unsigned char> *cast() const {
    return vector_;
  }

  unsigned char get(size_t i) {
    return (*vector_)[i];
  }

  size_t getSize() {
    return vector_->size();
  }

  ~unsigned_char_vector() {
	vector_->clear();
	delete vector_;
  }

private:
  std::vector<unsigned char> *vector_;
};

} // namespace api
} // namespace libbitcoin
#endif
