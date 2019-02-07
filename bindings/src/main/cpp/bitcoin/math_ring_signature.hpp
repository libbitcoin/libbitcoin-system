/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_RING_SIGNATURE_HPP
#define LIBBITCOIN_RING_SIGNATURE_HPP

#include <vector>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>

#include <bitcoin/math_ec_secret.hpp>
#include <bitcoin/math_hash_digest.hpp>
#include <bitcoin/math_key_rings.hpp>
#include <bitcoin/math_proof_list.hpp>
#include <bitcoin/math_secret_list.hpp>
#include <bitcoin/utility_data_slice.hpp>
#include <bitcoin/math_hash_digest.hpp>

namespace libbitcoin {
namespace api {
/**
 * key_rings is a collection of rings of public keys.
 * Each ring contains several public keys.
 * Creating a valid signature requires at least one private key from each ring.
 * That is given three rings [{A, B, C}, {D, E, F}, {X, Y}], then to create a
 * valid signature, we must use a private key from each of those sets.
 * For example A and E and X. We can summarize this operation as:
 * (A or B or C) and (D or E or F) and (X or Y)
 */
//typedef std::vector<point_list> key_rings;

/**
 * A borromean ring signature.
 * theta = {e_0, s_{i_j} : 0 <= i <= n, 0 <= j <= m_i}
 */
struct math_ring_signature
{
//    typedef std::vector<secret_list> proof_list;

    math_ec_secret challenge;
    math_proof_list proofs;
};

/**
 * Prepare hash digest for use in ring signature signing and verification.
 * Returns sha256(message || flatten(rings))
 * @return     digest   Ring signature digest
 */
BC_API math_hash_digest digest(
		utility_data_slice message,
		const math_key_rings& rings);

/**
 * Create a borromean ring signature.
 * There must exist a valid signing key for each ring of public keys.
 * For example given a ring of [{A, B, C}, {D, E, F}, {X, Y}] then we
 * must have a set of keys that satisfies this constraint:
 * (A or B or C) and (D or E or F) and (X or Y)
 * @param[in]  out      The new signature.
 * @param[in]  secrets  Signing keys. Should be at least one from each ring.
 * @param[in]  rings    The rings each with n_i public keys.
 * @param[in]  digest   The message digest to sign.
 * @param[in]  salts    Randomizing seed values.
 * @return false if the signing operation fails.
 */
BC_API bool sign(
		math_ring_signature& out,
		const math_secret_list& secrets,
		const math_key_rings& rings,
		const math_hash_digest& digest,
		const math_secret_list& salts);

/**
 * Verify a borromean ring signature.
 * @param[in]  rings        The rings each with N_i public keys.
 * @param[in]  digest       The message digest to verify.
 * @param[in]  signature    Signature.
 * @return false if the verify operation fails.
 */
BC_API bool verify(
		const math_key_rings& rings,
		const math_hash_digest& digest,
		const math_ring_signature& signature);

} // namespace api
} // namespace libbitcoin

#endif

