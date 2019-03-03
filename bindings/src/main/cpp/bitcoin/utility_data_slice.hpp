#ifndef LIBBITCOIN__UTILITY_DATA_SLICE_HPP
#define LIBBITCOIN__UTILITY_DATA_SLICE_HPP

#include <cstdint>
#include <bitcoin/bitcoin/utility/array_slice.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/utility/array_slice.hpp>

namespace libbitcoin {
namespace api {

class utility_data_slice {

public:
  utility_data_slice() : value(new libbitcoin::data_slice()){
  }

  const libbitcoin::data_slice cast() const {
    return value;
  }

//  size_t getSize() {
//    return data_.size();
//  }

//  ~utility_data_slice() {
//	data_.clear();
//  }

  libbitcoin::data_slice getValue() {
        return value;
    }

    void setValue(libbitcoin::data_slice value) {
        this->value = value;
    }
private:
    libbitcoin::data_slice value;

};

} // namespace api
} // namespace libbitcoin
#endif
