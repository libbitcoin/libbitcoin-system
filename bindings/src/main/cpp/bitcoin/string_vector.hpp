#ifndef LIBBITCOIN__STRING_VECTOR_HPP
#define LIBBITCOIN__STRING_VECTOR_HPP

#include <vector>
#include <string>

namespace libbitcoin {
namespace api {

class string_vector {
public:
  string_vector() : vector_(new std::vector<std::string>){
  }

  void set(size_t i, std::string value) {
      vector_[i] = value;
  }

  const std::vector<std::string> cast() const {
    return vector_;
  }

  std::string get(size_t i) {
    return vector_[i];
  }

  size_t getSize() {
    return vector_.size();
  }

  ~string_vector() {
	vector_.clear();
  }

private:
  std::vector<std::string> vector_;
};

} // namespace api
} // namespace libbitcoin
#endif
