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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_TABLE_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_TABLE_IPP

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Window tables.
// ----------------------------------------------------------------------------
// protected

// Odd multiples by repeated addition of 2a, with one inversion for all.
template <typename Word>
constexpr void algorithm::multiples(points_t<Word>& r,
    const affine_t<Word>& a) NOEXCEPT
{
    std_array<jacobian_t<Word>, table_size<point_bits>> points{};
    jacobian_t<Word> twice{};
    to_jacobian(points[0], a);
    double_(twice, points[0]);
    for (auto point = one; point < points.size(); ++point)
        add(points[point], points[sub1(point)], twice);

    to_affine(r, points);
}

// Table internals.
// ----------------------------------------------------------------------------
// protected

// Multiples (2i + 1)G of the chunk, and running products of their z.
template <size_t Bits, size_t Chunk>
constexpr algorithm::forward_t algorithm::forward() NOEXCEPT
{
    static_assert(bc::is_zero(table_size<Bits> % chunk_size));

    jacobian_t<uint64_t> first{}, twice{};
    to_jacobian(first, generator);
    double_(twice, first);

    forward_t out{};
    if constexpr (bc::is_zero(Chunk))
    {
        out.points[0] = first;
        out.products[0] = first.z;
    }
    else
    {
        const auto& prior = forwards<Bits, sub1(Chunk)>;
        add(out.points[0], prior.points.back(), twice);
        multiply(out.products[0], prior.products.back(), out.points[0].z);
    }

    for (auto point = one; point < chunk_size; ++point)
    {
        add(out.points[point], out.points[sub1(point)], twice);
        multiply(out.products[point], out.products[sub1(point)],
            out.points[point].z);
    }

    return out;
}

// The chunk after the last holds only the inverse of the full product, and
// each chunk unwinds its prefix products into affine points.
template <size_t Bits, size_t Chunk>
constexpr algorithm::backward_t algorithm::backward() NOEXCEPT
{
    constexpr auto last = sub1(chunk_count<Bits>);

    backward_t out{};
    if constexpr (Chunk > last)
    {
        inverse(out.inverse, forwards<Bits, last>.products.back());
    }
    else
    {
        const auto& chunk = forwards<Bits, Chunk>;
        field_t<uint64_t> prior{ 1 };
        if constexpr (is_nonzero(Chunk))
            prior = forwards<Bits, sub1(Chunk)>.products.back();

        out.inverse = backwards<Bits, add1(Chunk)>.inverse;

        field_t<uint64_t> inverse_z{};
        for (auto point = chunk_size; is_nonzero(point--);)
        {
            const auto& before = bc::is_zero(point) ? prior :
                chunk.products[sub1(point)];

            multiply(inverse_z, out.inverse, before);
            multiply(out.inverse, out.inverse, chunk.points[point].z);
            to_affine(out.points[point], chunk.points[point], inverse_z);
        }
    }

    return out;
}

template <size_t Bits, size_t Chunk>
constexpr void algorithm::tabulate(table_t<Bits>& r, bool mapped) NOEXCEPT
{
    const auto& chunk = backwards<Bits, Chunk>;
    for (size_t point{}; point < chunk_size; ++point)
    {
        auto entry = chunk.points[point];
        if (mapped)
        {
            endomorphism(entry, entry);
            normalize(entry.x);
        }

        const auto index = Chunk * chunk_size + point;
        for (size_t limb{}; limb < entry.x.size(); ++limb)
        {
            r.x[limb][index] = entry.x[limb];
            r.y[limb][index] = entry.y[limb];
        }
    }

    if constexpr (Chunk < sub1(chunk_count<Bits>))
        tabulate<Bits, add1(Chunk)>(r, mapped);
}

template <size_t Bits>
constexpr algorithm::table_t<Bits> algorithm::tabulate(bool mapped) NOEXCEPT
{
    table_t<Bits> out{};
    tabulate<Bits>(out, mapped);
    return out;
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
