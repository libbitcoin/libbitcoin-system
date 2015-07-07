/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SHARED_CONST_BUFFER_HPP
#define LIBBITCOIN_SHARED_CONST_BUFFER_HPP

#include <memory>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace network {

// TODO: split into cpp file.
// A reference-counted non-modifiable buffer class.
class BC_API shared_const_buffer
{
public:
    // Implement the ConstBufferSequence requirements.
    typedef boost::asio::const_buffer value_type;
    typedef const value_type* const_iterator;

     // Construct from a stream object.
    explicit shared_const_buffer(const data_chunk& data)
      : data_(std::make_shared<data_chunk>(data.begin(), data.end())),
        buffer_(boost::asio::buffer(*data_))
    {
    }

    const_iterator begin() const
    {
        return &buffer_;
    }

    const_iterator end() const
    {
        return &buffer_ + 1;
    }

private:
    std::shared_ptr<data_chunk> data_;
    value_type buffer_;
};

} // namespace network
} // namespace libbitcoin

#endif

