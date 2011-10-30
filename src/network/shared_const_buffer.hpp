#ifndef LIBBITCOIN_NET_SHARED_CONST_BUFFER_H
#define LIBBITCOIN_NET_SHARED_CONST_BUFFER_H

#include <boost/asio.hpp>

#include <bitcoin/types.hpp>
#include <bitcoin/util/serializer.hpp>

namespace libbitcoin {

// A reference-counted non-modifiable buffer class.
class shared_const_buffer
{
public:
    // Construct from a stream object
    explicit shared_const_buffer(const data_chunk& user_data)
      : data(new data_chunk(user_data.begin(), user_data.end())),
        buffer(boost::asio::buffer(*data))
    {
    }

    // Implement the ConstBufferSequence requirements.
    typedef boost::asio::const_buffer value_type;
    typedef const value_type* const_iterator;

    const const_iterator begin() const
    { 
        return &buffer; 
    }
    const const_iterator end() const 
    { 
        return &buffer + 1; 
    }

private:
    boost::shared_ptr<data_chunk> data;
    value_type buffer;
};

} // libbitcoin

#endif

