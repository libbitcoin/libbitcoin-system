/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256T_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256T_WRITER_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {

// constructors
// ----------------------------------------------------------------------------

template <text_t Tag, typename OStream>
sha256t_writer<Tag, OStream>::sha256t_writer(OStream& sink) NOEXCEPT
  : byte_writer<OStream>(sink), context_(midstate(), one)
{
}

template <text_t Tag, typename OStream>
sha256t_writer<Tag, OStream>::~sha256t_writer() NOEXCEPT
{
    // Derived virtual destructor called before base destructor.
    flusher();
}

// protected
// ----------------------------------------------------------------------------

template <text_t Tag, typename OStream>
void sha256t_writer<Tag, OStream>::do_write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    // Hash overflow produces update false, which requires (2^64-8)/8 bytes.
    // The stream could be invalidated, but writers shouldn't have to check it.
    context_.write(size, data);
}

template <text_t Tag, typename OStream>
void sha256t_writer<Tag, OStream>::do_flush() NOEXCEPT
{
    flusher();
    byte_writer<OStream>::do_flush();
}

// private
// ----------------------------------------------------------------------------

// static
template <text_t Tag, typename OStream>
constexpr sha256::state_t sha256t_writer<Tag, OStream>::midstate() NOEXCEPT
{
    // Cache midstate of tagged hash part that does not change for a given tag.
    // sha256(sha256(tag) || sha256(tag) || message) [bip340].
    constexpr auto tag1 = sha256::simple_hash(Tag.data);
    constexpr auto tag2 = sha256::midstate(tag1, tag1);
    return tag2;
}

// Only hash overflow returns update false, which requires (2^64-8)/8 bytes.
// The stream could invalidate, but writers shouldn't have to check this.
template <text_t Tag, typename OStream>
void sha256t_writer<Tag, OStream>::flusher() NOEXCEPT
{
    hash_digest hash{};

    // Finalize streaming hash.
    context_.flush(hash.data());
    context_.reset(midstate(), one);

    // Write hash to stream.
    byte_writer<OStream>::do_write_bytes(hash.data(), hash_size);
}

} // namespace system
} // namespace libbitcoin

#endif
