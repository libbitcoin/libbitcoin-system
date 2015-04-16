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
#include <bitcoin/bitcoin/utility/unicode_streambuf.hpp>

#include <cstddef>
#include <iostream>
#include <streambuf>

#ifdef _MSC_VER
    #include <windows.h>
#else
    #include <string>
    #include <string.h>
    #include <boost/locale/encoding_errors.hpp>
    #include <bitcoin/bitcoin/utility/unicode.hpp>
#endif

namespace libbitcoin {
    
unicode_streambuf::unicode_streambuf(wide_streambuf* wide_buffer)
    : wide_buffer_(wide_buffer)
{
    setg(narrow_, narrow_, narrow_);
    setp(narrow_, &narrow_[narrow_size_]);
}

unicode_streambuf::~unicode_streambuf()
{
    sync();
}

// Read characters from input buffer.
// This invokes wide_buffer_.xsgetn() which requires a patch for
// console (keyboard) input on Windows, so ensure this class is
// initialized with a patched std::wcin when std::wcin is used.
std::streambuf::int_type unicode_streambuf::underflow()
{
    auto read = wide_buffer_->sgetn(wide_, from_wide_characters_);
    if (read == 0)
        return traits_type::eof();

    size_t size = 0;

    // Convert from wide to narrow.
#ifdef _MSC_VER
    // This is an optimization for the expected common usage (Windows).
    size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide_,
        static_cast<int>(read), narrow_, static_cast<int>(narrow_size_), NULL,
        NULL);
#else
    const std::wstring wide(wide_, &wide_[read]);
    const auto narrow = to_utf8(wide);
    size = narrow.size();
    memcpy(narrow_, narrow.data(), size);
#endif

    if (size == 0)
        throw std::istream::failure("utf-16 to utf-8 conversion fail");

    // Reset the buffered portion of the input sequence.
    setg(narrow_, narrow_, &narrow_[size]);

    // Return the first character in the input sequence.
    return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type unicode_streambuf::overflow(
    std::streambuf::int_type value)
{
    if (value != traits_type::eof())
    {
        *pptr() = value;
        pbump(1);
    }

    uint8_t offset = 0;

    // The value of write is no larger than narrow_size_.
    std::ptrdiff_t write = pptr() - pbase();
    if (write != 0)
    {
        size_t size = 0;

        // Prevent character-splitting.
        while (offset < character_size_)
        {
            // Convert from narrow to wide.
#ifdef _MSC_VER
            // This is an optimization for the expected common usage (Windows).
            size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, narrow_,
                static_cast<int>(write - offset), wide_,
                static_cast<int>(to_wide_characters_));

            if (size == 0 && GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
                ++offset;
            else
                break;
#else
            const std::string narrow(narrow_, &narrow_[write - offset]);
            try
            {
                const auto wide = to_utf16(narrow);
                size = wide.size();
                wmemcpy(wide_, wide.data(), size);
                break;
            }
            catch (const boost::locale::conv::conversion_error&)
            {
                size = 0;
                ++offset;
            }
#endif
        }

        if (size == 0)
            throw std::ostream::failure("utf-8 to utf-16 conversion fail");

        // Write wide to output buffer.
        auto written = wide_buffer_->sputn(wide_, size);
        if (written != size)
            return traits_type::eof();
    }

    // Reset the buffered portion of the output sequence.
    setp(narrow_, &narrow_[narrow_size_]);

    // Copy the fractional character to the beginning of the buffer.
    memcpy(narrow_, &narrow_[write - offset], offset);

    // Continue writing at the end of the fractional character.
    pbump(offset);

    // Return whether value is not equivalent to eof.
    return traits_type::not_eof(value);
};

int unicode_streambuf::sync()
{
    const int success = 0;
    const int failure = -1;

    // Flush our output sequence.
    auto value = overflow(traits_type::eof());

    if (traits_type::eq_int_type(value, traits_type::eof()))
        return failure;

    return success;
}

} // namespace libbitcoin
