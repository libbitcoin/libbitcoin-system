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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TAPROOT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TAPROOT_HPP

#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API taproot
{
public:
    struct tap
    {
        INLINE operator bool() const NOEXCEPT
        {
            return version == tapscript_version;
        }

        uint8_t version;
        bool parity;
    };

    static tap parse(const data_chunk& control) NOEXCEPT;
    static bool is_valid_control_block(const data_chunk& control) NOEXCEPT;
    static hash_digest leaf_hash(uint8_t version,
        const script& script) NOEXCEPT;

    static bool verify_commitment(const data_chunk& control,
        const data_chunk& program, const hash_digest& hash,
        bool parity) NOEXCEPT;

protected:
    static hash_digest merkle_root(const data_chunk& control,
        const hash_digest& tapleaf_hash) NOEXCEPT;
    static hash_digest branch_hash(const hash_digest& left,
        const hash_digest& right) NOEXCEPT;
    static hash_digest tweak_hash(const ec_xonly& key,
        const hash_digest& merkle) NOEXCEPT;
};
} // namespace chain
} // namespace system
} // namespace libbitcoin


#endif
