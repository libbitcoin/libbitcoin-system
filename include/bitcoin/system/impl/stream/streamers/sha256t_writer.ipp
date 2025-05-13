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
#include <bitcoin/system/stream/streamers/sha256_writer.hpp>

namespace libbitcoin {
namespace system {

// static/private
TEMPLATE
constexpr sha256::state_t CLASS::
midstate() NOEXCEPT
{
    // TODO: need <= block_t constexpr hash.

    ////constexpr auto hashtag1 = sha256::hash(Tag.data);
    ////constexpr auto hashtag2 = sha256::hash(hashtag1, hashtag1);

    // TODO: hashtag2 needs to be unflushed sha256::state_t.
    // TODO: hashtag2 is flushed to sha256::digest_t (big endian).
    return {};
}

TEMPLATE
CLASS::
sha256t_writer(OStream& sink) NOEXCEPT
  : sha256_writer<OStream>(sink, midstate(), one)
{
}

} // namespace system
} // namespace libbitcoin

#endif
