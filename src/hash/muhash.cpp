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
#include <bitcoin/system/hash/muhash.hpp>

#include <limits>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

using element = uintx_t<muhash3072::bit_size>;
using product = uintx_t<muhash3072::bit_size * 2u>;

// The largest 3072 bit prime, the multiplicative group (2^3072 - offset).
constexpr uint32_t offset = 1103717;
static const element& modulus() NOEXCEPT
{
    static const element prime = std::numeric_limits<element>::max() -
        sub1(offset);
    return prime;
}

muhash3072::muhash3072() NOEXCEPT
  : numerator_(1), denominator_(1)
{
}

muhash3072::muhash3072(const data_slice& element) NOEXCEPT
  : muhash3072()
{
    insert(element);
}

// Elements are the chacha20 expansion of the element hash, one conditional
// subtraction is the full reduction as the expansion is below 2 * modulus.
muhash3072::element muhash3072::to_element(const hash_digest& hash) NOEXCEPT
{
    data_array<byte_size> bytes{};
    chacha20 cipher{ hash };
    cipher.stream(bytes);

    element value{};
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end(),
        byte_bits, false);

    if (value >= modulus())
        value -= modulus();

    return value;
}

// Fixed width (no allocation), the product is twice the element width and
// reduced by the special form of the modulus (2^3072 - c), as x = hi * 2^3072
// + lo is congruent to hi * c + lo. Two folds leave less than 2^3072 + 2^43.
muhash3072::element muhash3072::multiply(const element& left,
    const element& right) NOEXCEPT
{
    static const product mask = (product{ 1 } << bit_size) - 1u;
    auto value = product{ left } * product{ right };
    value = (value & mask) + (value >> bit_size) * offset;
    value = (value & mask) + (value >> bit_size) * offset;

    auto result = element{ value };
    while (result >= modulus())
        result -= modulus();

    return result;
}

// The modular inverse by exponentiation as the modulus is prime (fermat).
muhash3072::element muhash3072::inverse(const element& value) NOEXCEPT
{
    const auto exponent = modulus() - 2u;
    element result{ 1 };
    element base{ value };

    for (size_t bit{}; bit < bit_size; ++bit)
    {
        if (boost::multiprecision::bit_test(exponent, bit))
            result = multiply(result, base);

        base = multiply(base, base);
    }

    return result;
}

void muhash3072::insert(const data_slice& element) NOEXCEPT
{
    insert_hash(accumulator<sha256>::hash(element.size(), element.data()));
}

void muhash3072::insert_hash(const hash_digest& hash) NOEXCEPT
{
    numerator_ = multiply(numerator_, to_element(hash));
}

void muhash3072::remove(const data_slice& element) NOEXCEPT
{
    remove_hash(accumulator<sha256>::hash(element.size(), element.data()));
}

void muhash3072::remove_hash(const hash_digest& hash) NOEXCEPT
{
    denominator_ = multiply(denominator_, to_element(hash));
}

muhash3072& muhash3072::operator*=(const muhash3072& other) NOEXCEPT
{
    numerator_ = multiply(numerator_, other.numerator_);
    denominator_ = multiply(denominator_, other.denominator_);
    return *this;
}

hash_digest muhash3072::flush() NOEXCEPT
{
    numerator_ = multiply(numerator_, inverse(denominator_));
    denominator_ = 1;
    return sha256_hash(from_uintx(numerator_));
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
