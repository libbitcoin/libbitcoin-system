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
#ifndef LIBBITCOIN_UNICODE_STREAMS_HPP
#define LIBBITCOIN_UNICODE_STREAMS_HPP

#include <streambuf>
#include <string>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

/**
 * Class to patch Windows stdin keyboard input, file input is not a problem.
 * Limits keyboard input buffer to 1024 bytes, terminated by <ENTER>.
 * Initializes stdout, stderr and stdin for wide stream (utf8 translation).
 */
class BC_API unicode_streams
    : public std::basic_streambuf<wchar_t>
{
public:
    /**
     * Initialize unicode streams.
     */
    static void initialize();

protected:
    typedef std::basic_streambuf<wchar_t> wide_streambuf;
    typedef std::basic_streambuf<wchar_t>::traits_type traits;

    /**
     * Protected construction, use static initialize method.
     */
    unicode_streams(wide_streambuf const& stream_buffer);

    /**
     * Implement alternate console read.
     */
    virtual std::streamsize xsgetn(wchar_t* buffer, std::streamsize size);

    /**
     * Implement alternate console read.
     */
    virtual traits::int_type underflow();

private:
    std::wstring buffer_;
    static bool initialized_;
    static void* get_input_handle();
};

} // namespace libbitcoin

#endif

