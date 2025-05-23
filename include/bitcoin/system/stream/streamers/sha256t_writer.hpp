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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256T_WRITER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_SHA256T_WRITER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {
    
/// A tagged hash writer that accepts an ostream.
template <text_t Tag, typename OStream = std::ostream>
class sha256t_writer
  : public byte_writer<OStream>
{
public:
    DEFAULT_COPY_MOVE(sha256t_writer);

    /// Constructors.
    sha256t_writer(OStream& sink) NOEXCEPT;

    /// Flush on destruct.
    ~sha256t_writer() NOEXCEPT override;

protected:
    /// The maximum addressable stream position.
    static constexpr size_t maximum = hash_size;

    void do_write_bytes(const uint8_t* data, size_t size) NOEXCEPT override;
    void do_flush() NOEXCEPT override;

private:
    static constexpr sha256::state_t midstate() NOEXCEPT;

    void flusher() NOEXCEPT;

    accumulator<sha256> context_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/sha256t_writer.ipp>

#endif
