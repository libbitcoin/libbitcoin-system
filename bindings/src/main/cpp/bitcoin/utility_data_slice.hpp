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
	utility_data_slice(p_uint8_t& data, size_t begin, size_t end) :
			value(new libbitcoin::data_slice(data.cast()+begin, data.cast()+end)) {
  }

  libbitcoin::data_slice* getValue() {
        return value;
    }

    void setValue(libbitcoin::data_slice *value) {
        this->value = value;
    }
private:
    libbitcoin::data_slice *value;

};

} // namespace api
} // namespace libbitcoin
#endif
