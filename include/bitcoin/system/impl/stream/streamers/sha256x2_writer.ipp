/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256X2_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256X2_WRITER_IPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {
    
// All public methods must rely on protected for stream state except validity.

// constructors
// ----------------------------------------------------------------------------

template <typename OStream>
sha256x2_writer<OStream>::sha256x2_writer(OStream& sink) NOEXCEPT
  : byte_writer<OStream>(sink),
    context_{ intrinsics::sha256_initial }
{
    ////intrinsics::sha256_initialize(context_);
}

template <typename OStream>
sha256x2_writer<OStream>::~sha256x2_writer() NOEXCEPT
{
    // Derived virtual destructor called before base destructor.
    flusher();
}

// protected
// ----------------------------------------------------------------------------

template <typename OStream>
void sha256x2_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    intrinsics::sha256_update(context_, data, size);
}

template <typename OStream>
void sha256x2_writer<OStream>::do_flush() NOEXCEPT
{
    flusher();
}

// private
// ----------------------------------------------------------------------------

template <typename OStream>
void sha256x2_writer<OStream>::flusher() NOEXCEPT
{
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest hash;
    BC_POP_WARNING()

    intrinsics::sha256_finalize(context_, hash.data());
    byte_writer<OStream>::do_write_bytes(sha256_hash(hash).data(), hash_size);
    byte_writer<OStream>::do_flush();
}

} // namespace system
} // namespace libbitcoin

#endif
