#ifndef LIBBITCOIN__UTILITY_DATA_SLICE_HPP
#define LIBBITCOIN__UTILITY_DATA_SLICE_HPP

#include <cstdint>
#include <bitcoin/bitcoin/utility/array_slice.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/p_uint8_t.hpp>
//#include <bitcoin/bitcoin/utility/array_slice.hpp>

namespace libbitcoin {
namespace api {

class utility_data_slice {

public:
	utility_data_slice(libbitcoin::api::p_uint8_t& data, size_t begin, size_t end) :
			value_(new libbitcoin::data_slice(data.cast()+begin, data.cast()+end)) {
  }

  libbitcoin::data_slice* getValue() {
        return value_;
    }

    void setValue(libbitcoin::data_slice *value) {
        value_ = value;
    }
private:
    libbitcoin::data_slice *value_;

};

} // namespace api
} // namespace libbitcoin
#endif
