#ifndef LIBBITCOIN_SERIAL_H
#define LIBBITCOIN_SERIAL_H

#include <bitcoin/types.hpp>

namespace libbitcoin {

data_chunk write_compact_size(uint64_t size)
{
    data_chunk data;
    if (size < 253)
    {
        data.push_back(size);
    }
    else if (size <= USHRT_MAX)
    {
        data.push_back(253);
        extend_data(data, uncast_type<uint16_t>(size));
    }
    else if (size <= UINT_MAX)
    {
        data.push_back(254);
        extend_data(data, uncast_type<uint32_t>(size));
    }
    else
    {
        data.push_back(254);
        extend_data(data, uncast_type<uint64_t>(size));
    }
    return data;
}

} // libbitcoin

#endif

