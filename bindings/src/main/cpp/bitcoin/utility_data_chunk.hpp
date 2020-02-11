#ifndef LIBBITCOIN__UTILITY_DATA_CHUNK_HPP
#define LIBBITCOIN__UTILITY_DATA_CHUNK_HPP

#include <bitcoin/uint8_t_vector.hpp>

namespace libbitcoin {
namespace api {

class utility_data_chunk {
public:
	uint8_t_vector* getValue() {
        return value_;
    }

    void setValue(uint8_t_vector* value) {
        value_ = value;
    }
private:
    uint8_t_vector* value_;
};

} // api
} // libbitcoin

#endif
