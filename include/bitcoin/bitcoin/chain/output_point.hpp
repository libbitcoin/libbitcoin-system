/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CHAIN_OUTPUT_POINT_HPP
#define LIBBITCOIN_CHAIN_OUTPUT_POINT_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

class BC_API output_point
  : public point
{
public:

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation_type
    {
        /// An output is spent if a valid transaction has a valid claim on it.
        /// When validating blocks only long chain blocks can have a claim.
        /// When validating memory pool tx another mempool tx can have a claim.
        bool spent = false;

        /// A spend is confirmed if spender is in long chain (not memory pool).
        bool confirmed = false;

        /// The previous output is a coinbase (must verify spender maturity).
        bool coinbase = false;

        /// Prevout height is used for coinbase maturity and relative lock time.
        size_t height = 0;

        /// Median time past is used for relative lock time.
        uint32_t median_time_past = 0;

        /// The output cache contains the output referenced by the input point.
        /// If the cache.value is not_found (default) the output is not found.
        output cache = output{};
    };

    // Constructors.
    //-------------------------------------------------------------------------

    output_point();

    output_point(point&& other);
    output_point(const point& value);

    output_point(output_point&& other);
    output_point(const output_point& other);

    output_point(hash_digest&& hash, uint32_t index);
    output_point(const hash_digest& hash, uint32_t index);

    // Operators.
    //-------------------------------------------------------------------------
    // This class is move assignable and copy assignable.

    output_point& operator=(point&& other);
    output_point& operator=(const point&);
    output_point& operator=(output_point&& other);
    output_point& operator=(const output_point&);

    bool operator==(const point& other) const;
    bool operator!=(const point& other) const;
    bool operator==(const output_point& other) const;
    bool operator!=(const output_point& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static output_point factory_from_data(const data_chunk& data, bool wire=true);
    static output_point factory_from_data(std::istream& stream, bool wire=true);
    static output_point factory_from_data(reader& source, bool wire=true);

    // Validation.
    //-------------------------------------------------------------------------

    /// True if cached previous output is mature enough to spend from height.
    bool is_mature(size_t height) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation_type validation;

protected:
    // So that input may call reset from its own.
    friend class input;
};

} // namespace chain
} // namespace libbitcoin

#endif
