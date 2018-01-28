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
#ifndef LIBBITCOIN_CHAIN_PAYMENT_RECORD_HPP
#define LIBBITCOIN_CHAIN_PAYMENT_RECORD_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/chain/input_point.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

/// This class models blockchain storage of a payment record.
class BC_API payment_record
{
public:
    typedef std::vector<payment_record> list;

    // Constructors.
    //-------------------------------------------------------------------------

    payment_record();

    payment_record(chain::payment_record&& other);
    payment_record(const chain::payment_record& other);

    payment_record(size_t height, chain::input_point&& point,
        uint64_t checksum);
    payment_record(size_t height, const chain::input_point& point,
        uint64_t checksum);
    payment_record(size_t height, chain::output_point&& point,
        uint64_t value);
    payment_record(size_t height, const chain::output_point& point,
        uint64_t value);

    // Operators.
    //-------------------------------------------------------------------------

    payment_record& operator=(payment_record&& other);
    payment_record& operator=(const payment_record& other);

    bool operator==(const payment_record& other) const;
    bool operator!=(const payment_record& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static payment_record factory(const data_chunk& data, bool wire=true);
    static payment_record factory(std::istream& stream, bool wire=true);
    static payment_record factory(reader& source, bool wire=true);

    bool from_data(const data_chunk& data, bool wire=true);
    bool from_data(std::istream& stream, bool wire=true);
    bool from_data(reader& source, bool wire=true);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool wire=true) const;
    void to_data(std::ostream& stream, bool wire=true) const;
    void to_data(writer& sink, bool wire=true) const;

    // Properties (size, accessors).
    //-------------------------------------------------------------------------

    static size_t satoshi_fixed_size(bool wire=true);
    size_t serialized_size(bool wire=true) const;

    /// The blockchain height of the point.
    size_t height() const;

    /// The point is an output.
    bool is_output() const;

    /// The point is an input.
    bool is_input() const;

    /// The point that identifies the input or output.
    const chain::point& point() const;

    /// The value of the input point or checksum of the output point.
    uint64_t data() const;

protected:
    void reset();

private:
    enum class point_kind : uint8_t
    {
        output = 0,
        input = 1,
        invalid = 2
    };

    static point_kind to_kind(uint8_t value);

    uint32_t height_;
    point_kind kind_;
    chain::point point_;
    uint64_t data_;
};

} // namespace chain
} // namespace libbitcoin

#endif
