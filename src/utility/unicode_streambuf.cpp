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
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/unicode.hpp>

namespace libbitcoin {
    
unicode_streambuf::unicode_streambuf(wide_streambuf* wide_buffer)
    : wide_buffer_(wide_buffer)
{
    // Input buffer is not yet populated.
    setg(narrow_, narrow_, narrow_);

    // Output buffer is underexposed by 1 byte to accomodate the overflow byte.
    setp(narrow_, &narrow_[narrow_size_ - 1]);
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
    // Read from the wide input buffer.
    const auto read = wide_buffer_->sgetn(wide_, from_wide_characters_);

    // Handle read termination.
    if (read == 0)
        return traits_type::eof();

    // These values are statically guarded to be < maxint32.
    const auto wide_size = static_cast<int>(read);
    const auto narrow_size = static_cast<int>(narrow_size_);

    // Convert utf16 to utf8, returning bytes written.
    const auto bytes = to_utf8(narrow_, narrow_size, wide_, wide_size);

    // Reset the buffered portion of the input sequence.
    setg(narrow_, narrow_, &narrow_[bytes]);

    // Return the first character in the input sequence.
    return traits_type::to_int_type(*gptr());
}

// Write characters to output buffer.
// We compensate for character-splitting. This is necessary because
// MSVC does not support a UTF8 locale and as such streams interpret
// narrow characters in the default locale. This implementation
// assumes the stream will treat each byte of a multibyte narrow 
// chracter as an individual single byte character.
std::streambuf::int_type unicode_streambuf::overflow(
    std::streambuf::int_type byte)
{
    // Add a single explicitly read byte to the buffer.
    // The narrow buffer is underexposed by 1 byte to accomodate this.
    if (byte != traits_type::eof())
    {
        *pptr() = byte;
        pbump(1);
    }

    // This will be in the range 0..4, indicating the number of bytes that were
    // not read in the conversion. A nonzero value results when the buffer 
    // terminates within a utf8 multiple byte character.
    uint8_t unread = 0;

    // Get the number of bytes in the buffer to convert.
    BITCOIN_ASSERT(pptr() >= pbase());

    // These values are statically guarded to be <= maxint32.
    const auto narrow_size = static_cast<int>(pptr() - pbase());
    const auto wide_size = static_cast<int>(to_wide_characters_);

    if (narrow_size != 0)
    {
        // Convert utf8 to utf16, returning chars written and bytes unread.
        const auto chars = to_utf16(wide_, wide_size, narrow_, narrow_size,
            unread);

        // Write to the wide output buffer.
        const auto written = wide_buffer_->sputn(wide_, chars);

        // Handle write failure as an EOF.
        if (written != chars)
            return traits_type::eof();
    }

    // Reset the buffered portion of the output sequence.
    // The buffer is underexposed by 1 byte to accomodate the overflow byte.
    setp(narrow_, &narrow_[narrow_size_ - 1]);

    // Copy the fractional character to the beginning of the buffer.
    memcpy(narrow_, &narrow_[narrow_size - unread], unread);

    // Start any subsequent writes immediately after the fractional character.
    pbump(unread);

    // Return the overflow byte or EOF sentinel.
    return byte;
};

// Flush our output sequence.
int unicode_streambuf::sync()
{
    const int success = 0;
    const int failure = -1;

    // We expect EOF to be returned, because we passed it.
    if (overflow(traits_type::eof()) == traits_type::eof())
        return success;

    return failure;
}

} // namespace libbitcoin
