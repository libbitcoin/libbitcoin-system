/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_UNICODE_STREAMBUF_HPP
#define LIBBITCOIN_UNICODE_STREAMBUF_HPP

#include <streambuf>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/console_streambuf.hpp>

namespace libbitcoin {

/**
 * Class to translate internal utf8 iostreams to external utf16 iostreams.
 */
class BC_API unicode_streambuf
    : public std::streambuf
{
public:
    /**
     * Construct unicode stream buffer from a weak reference to a wide buffer.
     * @param[in]  wide_buffer  A wide stream buffer for i/o relay.
     */
    unicode_streambuf(wide_streambuf* wide_buffer);

    /**
     * Synchronize stream buffer.
     */
    virtual ~unicode_streambuf();

protected:
    /**
     * Implement underflow for support of input streams.
     */
    virtual std::streambuf::int_type underflow();

    /**
     * Implement overflow for support of output streams.
     * @param[in]  value  Character to be put.
     */
    virtual std::streambuf::int_type overflow(std::streambuf::int_type value);

    /**
     * Implement sync for support of output streams.
     */
    virtual int sync();

private:
    // UTF8 encoding requires up to 4 bytes per character.
    static const size_t character_size_ = 4;

    // The input buffer size in number of characters.
    // This is the min number of 4 byte utf8 characters to fill narrow_size_.
    static const size_t from_wide_characters_ = 256;

    // The input buffer size in bytes.
    static const size_t narrow_size_ = from_wide_characters_ * character_size_;

    // The input buffer size in number of characters.
    // This is maxed out when all wide characters are single byte utf8.
    static const size_t to_wide_characters_ = narrow_size_;

    char narrow_[narrow_size_ + character_size_];
    wchar_t wide_[to_wide_characters_ + character_size_];
    wide_streambuf* wide_buffer_;

    static_assert(narrow_size_ <= MAX_INT32,
        "Narrow buffer must not exceed max int32.");
    static_assert(narrow_size_ >= character_size_,
        "Narrow buffer must be at least 4 bytes wide.");
};

} // namespace libbitcoin

#endif
