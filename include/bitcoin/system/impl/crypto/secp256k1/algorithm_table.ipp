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

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
