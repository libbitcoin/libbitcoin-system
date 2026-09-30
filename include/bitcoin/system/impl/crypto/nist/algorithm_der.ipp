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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_DER_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_DER_IPP

// DER
// ============================================================================
// ECDSA-Sig-Value ::= SEQUENCE { r INTEGER, s INTEGER }. For these sizes all
// lengths are below 128, so only the short length form is valid.

namespace libbitcoin {
namespace system {
namespace nist {

constexpr uint8_t sequence_tag = 0x30;
constexpr uint8_t integer_tag = 0x02;
constexpr size_t long_length = 0x80;

// public
// ----------------------------------------------------------------------------

TEMPLATE
data_chunk CLASS::
encode(const signature_t& signature) NOEXCEPT
{
    data_chunk body{};
    encode_integer(body, slice<zero, size>(signature));
    encode_integer(body, slice<size, two * size>(signature));

    data_chunk out{ sequence_tag, narrow_cast<uint8_t>(body.size()) };
    out.insert(out.end(), body.cbegin(), body.cend());
    return out;
}

TEMPLATE
bool CLASS::
decode(signature_t& out, const_byte_span der) NOEXCEPT
{
    if ((der.size() < two) || (der[0] != sequence_tag))
        return false;

    const auto length = wide_cast<size_t>(der[1]);
    if ((length >= long_length) || (length != der.size() - two))
        return false;

    bytes_t r{}, s{};
    auto rest = der.subspan(two);
    if (!decode_integer(r, rest) || !decode_integer(s, rest) ||
        !rest.empty())
        return false;

    out = splice(r, s);
    return true;
}

// protected
// ----------------------------------------------------------------------------

// Minimal big-endian, with a leading zero if the high bit is set.
TEMPLATE
void CLASS::
encode_integer(data_chunk& out, const_byte_span value) NOEXCEPT
{
    auto digits = value;
    while ((digits.size() > one) && bc::is_zero(digits.front()))
        digits = digits.subspan(one);

    const auto pad = get_left(digits.front());
    out.push_back(integer_tag);
    out.push_back(narrow_cast<uint8_t>(digits.size() + to_int<size_t>(pad)));
    if (pad)
        out.push_back(0x00);

    out.insert(out.end(), digits.begin(), digits.end());
}

// Minimal non-negative integer of at most size significant bytes.
TEMPLATE
bool CLASS::
decode_integer(bytes_t& out, const_byte_span& der) NOEXCEPT
{
    if ((der.size() < two) || (der[0] != integer_tag))
        return false;

    const auto length = wide_cast<size_t>(der[1]);
    if (bc::is_zero(length) || (length >= long_length) ||
        (length > der.size() - two))
        return false;

    auto value = der.subspan(two, length);
    if (get_left(value[0]))
        return false;

    if (bc::is_zero(value[0]) && (length > one))
    {
        if (!get_left(value[1]))
            return false;

        value = value.subspan(one);
    }

    if (value.size() > size)
        return false;

    const auto offset = size - value.size();
    out = {};
    std::copy(value.begin(), value.end(), std::next(out.begin(), offset));
    der = der.subspan(two + length);
    return true;
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
