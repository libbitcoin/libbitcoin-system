/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SPONGE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SPONGE_IPP

// Based on:
// FIPS PUB 202, Algorithm 8 SPONGE[f, pad, r](N, d).
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf

namespace libbitcoin {
namespace system {

TEMPLATE
inline CLASS::
sponge() NOEXCEPT
  : state_{ Algorithm::H::get }
{
}

TEMPLATE
inline void CLASS::
write(const_byte_span data) NOEXCEPT
{
    BC_ASSERT(!sealed_);

    // Complete a buffered partial block.
    if (is_nonzero(offset_))
    {
        const auto count = std::min(block_bytes - offset_, data.size());
        std::copy_n(data.begin(), count, std::next(buffer_.begin(), offset_));
        data = data.subspan(count);
        offset_ += count;

        if (offset_ < block_bytes)
            return;

        Algorithm::accumulate(state_, buffer_);
        offset_ = zero;
    }

    // Whole blocks, remainder buffered.
    const auto remain = data.size() % block_bytes;
    const auto bytes = data.size() - remain;
    Algorithm::accumulate(state_, iblocks_t{ bytes, data.data() });
    std::copy_n(std::next(data.begin(), bytes), remain, buffer_.begin());
    offset_ = remain;
}

TEMPLATE
inline typename CLASS::digest_t
CLASS::
flush() NOEXCEPT
{
    digest_t digest{};
    squeeze(digest);
    return digest;
}

TEMPLATE
inline void CLASS::
squeeze(byte_span out) NOEXCEPT
{
    // Seal the buffered tail, leaving no buffered output.
    if (!sealed_)
    {
        Algorithm::seal(state_, offset_, buffer_.data());
        offset_ = block_bytes;
        sealed_ = true;
    }

    while (!out.empty())
    {
        if (offset_ == block_bytes)
        {
            buffer_ = Algorithm::squeeze(state_);
            offset_ = zero;
        }

        const auto count = std::min(block_bytes - offset_, out.size());
        std::copy_n(std::next(buffer_.cbegin(), offset_), count, out.begin());
        out = out.subspan(count);
        offset_ += count;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
