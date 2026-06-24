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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_THRESHOLD_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_THRESHOLD_HPP

#include <atomic>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Script helper for threshold signature accumulation.
struct threshold
{
    enum category_t : uint8_t
    {
        unknown,
        single,
        equal,
        inequal,
        lesser,
        greater,
        not_lesser,
        not_greater,
        between
    };

    struct tuple_t
    {
        /// Digest is created in the sigop (sigop scope - must copy).
        hash_digest digest;

        /// Point is a stack element (script scope - use reference).
        cref<ec_xonly> point;

        /// Signature is a stack element (script scope - use reference).
        cref<ec_signature> sig;
    };
    using tuples_t = std::vector<tuple_t>;

    /// Convert opcode to category.
    static constexpr category_t to_category(opcode code) NOEXCEPT
    {
        switch (code)
        {
            // checksig is not a threshold script code but is recorded with
            // single signatures archived in the multisig table.
            case opcode::checksig:
                return category_t::single;
            case opcode::numequal:
            case opcode::numequalverify:
                return category_t::equal;
            case opcode::numnotequal:
                return category_t::inequal;
            case opcode::lessthan:
                return category_t::lesser;
            case opcode::greaterthan:
                return category_t::greater;
            case opcode::lessthanorequal:
                return category_t::not_lesser;
            case opcode::greaterthanorequal:
                return category_t::not_greater;
            case opcode::within:
                return category_t::between;
            default:
                return category_t::unknown;
        }
    }

    /// Scoping requires that capture_.threshold(threshold) does not
    /// retain a reference to point or sig (must copy or dispose the refs).
    inline bool push_tuple(const hash_digest& digest,
        const cref<ec_xonly>& point, const cref<ec_signature>& sig) NOEXCEPT
    {
        tuples.emplace_back(digest, point, sig);
        return tuples.size() == expected;
    }

    tuples_t tuples{};
    category_t category{};
    uint16_t minimum{};
    uint16_t maximum{};
    uint16_t expected{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
