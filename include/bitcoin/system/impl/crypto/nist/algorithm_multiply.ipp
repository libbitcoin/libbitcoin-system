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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_MULTIPLY_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_MULTIPLY_IPP

// Multiplication
// ============================================================================
// Fixed four bit windows, most significant first. The secret multiplication
// reads every table entry for each window and adds each window's entry, so
// its sequence of operations is independent of the scalar.

namespace libbitcoin {
namespace system {
namespace nist {

TEMPLATE
constexpr typename CLASS::table_t CLASS::
make_table(const projective_t& p) NOEXCEPT
{
    table_t table{};
    table[0] = infinity();
    table[1] = p;
    for (auto index = two; index < entries; ++index)
        table[index] = add(table[sub1(index)], p);

    return table;
}

TEMPLATE
constexpr typename CLASS::table_t CLASS::
generator_table() NOEXCEPT
{
    return make_table(generator());
}

TEMPLATE
constexpr size_t CLASS::
digit(const limbs_t& scalar, size_t index) NOEXCEPT
{
    constexpr auto mask = possible_wide_cast<uint64_t>(sub1(entries));
    const auto bit = index * window;
    const auto limb = scalar[bit / bits<uint64_t>];
    const auto value = shift_right(limb, bit % bits<uint64_t>);
    return possible_narrow_cast<size_t>(bit_and(value, mask));
}

TEMPLATE
constexpr typename CLASS::projective_t CLASS::
lookup(const table_t& table, size_t digit) NOEXCEPT
{
    projective_t out{};
    for (size_t index{}; index < entries; ++index)
    {
        const auto mask = twos_complement(to_int<uint64_t>(index == digit));
        select(out.x, mask, table[index].x, out.x);
        select(out.y, mask, table[index].y, out.y);
        select(out.z, mask, table[index].z, out.z);
    }

    return out;
}

// scalar * P, table of P (constant time).
TEMPLATE
constexpr typename CLASS::projective_t CLASS::
multiply(const table_t& table, const limbs_t& scalar) NOEXCEPT
{
    auto out = infinity();
    for (auto index = windows; !bc::is_zero(index); --index)
    {
        out = twice(twice(twice(twice(out))));
        out = add(out, lookup(table, digit(scalar, sub1(index))));
    }

    return out;
}

// g * G + q * Q, tables of G and Q (variable time).
TEMPLATE
constexpr typename CLASS::projective_t CLASS::
multiply(const limbs_t& g, const table_t& generators, const limbs_t& q,
    const table_t& table) NOEXCEPT
{
    auto out = infinity();
    for (auto index = windows; !bc::is_zero(index); --index)
    {
        out = twice(twice(twice(twice(out))));
        out = add(out, generators[digit(g, sub1(index))]);
        out = add(out, table[digit(q, sub1(index))]);
    }

    return out;
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
