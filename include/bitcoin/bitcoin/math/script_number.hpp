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
#ifndef LIBBITCOIN_SCRIPT_NUMBER_HPP
#define LIBBITCOIN_SCRIPT_NUMBER_HPP

#include <cstddef>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

BC_CONSTEXPR size_t max_script_number_size = 4;
BC_CONSTEXPR size_t cltv_max_script_number_size = 5;

/**
 * Numeric opcodes (OP_1ADD, etc) are restricted to operating on
 * 4-byte integers. The semantics are subtle, though: operands must be
 * in the range [-2^31 +1...2^31 -1], but results may overflow (and are
 * valid as long as they are not used in a subsequent numeric operation).
 *
 * script_number enforces those semantics by storing results as
 * an int64 and allowing out-of-range values to be returned as a vector of
 * bytes but throwing an exception if arithmetic is done or the result is
 * interpreted as an integer.
 */
class BC_API script_number
{
public:
    /// Construct with zero value, may call set_data() after.
    script_number();

    /// Construct with specified value.
    explicit script_number(int64_t value);

    /// Set the value from a byte vector with LSB first ordering.
    bool set_data(const data_chunk& data,
        uint8_t max_size=max_script_number_size);

    /// Return the value as a byte vector with LSB first ordering.
    data_chunk data() const;

    /// Return the value bounded by the limits of int32.
    int32_t int32() const;

    /// Return the value.
    int64_t int64() const;

    /// Arithmetic with a number (throws on overflow).
    script_number operator+(const int64_t value) const;
    script_number operator-(const int64_t value) const;
    script_number& operator+=(const int64_t value);
    script_number& operator-=(const int64_t value);

    /// Arithmetic with another script_number (throws on overflow).
    script_number operator+(const script_number& other) const;
    script_number operator-(const script_number& other) const;
    script_number& operator+=(const script_number& other);
    script_number& operator-=(const script_number& other);

    /// This script_number.
    script_number operator+() const;

    /// Math-negated copy of this script_number (throws on minimum value).
    script_number operator-() const;

    /// Comparison operators with a number.
    bool operator==(const int64_t value) const;
    bool operator!=(const int64_t value) const;
    bool operator<=(const int64_t value) const;
    bool operator<(const int64_t value) const;
    bool operator>=(const int64_t value) const;
    bool operator>(const int64_t value) const;

    /// Comparison operators with another script_number.
    bool operator==(const script_number& other) const;
    bool operator!=(const script_number& other) const;
    bool operator<=(const script_number& other) const;
    bool operator<(const script_number& other) const;
    bool operator>=(const script_number& other) const;
    bool operator>(const script_number& other) const;

private:
    int64_t value_;
};

} // namespace libbitcoin

#endif

