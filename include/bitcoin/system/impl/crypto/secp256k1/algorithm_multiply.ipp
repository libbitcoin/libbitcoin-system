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
// addition was exceptional are reported, not computed. A single word adds
// sparse digits in place of windows.
template <typename Word>
constexpr Word algorithm::multiply(jacobian_t<Word>& r,
    const scalars_t<Word>& g, const affine_t<Word>& a,
    const scalars_t<Word>& k) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        return multiply_naf(r, g.front(), a, k.front());
    }
    else
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

        // Point multiples are affine on the curve isomorphic by scale, where
        // the sum is computed, so generator multiples are added by that scale.
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

            if (bit <= generator_top && is_zero(bit % generator_bits))
            {
                const auto position = bit / generator_bits;
                digit(index, negative, g_first, position, false);
                lookup(addend, index, false, negative);
                add_point(sum, addend, scale, faults);
                digit(index, negative, g_second, position, false);
                lookup(addend, index, true, negative);
                add_point(sum, addend, scale, faults);
            }

            if (bit <= point_top && is_zero(bit % point_bits))
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

// Each half adds a table point at each of its nonzero digits, with doubling
// from the highest digit of any half.
constexpr uint64_t algorithm::multiply_naf(jacobian_t<uint64_t>& r,
    const scalar_t& g, const affine_t<uint64_t>& a,
    const scalar_t& k) NOEXCEPT
{
    std_array<scalar_t, 4> halves{};
    split(halves[0], halves[1], g);
    split(halves[2], halves[3], k);

    std_array<naf_t, 4> digits{};
    std_array<bool, 4> negatives{};
    size_t top{};
    for (size_t half{}; half < halves.size(); ++half)
    {
        auto& magnitude = halves[half];
        negatives[half] = is_high(magnitude);
        if (negatives[half])
            negate(magnitude, magnitude);

        top = greater(top, half < two ?
            naf<generator_bits>(digits[half], magnitude) :
            naf<naf_bits>(digits[half], magnitude));
    }

    points_t<uint64_t, naf_bits> a_first{}, a_second{};
    field_t<uint64_t> scale{};
    multiples(a_first, scale, a);
    for (size_t point{}; point < a_first.size(); ++point)
        endomorphism(a_second[point], a_first[point]);

    uint64_t faults{};
    affine_t<uint64_t> addend{};
    jacobian_t<uint64_t> sum{};
    sum.infinity = max_uint64;

    for (auto bit = top; is_nonzero(bit--);)
    {
        if (is_zero(sum.infinity))
            double_(sum, sum);

        for (size_t half{}; half < halves.size(); ++half)
        {
            const auto value = digits[half][bit];
            if (is_zero(value))
                continue;

            const size_t magnitude = absolute(value);
            const auto entry = to_half(sub1(magnitude));
            const auto negative = is_negative(value) != negatives[half] ?
                max_uint64 : 0_u64;

            if (half < two)
            {
                lookup(addend, uint64_t{ entry }, is_nonzero(half),
                    negative);
                add_point(sum, addend, scale, faults);
            }
            else
            {
                negate(addend, half == two ? a_first[entry] :
                    a_second[entry], negative);
                add_point(sum, addend, faults);
            }
        }
    }

    multiply(sum.z, sum.z, scale);
    r = sum;
    return faults;
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

// Each digit is the next Bits + 1 bits with a carry from the last, made
// negative with a carry where its top bit is set.
template <size_t Bits>
constexpr size_t algorithm::naf(naf_t& r, const scalar_t& magnitude) NOEXCEPT
{
    constexpr auto width = add1(Bits);
    constexpr auto size = array_count<naf_t>;
    constexpr auto limb_size = bits<uint64_t>;

    r = {};
    size_t top{};
    auto carry = false;
    for (size_t bit{}; bit < size;)
    {
        const auto limb = bit / limb_size;
        const auto shift = bit % limb_size;
        if (get_right(magnitude[limb], shift) == carry)
        {
            ++bit;
            continue;
        }

        const auto count = lesser(width, size - bit);
        auto value = magnitude[limb] >> shift;
        if (shift + count > limb_size && add1(limb) < array_count<scalar_t>)
            value |= magnitude[add1(limb)] << (limb_size - shift);

        value &= unmask_right<uint64_t>(count);
        value += to_int<uint64_t>(carry);
        carry = get_right(value, Bits);

        const auto digit = to_signed(value);
        r[bit] = narrow_cast<int16_t>(carry ? digit - power2<int64_t>(width) :
            digit);

        top = add1(bit);
        bit += count;
    }

    return top;
}

template <typename Word>
constexpr Word algorithm::pack(const words_t<Word>& values) NOEXCEPT
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

// Tables hold odd multiples 1, 3, ..., so digit d is entry (|d| - 1) / 2, and
// a point table interleaves lanes within each of the ten limbs of an entry.
template <size_t Bits, typename Word>
constexpr void algorithm::digit(Word& index, Word& negative,
    const recodes_t<Bits, Word>& halves, size_t position,
    bool interleaved) NOEXCEPT
{
    constexpr auto stride = two * array_count<field_t<Word>> * lanes<Word>;

    words_t<Word> indexes{}, negatives{};
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

// Entry e is in slice e / s at offset locate(e mod s), from which limb i of x
// is at i * b and limb i of y at (5 + i) * b, and the endomorphism table
// follows the generator table in each slice.
template <typename Word>
constexpr void algorithm::lookup(affine_t<Word>& r, Word entry, bool mapped,
    Word negative) NOEXCEPT
{
    constexpr auto size = array_count<field_t<Word>>;
    const auto entries = unpack(entry);
    const auto table = mapped ? table_words : zero;

    std_array<words_t<Word>, size> xs{}, ys{};
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    for (size_t lane{}; lane < lanes<Word>; ++lane)
    {
        const auto value = possible_narrow_cast<size_t>(entries[lane]);
        const auto base = generator_slices[value / slice_size] + table +
            locate(value % slice_size);

        for (size_t limb{}; limb < size; ++limb)
        {
            xs[limb][lane] = base[limb * block_size];
            ys[limb][lane] = base[(size + limb) * block_size];
        }
    }
    BC_POP_WARNING()

    affine_t<Word> point{};
    for (size_t limb{}; limb < size; ++limb)
    {
        point.x[limb] = pack<Word>(xs[limb]);
        point.y[limb] = pack<Word>(ys[limb]);
    }

    negate(r, point, negative);
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
    words_t<Word> evens{}, positives{};
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
