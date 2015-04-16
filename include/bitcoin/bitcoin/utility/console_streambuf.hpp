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
#ifndef LIBBITCOIN_CONSOLE_STREAMBUF_HPP
#define LIBBITCOIN_CONSOLE_STREAMBUF_HPP

#include <streambuf>
#include <string>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

/**
 * An alias for the wide character basic stream buffer type.
 */
typedef std::basic_streambuf<wchar_t> wide_streambuf;

/**
 * An alias for the wide character basic stream buffer traits type.
 */
typedef std::basic_streambuf<wchar_t>::traits_type wide_traits;

/**
 * Class to patch Windows stdin keyboard input, file input is not a problem.
 * Initializes stdout, stderr and stdin for wide stream (utf8 translation).
 */
class BC_API console_streambuf
    : public wide_streambuf
{
public:
    /**
     * Initialize stdio to use utf8 translation on Windows.
     */
    static void initialize();

protected:
    /**
     * Protected construction, use static initialize method.
     */
    console_streambuf(wide_streambuf const& stream_buffer);

    /**
     * Implement alternate console read.
     */
    virtual std::streamsize xsgetn(wchar_t* buffer, std::streamsize size);

    /**
     * Implement alternate console read.
     */
    virtual wide_traits::int_type underflow();

private:
    std::wstring buffer_;
    static bool initialized_;
};

} // namespace libbitcoin

#endif
