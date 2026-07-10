/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_STRIPPER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_STRIPPER_HPP

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Helper class for operations stripping.
/// Bridges the gap between xptr (stack data) and cptr (op data). Otherwise
/// chain::operation could be used. This is also slightly more efficient,
/// being specialized for valid ops and the assumption of distinct data_chunks.
class BC_API stripper final
{
public:
    using span = std::span<const stripper>;
    using buffer = std::array<stripper, add1(max_multisig_public_keys)>;

    /// Fills a stack allocation with opcode::codeseparator.
    inline stripper() NOEXCEPT
      : code_(opcode::codeseparator), data_()
    {
    }

    // ************************************************************************
    // CONSENSUS: nominal endorsement operation encoding is required.
    // ************************************************************************
    inline explicit stripper(const chunk_xptr& push_data) NOEXCEPT
      : code_(operation::nominal_opcode_from_data(*push_data)),
        data_(push_data)
    {
    }

    inline explicit stripper(opcode code) NOEXCEPT
      : code_(code), data_()
    {
    }

    inline opcode code() const NOEXCEPT
    {
        return code_;
    }

    inline const data_chunk& data() const NOEXCEPT
    {
        return *data_;
    }

    inline const chunk_xptr& data_ptr() const NOEXCEPT
    {
        return data_;
    }

private:
    opcode code_;
    chunk_xptr data_;
};

inline bool operator==(const operation& op, const stripper& strip) NOEXCEPT
{
    // Endorsements should match by value but not pointer.
    return op.code() == strip.code() && op.data() == strip.data();
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
