#ifndef LIBBITCOIN_NET_SHARED_CONST_BUFFER_H
#define LIBBITCOIN_NET_SHARED_CONST_BUFFER_H

#include <boost/asio.hpp>
#include <vector>

#include <bitcoin/util/serializer.hpp>

namespace libbitcoin {
namespace net {

// A reference-counted non-modifiable buffer class.
class shared_const_buffer
{
public:
  // Construct from a stream object
  explicit shared_const_buffer(const data_chunk& user_data)
    : data(new std::vector<char>(user_data.begin(), user_data.end())),
      buffer(boost::asio::buffer(*data))
  {
  }

  // Implement the ConstBufferSequence requirements.
  typedef boost::asio::const_buffer value_type;
  typedef const boost::asio::const_buffer* const_iterator;
  const boost::asio::const_buffer* begin() const { return &buffer; }
  const boost::asio::const_buffer* end() const { return &buffer + 1; }

private:
  boost::shared_ptr<std::vector<char>> data;
  boost::asio::const_buffer buffer;
};

} // net
} // libbitcoin

#endif

