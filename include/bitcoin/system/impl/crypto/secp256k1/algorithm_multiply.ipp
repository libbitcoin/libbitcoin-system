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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_MULTIPLY_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_MULTIPLY_IPP

// Based on:
// Strauss-Shamir interleaving of signed odd windows over GLV split halves.

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Multiplication.
// ----------------------------------------------------------------------------
// protected

// Each scalar splits into two halves, each window of each half adds a table
// point, and halves made odd are corrected at the end. Lanes in which an
// addition was exceptional are reported, not computed.
template <typename Word>
constexpr Word algorithm::multiply(jacobian_t<Word>& r,
    const scalars_t<Word>& g, const affine_t<Word>& a,
    const scalars_t<Word>& k) NOEXCEPT
{
    constexpr auto generator_top = sub1(digit_count<generator_bits>) *
        generator_bits;
    constexpr auto point_top = sub1(digit_count<point_bits>) * point_bits;
    constexpr auto top = greater(generator_top, point_top);

    recodes_t<generator_bits, Word> g_first{}, g_second{};
    recodes_t<point_bits, Word> k_first{}, k_second{};
    for (size_t lane{}; lane < lanes<Word>; ++lane)
    {
        scalar_t first{}, second{};
        split(first, second, g[lane]);
        recode(g_first[lane], first);
        recode(g_second[lane], second);
        split(first, second, k[lane]);
        recode(k_first[lane], first);
        recode(k_second[lane], second);
    }

    // Point multiples are affine on the curve isomorphic by scale, where the
    // sum is computed, so generator multiples are added by that scale.
    points_t<Word> a_first{}, a_second{};
    field_t<Word> scale{};
    multiples(a_first, scale, a);
    for (size_t point{}; point < a_first.size(); ++point)
        endomorphism(a_second[point], a_first[point]);

    auto faults = f::broadcast<Word>(uint64_t{});
    Word index{}, negative{};
    affine_t<Word> addend{};
    jacobian_t<Word> sum{};
    sum.infinity = f::broadcast<Word>(max_uint64);

    for (auto bit = add1(top); is_nonzero(bit--);)
    {
        if (bit != top)
            double_(sum, sum);

        if (bit <= generator_top && bc::is_zero(bit % generator_bits))
        {
            const auto position = bit / generator_bits;
            digit(index, negative, g_first, position, false);
            lookup(addend, generator_table, index, negative);
            add_point(sum, addend, scale, faults);
            digit(index, negative, g_second, position, false);
            lookup(addend, endomorphism_table, index, negative);
            add_point(sum, addend, scale, faults);
        }

        if (bit <= point_top && bc::is_zero(bit % point_bits))
        {
            const auto position = bit / point_bits;
            digit(index, negative, k_first, position, true);
            lookup(addend, a_first, index, negative);
            add_point(sum, addend, faults);
            digit(index, negative, k_second, position, true);
            lookup(addend, a_second, index, negative);
            add_point(sum, addend, faults);
        }
    }

    affine_t<Word> base{ broadcast<Word>(generator.x),
        broadcast<Word>(generator.y) };

    const auto unit = broadcast<Word>({ 1 });
    correct(sum, base, scale, g_first, faults);
    endomorphism(base, base);
    correct(sum, base, scale, g_second, faults);
    correct(sum, a_first.front(), unit, k_first, faults);
    correct(sum, a_second.front(), unit, k_second, faults);

    multiply(sum.z, sum.z, scale);
    r = sum;
    return faults;
}

// Double and add of each bit, without tables or exceptions.
constexpr void algorithm::multiply_complete(jacobian_t<uint64_t>& r,
    const scalar_t& g, const affine_t<uint64_t>& a,
    const scalar_t& k) NOEXCEPT
{
    constexpr auto limb_size = bits<uint64_t>;
    jacobian_t<uint64_t> sum{};
    sum.infinity = max_uint64;

    for (auto bit = array_count<scalar_t> * limb_size; is_nonzero(bit--);)
    {
        double_(sum, sum);

        if (get_right(g[bit / limb_size], bit % limb_size))
            add_complete(sum, sum, generator);

        if (get_right(k[bit / limb_size], bit % limb_size))
            add_complete(sum, sum, a);
    }

    r = sum;
}

// Multiplication internals.
// ----------------------------------------------------------------------------
// protected

// The magnitude of a negative half is recoded, and an even one made odd.
template <size_t Bits>
constexpr void algorithm::recode(recoded_t<Bits>& r,
    const scalar_t& half) NOEXCEPT
{
    auto magnitude = half;
    r.negative = is_high(half);
    if (r.negative)
        negate(magnitude, half);

    r.even = !get_right(magnitude[0]);
    if (r.even)
        set_right_into(magnitude[0]);

    recode<Bits, digit_count<Bits>>(r.digits, magnitude);
}

template <typename Word>
constexpr Word algorithm::pack(
    const std_array<uint64_t, lanes<Word>>& values) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        return values[0];
    }
    else if constexpr (lanes<Word> == 2)
    {
        return f::set<Word>(values[0], values[1]);
    }
    else if constexpr (lanes<Word> == 4)
    {
        return f::set<Word>(values[0], values[1], values[2], values[3]);
    }
    else
    {
        return f::set<Word>(values[0], values[1], values[2], values[3],
            values[4], values[5], values[6], values[7]);
    }
}

// Tables hold odd multiples 1, 3, ..., so digit d is at (|d| - 1) / 2, and a
// point table interleaves lanes within each of the ten limbs of an entry.
template <size_t Bits, typename Word>
constexpr void algorithm::digit(Word& index, Word& negative,
    const recodes_t<Bits, Word>& halves, size_t position,
    bool interleaved) NOEXCEPT
{
    constexpr auto stride = two * array_count<field_t<Word>> * lanes<Word>;

    std_array<uint64_t, lanes<Word>> indexes{}, negatives{};
    for (size_t lane{}; lane < lanes<Word>; ++lane)
    {
        const auto& half = halves[lane];
        const auto value = half.digits[position];
        const size_t magnitude = absolute(value);
        const auto entry = to_half(sub1(magnitude));
        indexes[lane] = interleaved ? entry * stride + lane : entry;
        negatives[lane] = is_negative(value) != half.negative ? max_uint64 :
            0_u64;
    }

    index = pack<Word>(indexes);
    negative = pack<Word>(negatives);
}

template <size_t Bits, typename Word>
constexpr void algorithm::lookup(affine_t<Word>& r, const table_t<Bits>& table,
    Word index, Word negative) NOEXCEPT
{
    affine_t<Word> entry{};
    for (size_t limb{}; limb < entry.x.size(); ++limb)
    {
        entry.x[limb] = f::gather(table.x[limb].data(), index);
        entry.y[limb] = f::gather(table.y[limb].data(), index);
    }

    negate(r, entry, negative);
}

// Limb i of lane l of point table entry e is at (10e + i) * lanes + l, where
// offset is 10e * lanes + l.
template <typename Word>
constexpr void algorithm::lookup(affine_t<Word>& r, const points_t<Word>& table,
    Word offset, Word negative) NOEXCEPT
{
    constexpr auto size = array_count<field_t<Word>>;

    affine_t<Word> entry{};
    if constexpr (is_same_type<Word, uint64_t>)
    {
        entry = table[possible_narrow_cast<size_t>(offset) / (two * size)];
    }
    else
    {
        const auto base = pointer_cast<const uint64_t>(table.data());

        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        for (size_t limb{}; limb < size; ++limb)
        {
            entry.x[limb] = f::gather(base + limb * lanes<Word>, offset);
            entry.y[limb] = f::gather(base + (size + limb) * lanes<Word>,
                offset);
        }
        BC_POP_WARNING()
    }

    negate(r, entry, negative);
}

// Lanes at infinity take b, and other exceptional lanes are faults.
template <typename Word>
constexpr void algorithm::add_point(jacobian_t<Word>& r,
    const affine_t<Word>& b, Word& faults) NOEXCEPT
{
    jacobian_t<Word> sum{};
    const auto uncomputed = add(sum, r, b);
    if (f::any(r.infinity))
    {
        jacobian_t<Word> lifted{};
        to_jacobian(lifted, b);
        select(sum, r.infinity, lifted, sum);
        faults = f::or_(faults, f::andnot(r.infinity, uncomputed));
    }
    else
    {
        faults = f::or_(faults, uncomputed);
    }

    r = sum;
}

// Lanes at infinity take b mapped by scale, and other exceptional lanes are
// faults.
template <typename Word>
constexpr void algorithm::add_point(jacobian_t<Word>& r,
    const affine_t<Word>& b, const field_t<Word>& scale, Word& faults) NOEXCEPT
{
    jacobian_t<Word> sum{};
    const auto uncomputed = add(sum, r, b, scale);
    if (f::any(r.infinity))
    {
        field_t<Word> ss{}, sss{};
        square(ss, scale);
        multiply(sss, ss, scale);

        jacobian_t<Word> lifted{};
        to_jacobian(lifted, b);
        multiply(lifted.x, lifted.x, ss);
        multiply(lifted.y, lifted.y, sss);
        select(sum, r.infinity, lifted, sum);
        faults = f::or_(faults, f::andnot(r.infinity, uncomputed));
    }
    else
    {
        faults = f::or_(faults, uncomputed);
    }

    r = sum;
}

// A half made odd added its signed base once more, so subtract it where even.
template <size_t Bits, typename Word>
constexpr void algorithm::correct(jacobian_t<Word>& r, const affine_t<Word>& a,
    const field_t<Word>& scale, const recodes_t<Bits, Word>& halves,
    Word& faults) NOEXCEPT
{
    std_array<uint64_t, lanes<Word>> evens{}, positives{};
    for (size_t lane{}; lane < lanes<Word>; ++lane)
    {
        evens[lane] = halves[lane].even ? max_uint64 : 0_u64;
        positives[lane] = halves[lane].negative ? 0_u64 : max_uint64;
    }

    const auto even = pack<Word>(evens);
    if (!f::any(even))
        return;

    affine_t<Word> addend{};
    negate(addend, a, pack<Word>(positives));

    auto sum = r;
    auto fault = f::broadcast<Word>(uint64_t{});
    add_point(sum, addend, scale, fault);
    select(r, even, sum, r);
    faults = f::or_(faults, f::and_(fault, even));
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
