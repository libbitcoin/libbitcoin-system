/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_HASH_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_HASH_WRITER_IPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {
    
// All public methods must rely on protected for stream state except validity.

// constructors
// ----------------------------------------------------------------------------

template <typename OStream>
sha256_writer<OStream>::sha256_writer(OStream& sink) noexcept
  : byte_writer<OStream>(sink)
{
    SHA256Init(&context_);
}

template <typename OStream>
sha256_writer<OStream>::~sha256_writer() noexcept
{
    // Derived virtual destructor called before base destructor.
    flusher();
}

// protected
// ----------------------------------------------------------------------------

template <typename OStream>
void sha256_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) noexcept
{
    SHA256Update(&context_, data, size);
}

template <typename OStream>
void sha256_writer<OStream>::do_flush() noexcept
{
    flusher();
}

// private
// ----------------------------------------------------------------------------

template <typename OStream>
void sha256_writer<OStream>::flusher() noexcept
{
    hash_digest hash;
    SHA256Final(&context_, hash.data());
    byte_writer<OStream>::do_write_bytes(hash.data(), maximum);
    byte_writer<OStream>::do_flush();
}

} // namespace system
} // namespace libbitcoin

#endif
