/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_IPP

#include <algorithm>
#include <ios>
#include <istream>
#include <limits>
#include <memory>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {
    
// Suppress istream members and allocator may throw inside NOEXCEPT.
// The intended behavior in this case is program abort.
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// private/static
template <typename IStream>
inline typename byte_reader<IStream>::memory_arena
byte_reader<IStream>::default_arena() NOEXCEPT
{
    return bc::default_arena::get();
}

// All public methods must rely on protected for stream state except validity.

// constructors
// ----------------------------------------------------------------------------

template <typename IStream>
byte_reader<IStream>::byte_reader(IStream& source,
    const memory_arena& arena) NOEXCEPT
  : stream_(source),
    remaining_(system::maximum<size_t>),
    allocator_(arena)
{
    ////BC_ASSERT_MSG(stream_.exceptions() == IStream::goodbit,
    ////    "Input stream must not be configured to throw exceptions.");
}

// big endian
// ----------------------------------------------------------------------------

template <typename IStream>
template <typename Integer, size_t Size,
    if_integer<Integer>, if_not_greater<Size, sizeof(Integer)>>
Integer byte_reader<IStream>::read_big_endian() NOEXCEPT
{
    Integer value{};
    auto& bytes = byte_cast(value);

    // Call into virtual reader (vs. stream) so derived class can reuse.
    read_bytes(std::next(bytes.data(), sizeof(Integer) - Size), Size);
    return native_from_big_end(value);
}

template <typename IStream>
uint16_t byte_reader<IStream>::read_2_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint16_t>();
}

template <typename IStream>
uint32_t byte_reader<IStream>::read_3_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint32_t, 3>();
}

template <typename IStream>
uint32_t byte_reader<IStream>::read_4_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint32_t>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_5_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint64_t, 5>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_6_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint64_t, 6>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_7_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint64_t, 7>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_8_bytes_big_endian() NOEXCEPT
{
    return read_big_endian<uint64_t>();
}

// little endian
// ----------------------------------------------------------------------------

template <typename IStream>
template <typename Integer, size_t Size,
    if_integer<Integer>, if_not_greater<Size, sizeof(Integer)>>
Integer byte_reader<IStream>::read_little_endian() NOEXCEPT
{
    Integer value{};
    auto& bytes = byte_cast(value);

    // Call into virtual reader (vs. stream) so derived class can reuse.
    read_bytes(bytes.data(), Size);
    return native_from_little_end(value);
}

template <typename IStream>
uint16_t byte_reader<IStream>::read_2_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint16_t>();
}

template <typename IStream>
uint32_t byte_reader<IStream>::read_3_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint32_t, 3>();
}

template <typename IStream>
uint32_t byte_reader<IStream>::read_4_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint32_t>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_5_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint64_t, 5>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_6_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint64_t, 6>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_7_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint64_t, 7>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_8_bytes_little_endian() NOEXCEPT
{
    return read_little_endian<uint64_t>();
}

template <typename IStream>
uint64_t byte_reader<IStream>::read_variable() NOEXCEPT
{
    switch (const auto value = read_byte())
    {
        case varint_eight_bytes:
            return read_8_bytes_little_endian();
        case varint_four_bytes:
            return read_4_bytes_little_endian();
        case varint_two_bytes:
            return read_2_bytes_little_endian();
        default:
            return value;
    }
}

template <typename IStream>
size_t byte_reader<IStream>::read_size(size_t limit) NOEXCEPT
{
    const auto size = read_variable();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size > limit)
    {
        invalid();
        return zero;
    }
 
    return possible_narrow_cast<size_t>(size);
}

template <typename IStream>
code byte_reader<IStream>::read_error_code() NOEXCEPT
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_t>(value));
}

template <typename IStream>
uint8_t byte_reader<IStream>::peek_byte() NOEXCEPT
{
    return do_peek_byte();
}

template <typename IStream>
uint8_t byte_reader<IStream>::read_byte() NOEXCEPT
{
    uint8_t value = pad();
    do_read_bytes(&value, one);
    return value;
}

// byte arrays
// ----------------------------------------------------------------------------

template <typename IStream>
template <size_t Size>
data_array<Size> byte_reader<IStream>::read_forward() NOEXCEPT
{
    // Truncated bytes are populated with 0x00.
    // Reader supports directly populating an array, this avoids a copy.
    data_array<Size> out{};
    do_read_bytes(out.data(), Size);
    return out;
}

template <typename IStream>
template <size_t Size>
data_array<Size> byte_reader<IStream>::read_reverse() NOEXCEPT
{
    return system::reverse(read_forward<Size>());
}

template <typename IStream>
template <size_t Size>
data_array_cptr<Size> byte_reader<IStream>::read_forward_cptr() NOEXCEPT
{
    if (!valid())
        return {};

    const auto cptr = to_allocated<data_array<Size>>(get_arena());
    if (!cptr)
    {
        invalidate();
        return cptr;
    }

    // Truncated bytes are populated with 0x00.
    // Reader supports directly populating an array, this avoids a copy.
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    do_read_bytes(to_non_const_raw_ptr(cptr)->data(), Size);
    BC_POP_WARNING()

    if (!valid())
        return {};

    return cptr;
}

template <typename IStream>
template <size_t Size>
data_array_cptr<Size> byte_reader<IStream>::read_reverse_cptr() NOEXCEPT
{
    const auto cptr = read_forward_cptr<Size>();
    if (!cptr)
        return cptr;

    // Guarded above.
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    const auto ptr = to_non_const_raw_ptr(cptr);
    BC_POP_WARNING()

    std::reverse(ptr->begin(), ptr->end());
    return cptr;
}

template <typename IStream>
mini_hash byte_reader<IStream>::read_mini_hash() NOEXCEPT
{
    return read_forward<mini_hash_size>();
}

template <typename IStream>
short_hash byte_reader<IStream>::read_short_hash() NOEXCEPT
{
    return read_forward<short_hash_size>();
}

template <typename IStream>
hash_digest byte_reader<IStream>::read_hash() NOEXCEPT
{
    return read_forward<hash_size>();
}

template <typename IStream>
long_hash byte_reader<IStream>::read_long_hash() NOEXCEPT
{
    return read_forward<long_hash_size>();
}

template <typename IStream>
mini_hash_cptr byte_reader<IStream>::read_mini_hash_cptr() NOEXCEPT
{
    return read_forward_cptr<mini_hash_size>();
}

template <typename IStream>
short_hash_cptr byte_reader<IStream>::read_short_hash_cptr() NOEXCEPT
{
    return read_forward_cptr<short_hash_size>();
}

template <typename IStream>
hash_cptr byte_reader<IStream>::read_hash_cptr() NOEXCEPT
{
    return read_forward_cptr<hash_size>();
}

template <typename IStream>
long_hash_cptr byte_reader<IStream>::read_long_hash_cptr() NOEXCEPT
{
    return read_forward_cptr<long_hash_size>();
}

// byte vectors
// ----------------------------------------------------------------------------

template <typename IStream>
data_chunk byte_reader<IStream>::read_bytes() NOEXCEPT
{
    // Count bytes to the end, avoids push_back reallocations.
    size_t size{};
    while (!get_exhausted())
    {
        ++size;
        skip_byte();
    };

    rewind_bytes(size);
    return read_bytes(size);
}

template <typename IStream>
chunk_cptr byte_reader<IStream>::read_bytes_cptr() NOEXCEPT
{
    // Count bytes to the end, avoids push_back reallocations.
    size_t size{};
    while (!get_exhausted())
    {
        ++size;
        skip_byte();
    };

    rewind_bytes(size);
    return read_bytes_cptr(size);
}

template <typename IStream>
data_chunk byte_reader<IStream>::read_bytes(size_t size) NOEXCEPT
{
    if (is_zero(size))
        return {};

    // This allows caller to read an invalid stream without allocation.
    if (!valid())
        return {};

    data_chunk out(size);
    do_read_bytes(out.data(), size);
    return out;
}

template <typename IStream>
chunk_cptr byte_reader<IStream>::read_bytes_cptr(size_t size) NOEXCEPT
{
    // This allows caller to read an invalid stream without allocation.
    if (!valid())
        return {};

    // TODO: bypass vector byte fill.
    const auto cptr = to_allocated<data_chunk>(get_arena(), size);
    if (!cptr)
    {
        invalidate();
        return cptr;
    }

    if (is_zero(size))
        return cptr;

    // Guarded above.
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    do_read_bytes(to_non_const_raw_ptr(cptr)->data(), size);
    BC_POP_WARNING()

    if (!valid())
        return {};

    return cptr;
}

template <typename IStream>
data_chunk* byte_reader<IStream>::read_bytes_raw(size_t size) NOEXCEPT
{
    // This allows caller to read an invalid stream without allocation.
    if (!valid())
        return nullptr;

    // TODO: bypass vector byte fill.
    const auto raw = allocator_.new_object<data_chunk>(size);
    if (raw == nullptr)
    {
        invalidate();
        return raw;
    }

    if (is_zero(size))
        return raw;

    do_read_bytes(raw->data(), size);
    if (!valid())
    {
        allocator_.delete_object<data_chunk>(raw);
        return nullptr;
    }

    return raw;
}

template <typename IStream>
void byte_reader<IStream>::read_bytes(uint8_t* buffer, size_t size) NOEXCEPT
{
    do_read_bytes(buffer, size);
}

// strings
// ----------------------------------------------------------------------------

template <typename IStream>
std::string byte_reader<IStream>::read_string(size_t limit) NOEXCEPT
{
    return read_string_buffer(read_size(limit));
}

template <typename IStream>
std::string byte_reader<IStream>::read_string_buffer(size_t size) NOEXCEPT
{
    // Isolating get_exhausted to first call is an optimization (must clear).
    if (get_exhausted())
        return {};

    // This will produce one (zero) terminating byte if size exceeds available.
    std::string out{};
    out.reserve(add1(size));
    while (!is_zero(size--) && valid())
        out.push_back(read_byte());

    // Removes zero and all after, required for bitcoin string deserialization.
    const auto position = out.find('\0');
    out.resize(position == std::string::npos ? out.size() : position);
    ////out.shrink_to_fit();

    clear();
    return out;
}

// streams
// ----------------------------------------------------------------------------

template <typename IStream>
std::ostream& byte_reader<IStream>::read(std::ostream& out) NOEXCEPT
{
    // This creates an intermediate buffer the size of the stream.
    // This is presumed to be more optimal than looping individual bytes.
    byte_writer<std::ostream>(out).write_bytes(read_bytes());
    return out;
}

// context
// ----------------------------------------------------------------------------

template <typename IStream>
void byte_reader<IStream>::skip_byte() NOEXCEPT
{
    do_skip_bytes(one);
}

template <typename IStream>
void byte_reader<IStream>::skip_bytes(size_t size) NOEXCEPT
{
    do_skip_bytes(size);
}

template <typename IStream>
void byte_reader<IStream>::skip_variable() NOEXCEPT
{
    switch (read_byte())
    {
        case varint_eight_bytes:
            do_skip_bytes(8);
            return;
        case varint_four_bytes:
            do_skip_bytes(4);
            return;
        case varint_two_bytes:
            do_skip_bytes(2);
            return;
        default:
            return;
    }
}

template <typename IStream>
void byte_reader<IStream>::rewind_byte() NOEXCEPT
{
    do_rewind_bytes(one);
}

template <typename IStream>
void byte_reader<IStream>::rewind_bytes(size_t size) NOEXCEPT
{
    do_rewind_bytes(size);
}

template <typename IStream>
void byte_reader<IStream>::set_position(size_t absolute) NOEXCEPT
{
    // Clear a presumed error state following a read overflow.
    clear();

    // This allows conversion of and absolute to relative position.
    const auto position = get_read_position();

    if (absolute == position)
        return;

    if (absolute > position)
        do_skip_bytes(absolute - position);
    else
        do_rewind_bytes(position - absolute);
}

template <typename IStream>
bool byte_reader<IStream>::is_exhausted() const NOEXCEPT
{
    // True if invalid or if no bytes remain in the stream.
    return get_exhausted();
}

// control
// ----------------------------------------------------------------------------
// These only call non-virtual (private) methods.

template <typename IStream>
size_t byte_reader<IStream>::get_read_position() NOEXCEPT
{
    return getter();
}

template <typename IStream>
void byte_reader<IStream>::set_limit(size_t size) NOEXCEPT
{
    limit(size);
}

template <typename IStream>
void byte_reader<IStream>::invalidate() NOEXCEPT
{
    // Permanently invalidate the stream/reader.
    invalid();
}

template <typename IStream>
byte_reader<IStream>::operator bool() const NOEXCEPT
{
    // True if any call created an error state, even if there have been
    // subsequent calls, or if any error state preexists on the stream.
    return valid();
}

// This should not be necessary with bool() defined, but it is.
template <typename IStream>
bool byte_reader<IStream>::operator!() const NOEXCEPT
{
    return !valid();
}

template <typename IStream>
typename byte_reader<IStream>::memory_arena
byte_reader<IStream>::get_arena() const NOEXCEPT
{
    return allocator_.resource();
}

template <typename IStream>
typename byte_reader<IStream>::memory_allocator&
byte_reader<IStream>::get_allocator() const NOEXCEPT
{
    return allocator_;
}

// protected virtual
// ----------------------------------------------------------------------------
// These may only call non-virtual (private) methods (due to overriding).

template <typename IStream>
uint8_t byte_reader<IStream>::do_peek_byte() NOEXCEPT
{
    if (limiter(one))
        return pad();

    // This sets eofbit (or badbit) on empty and eofbit if otherwise at end.
    // eofbit does not cause !!eofbit == true, but badbit does, so we validate
    // the call the achieve consistent behavior. The reader will be invalid if
    // the stream is peeked past end, including when empty.
    const auto value =
        possible_narrow_and_sign_cast<uint8_t>(stream_.peek());

    validate();
    return valid() ? value : pad();
}

template <typename IStream>
void byte_reader<IStream>::do_read_bytes(uint8_t* buffer, size_t size) NOEXCEPT
{
    // Limited reads are not partially filled or padded.
    if (limiter(size))
        return;

    // It is not generally more efficient to call stream_.get() for one byte.
    // Partially-failed reads here will be populated by the stream, not padded.
    // However, both copy_source and stringstream will zero-fill partial reads.
    // Read on empty is inconsistent, so validate the result. The reader will be
    // invalid if the stream is get past end, including when empty.

    // Read past stream end invalidates stream unless size exceeds maximum.
    BC_ASSERT(size <= maximum);
    stream_.read(pointer_cast<char>(buffer),
        possible_narrow_and_sign_cast<typename IStream::pos_type>(size));

    validate();
}

template <typename IStream>
void byte_reader<IStream>::do_skip_bytes(size_t size) NOEXCEPT
{
    if (limiter(size))
        return;

    // Skip past stream end invalidates stream unless size exceeds maximum.
    BC_ASSERT(size <= maximum);
    seeker(possible_narrow_and_sign_cast<typename IStream::pos_type>(size));
}

template <typename IStream>
void byte_reader<IStream>::do_rewind_bytes(size_t size) NOEXCEPT
{
    // Given that the stream size is unknown to the reader, the limit may be
    // arbitrarily high. This prevents an overflow if sum exceeds max_size_t.
    // max_size_t is presumed to exceed the range of IStream::pos_type, in 
    // which case this constraint does not affect the limiting behavior.
    remaining_ = ceilinged_add(remaining_, size);

    // Rewind past stream start invalidates stream unless size exceeds maximum.
    BC_ASSERT(size <= maximum);
    seeker(-possible_narrow_and_sign_cast<typename IStream::pos_type>(size));
}

template <typename IStream>
bool byte_reader<IStream>::get_exhausted() const NOEXCEPT
{
    if (is_zero(remaining_))
        return true;

    // Empty behavior is broadly inconsistent across implementations.
    // It is also necessary to start many reads, including initial reads, with
    // an exhaustion check, which must be consistent and not state-changing.
    // A state change would preclude testing for errors after testing for end.
    // This method is const because it reliably creates no net state change.
    // Streams are valid unless read or peeked. Peek does not change stream
    // position, so it is used to force the stream into a failure state when
    // empty. peek past end always sets eofbit but peek state on empty is
    // inconsistent across streams, though a flag is always set. eofbit is set
    // by istringstream and badbit is set by boost.

    if (!valid())
        return true;

    // Peek to force error on eof, save condition, restore valid stream state.
    stream_.peek();
    const auto eof = !valid();
    stream_.clear();

    return eof;
}

// private
// ----------------------------------------------------------------------------
// These may only call other private methods (due to overriding).

template <typename IStream>
bool byte_reader<IStream>::valid() const NOEXCEPT
{
    // zero is the istream documented flag for no error.
    return stream_.rdstate() == IStream::goodbit;
}

template <typename IStream>
void byte_reader<IStream>::invalid() NOEXCEPT
{
    // If eofbit is set, failbit is generally set on all operations.
    // badbit is unrecoverable, set the others to ensure consistency.
    stream_.setstate(IStream::eofbit | IStream::failbit | IStream::badbit);
}

template <typename IStream>
void byte_reader<IStream>::validate() NOEXCEPT
{
    // Ensure that any failure in the call fully invalidates the stream/reader.
    // Some errors are recoverable, so a sequence of operations without testing
    // for validity could miss an error on intervening operations. For example,
    // seekg can reset eofbit and read past doesn't set badbit (recoverable).
    if (!valid())
        invalid();
}

template <typename IStream>
void byte_reader<IStream>::clear() NOEXCEPT
{
    // Does not reset the current position.
    stream_.clear();
}

template <typename IStream>
size_t byte_reader<IStream>::getter() NOEXCEPT
{
    static const auto failure = typename IStream::pos_type(-1);
    typename IStream::pos_type position;

    // Force these to be consistent, and avoid propagating exceptions.
    // Assuming behavior is consistent with seekg (as documented).
    // Returns current position on success and pos_type(-1) on failure.
    try
    {
        // This does not honor BOOST_EXCEPTION_DISABLE.
        position = stream_.tellg();
        validate();
    }
    catch (const typename IStream::failure&)
    {
        position = failure;
        invalid();
    }

    // Max size_t is presumed to exceed max IStream::pos_type.
    return position == failure ? zero :
        possible_narrow_and_sign_cast<size_t>(position);
}

template <typename IStream>
void byte_reader<IStream>::limit(size_t size) NOEXCEPT
{
    remaining_ = size;
}

template <typename IStream>
bool byte_reader<IStream>::limiter(size_t size) NOEXCEPT
{
    if (size > remaining_)
    {
        // Does not reset the current position or the remaining limit.
        invalidate();
        return true;
    }

    remaining_ -= size;
    return false;
}

template <typename IStream>
void byte_reader<IStream>::seeker(typename IStream::pos_type offset) NOEXCEPT
{
    // Force these to be consistent by treating zero seek as a no-op.
    // boost/istringstream both succeed on non-empty zero seek.
    // istringstream succeeds on empty zero seek, boost sets failbit.
    if (is_zero(offset))
        return;

    // Force these to be consistent, and avoid propagating exceptions.
    // istringstream sets failbit on non-empty over/underflow, boost throws.
    // boost/istringstream both set failbit on empty over/underflow.
    try
    {
        // This does not honor BOOST_EXCEPTION_DISABLE.
        stream_.seekg(offset, IStream::cur);
        validate();
    }
    catch (const typename IStream::failure&)
    {
        invalid();
    }
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
