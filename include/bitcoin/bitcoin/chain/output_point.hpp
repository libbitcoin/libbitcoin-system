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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_POINT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_POINT_HPP

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
    struct validation
    {
        /// This value is relative to queried fork point and candidacy.
        /// The output spender's tx->block is candidate/confirmed.
        bool spent = false;

        /// The output->tx is in a candidate chain block's tx.
        bool candidate = false;

        /// The output->tx is confirmed, relative to queried fork point.
        bool confirmed = false;

        /// The previous output is a coinbase (must verify spender maturity).
        bool coinbase = false;

        /// Block height (used for coinbase maturity and relative lock time).
        size_t height = 0;

        /// Median time past of the block (used for relative lock time).
        uint32_t median_time_past = 0;

        /// The output referenced by an input's output point.
        /// If cache.value is output::not_found then the output is missing.
        output cache;
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

    static output_point factory(const data_chunk& data, bool wire=true);
    static output_point factory(std::istream& stream, bool wire=true);
    static output_point factory(reader& source, bool wire=true);

    // Validation.
    //-------------------------------------------------------------------------

    /// True if cached previous output is mature enough to spend from height.
    bool is_mature(size_t height) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation metadata;

protected:
    // So that input may call reset from its own.
    friend class input;
};

} // namespace chain
} // namespace libbitcoin

#endif
