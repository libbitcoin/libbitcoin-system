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

// Odd multiples by repeated addition of 2a, on the curve isomorphic by the z
// of 2a (where 2a is affine). Each multiple is then scaled to the z of the
// last by the product of later z ratios, so that all are affine on the curve
// isomorphic by scale (the z of 2a times the z of the last).
template <size_t Size, typename Word>
constexpr void algorithm::multiples(std_array<affine_t<Word>, Size>& r,
    field_t<Word>& scale, const affine_t<Word>& a) NOEXCEPT
{
    std_array<field_t<Word>, Size> ratios{};
    jacobian_t<Word> sum{}, twice{};
    field_t<Word> zz{}, zzz{};

    to_jacobian(sum, a);
    double_(twice, sum);
    const affine_t<Word> step{ twice.x, twice.y };
    square(zz, twice.z);
    multiply(zzz, zz, twice.z);
    multiply(sum.x, a.x, zz);
    multiply(sum.y, a.y, zzz);

    r.front() = { sum.x, sum.y };
    for (auto point = one; point < Size; ++point)
    {
        add(sum, ratios[point], sum, step, sum.z);
        r[point] = { sum.x, sum.y };
    }

    auto factor = ratios.back();
    for (auto point = sub1(Size); is_nonzero(point--);)
    {
        square(zz, factor);
        multiply(zzz, zz, factor);
        multiply(r[point].x, r[point].x, zz);
        multiply(r[point].y, r[point].y, zzz);
        if (is_nonzero(point))
            multiply(factor, factor, ratios[point]);
    }

    multiply(scale, sum.z, twice.z);
}

// Table location.
// ----------------------------------------------------------------------------
// protected

// Entry i is column i mod b of block i / b, where each block holds ten limb
// columns of b values.
constexpr size_t algorithm::locate(size_t entry) NOEXCEPT
{
    constexpr auto stride = two * array_count<field_t<uint64_t>> * block_size;
    return (entry / block_size) * stride + (entry % block_size);
}

// Table internals.
// ----------------------------------------------------------------------------
// protected

// Group g starts with multiple (2gs + 1)G for group size s, the start of the
// prior group plus 2sG.
template <size_t Group>
constexpr algorithm::jacobian_t<uint64_t> algorithm::start() NOEXCEPT
{
    jacobian_t<uint64_t> out{};
    if constexpr (is_zero(Group))
    {
        to_jacobian(out, generator);
    }
    else
    {
        jacobian_t<uint64_t> stride{};
        to_jacobian(stride, generator);
        for (size_t bit{}; bit <= group_bits; ++bit)
            double_(stride, stride);

        add(out, starts<sub1(Group)>, stride);
    }

    return out;
}

// Multiples (2i + 1)G of the block, and running products of their z within
// the group.
template <size_t Group, size_t Block>
constexpr algorithm::forward_t algorithm::forward() NOEXCEPT
{
    jacobian_t<uint64_t> first{}, twice{};
    to_jacobian(first, generator);
    double_(twice, first);

    forward_t out{};
    if constexpr (is_zero(Block))
    {
        out.points[0] = starts<Group>;
        out.products[0] = out.points[0].z;
    }
    else
    {
        const auto& prior = forwards<Group, sub1(Block)>;
        add(out.points[0], prior.points.back(), twice);
        multiply(out.products[0], prior.products.back(), out.points[0].z);
    }

    for (auto point = one; point < block_size; ++point)
    {
        add(out.points[point], out.points[sub1(point)], twice);
        multiply(out.products[point], out.products[sub1(point)],
            out.points[point].z);
    }

    return out;
}

// The block after the last of a group holds only the inverse of the group
// product, and each block unwinds its prefix products into affine points.
template <size_t Group, size_t Block>
constexpr algorithm::backward_t algorithm::backward() NOEXCEPT
{
    constexpr auto last = sub1(group_blocks);

    backward_t out{};
    if constexpr (Block > last)
    {
        inverse(out.inverse, forwards<Group, last>.products.back());
    }
    else
    {
        const auto& chunk = forwards<Group, Block>;
        field_t<uint64_t> prior{ 1 };
        if constexpr (is_nonzero(Block))
            prior = forwards<Group, sub1(Block)>.products.back();

        out.inverse = backwards<Group, add1(Block)>.inverse;

        field_t<uint64_t> inverse_z{};
        for (auto point = block_size; is_nonzero(point--);)
        {
            const auto& before = is_zero(point) ? prior :
                chunk.products[sub1(point)];

            multiply(inverse_z, out.inverse, before);
            multiply(out.inverse, out.inverse, chunk.points[point].z);
            to_affine(out.points[point], chunk.points[point], inverse_z);
        }
    }

    return out;
}

template <size_t Block, bool Mapped>
constexpr algorithm::block_t algorithm::block() NOEXCEPT
{
    const auto& chunk = backwards<Block / group_blocks, Block % group_blocks>;

    block_t out{};
    for (size_t point{}; point < block_size; ++point)
    {
        auto entry = chunk.points[point];
        if constexpr (Mapped)
        {
            endomorphism(entry, entry);
            normalize(entry.x);
        }

        for (size_t limb{}; limb < entry.x.size(); ++limb)
        {
            out.x[limb][point] = entry.x[limb];
            out.y[limb][point] = entry.y[limb];
        }
    }

    return out;
}

template <size_t Bits, bool Mapped, size_t... Blocks>
constexpr algorithm::table_t<Bits> algorithm::tabulate(
    std::index_sequence<Blocks...>) NOEXCEPT
{
    return { blocks<Blocks, Mapped>... };
}

template <size_t Bits, bool Mapped>
constexpr algorithm::table_t<Bits> algorithm::tabulate() NOEXCEPT
{
    return tabulate<Bits, Mapped>(std::make_index_sequence<block_count<Bits>>{});
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
