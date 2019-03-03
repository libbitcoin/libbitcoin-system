#ifndef LIBBITCOIN__UTILITY_DATA_CHUNK_HPP
#define LIBBITCOIN__UTILITY_DATA_CHUNK_HPP

#include <uint8_t_vector.hpp>

namespace libbitcoin {
namespace api {

class utility_data_chunk {
public:
	uint8_t_vector getValue() {
        return value;
    }

    void setValue(uint8_t_vector value) {
        this->value = value;
    }
private:
    uint8_t_vector value;
};

} // api
} // libbitcoin

#endif
