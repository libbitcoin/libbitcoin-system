/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_SEQUENCE_HPP
#define LIBBITCOIN_CHAIN_SEQUENCE_HPP

#include <cstdint>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace chain {

typedef std::vector<operation> sequence;

/// No-code patterns.
BC_API bool is_push_only(const sequence& ops);

/// Unspendable pattern (standard).
BC_API bool is_null_data_pattern(const sequence& ops);

/// Payment script patterns (standard).
BC_API bool is_pay_multisig_pattern(const sequence& ops);
BC_API bool is_pay_public_key_pattern(const sequence& ops);
BC_API bool is_pay_key_hash_pattern(const sequence& ops);
BC_API bool is_pay_script_hash_pattern(const sequence& ops);

/// Signature script patterns (standard).
BC_API bool is_sign_multisig_pattern(const sequence& ops);
BC_API bool is_sign_public_key_pattern(const sequence& ops);
BC_API bool is_sign_key_hash_pattern(const sequence& ops);
BC_API bool is_sign_script_hash_pattern(const sequence& ops);

/// Stack factories (standard).
BC_API sequence to_null_data_pattern(data_slice data);
BC_API sequence to_pay_public_key_pattern(data_slice point);
BC_API sequence to_pay_key_hash_pattern(const short_hash& hash);
BC_API sequence to_pay_script_hash_pattern(const short_hash& hash);
BC_API sequence to_pay_multisig_pattern(uint8_t signatures,
    const point_list& points);
BC_API sequence to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points);

} // end chain
} // end libbitcoin

#endif
