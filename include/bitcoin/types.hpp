#ifndef LIBBITCOIN_TYPES_H
#define LIBBITCOIN_TYPES_H

#include <boost/asio.hpp>
#include <memory>
#include <vector>

#include <bitcoin/net/types.hpp>
#include <bitcoin/storage/types.hpp>

namespace libbitcoin {

using std::shared_ptr;
using boost::asio::io_service;

typedef shared_ptr<io_service> service_ptr;
typedef shared_ptr<io_service::work> work_ptr;
typedef shared_ptr<io_service::strand> strand_ptr;

typedef unsigned char byte;
typedef std::vector<byte> data_chunk;

template<typename T>
T cast_chunk(const data_chunk& chunk)
{
    T val = 0;
    for (size_t i = 0; i < sizeof(T); ++i)
        val += (chunk[i] << i*8);
    return val;
}

} // libbitcoin

#endif


