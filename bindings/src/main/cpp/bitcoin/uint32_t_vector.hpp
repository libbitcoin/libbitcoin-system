#ifndef LIBBITCOIN_UINT32_T_VECTOR_HPP
#define LIBBITCOIN_UINT32_T_VECTOR_HPP

#include <cstdint>
#include <vector>

namespace libbitcoin {
namespace api {

class uint32_t_vector {


public:
  uint32_t_vector() : vector_(new std::vector<std::uint32_t>){
  }
    
/*  uint32_t_vector(std::vector<uint32_t>value, size_t n) {
    p = value;
    size = n;
  }

  void set(std::vector<uint32_t>value, size_t n) {
    if (p) delete [] p;
    size = n;
    p = new uint32_t[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, std::uint32_t value) {
      (*vector_)[i] = value;
  }

  const std::vector<std::uint32_t> *cast() const {
    return vector_;
  }

  std::uint32_t get(size_t i) {
    return (*vector_)[i];
  }

  size_t getSize() {
    return vector_->size();
  }

  ~uint32_t_vector() {
	vector_->clear();
	delete vector_;
  }

private:
  std::vector<std::uint32_t> *vector_;
};

} // namespace api
} // namespace libbitcoin
#endif
