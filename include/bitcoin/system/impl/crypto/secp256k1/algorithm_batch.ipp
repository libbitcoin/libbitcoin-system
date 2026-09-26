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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_BATCH_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_BATCH_IPP

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Batch verification.
// ----------------------------------------------------------------------------
// protected

// The s of all rows invert together. Rows without a valid signature or key
// fill their lanes with the generator and unit scalars, and are not read.
// Lanes with an exceptional addition verify alone.
template <typename Word>
bool algorithm::verify_ecdsa(data_chunk& results,
    std::span<const ec_compressed> keys, std::span<const hash_digest> hashes,
    std::span<const ec_signature> signatures) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    constexpr auto size = array_count<bytes_t>;
    const auto count = keys.size();
    results.assign(count, uint8_t{});

    data_chunk valid(count);
    std::vector<scalar_t> r(count), w(count), z(count);
    for (size_t row{}; row < count; ++row)
    {
        const auto& signature = signatures[row];
        valid[row] = to_int<uint8_t>(
            from_bytes(r[row], array_cast<uint8_t, size>(signature)) &&
            from_bytes(w[row], array_cast<uint8_t, size, size>(signature)) &&
            !is_zero_scalar(r[row]) && !is_zero_scalar(w[row]));

        if (is_zero(valid[row]))
            w[row] = { 1 };

        /* bool */ from_bytes(z[row], hashes[row]);
    }

    inverse(w);

    for (size_t base{}; base < count; base += width)
    {
        std_array<field_t<uint64_t>, width> xs{}, rs{};
        std_array<uint64_t, width> odds{}, overflows{};
        scalars_t<Word> u1{}, u2{};
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            xs[lane] = generator.x;
            u1[lane] = { 1 };
            u2[lane] = { 1 };
            if (row >= count || is_zero(valid[row]))
                continue;

            const auto& key = keys[row];
            const auto sign = key.front();
            if ((sign != ec_even_sign && sign != ec_odd_sign) ||
                !from_bytes<one>(xs[lane], key))
            {
                valid[row] = 0;
                xs[lane] = generator.x;
                continue;
            }

            odds[lane] = sign == ec_odd_sign ? max_uint64 : 0_u64;
            overflows[lane] = is_less(r[row], prime_minus_order) ?
                max_uint64 : 0_u64;

            multiply(u1[lane], z[row], w[row]);
            multiply(u2[lane], r[row], w[row]);
            to_field(rs[lane], r[row]);
        }

        field_t<Word> x{};
        pack(x, xs);
        affine_t<Word> point{};
        const auto on = unpack(lift(point, x, pack<Word>(odds)));

        jacobian_t<Word> sum{};
        const auto faults = unpack(multiply(sum, u1, point, u2));

        // x(R) mod n is r, as X = r * Z^2, or X = (r + n) * Z^2 if r + n < p.
        field_t<Word> rx{}, zz{}, expected{};
        pack(rx, rs);
        normalize(sum.x);
        square(zz, sum.z);
        multiply(expected, rx, zz);
        normalize(expected);
        auto match = equal(expected, sum.x);

        const auto overflow = pack<Word>(overflows);
        if (f::any(overflow))
        {
            add(rx, rx, broadcast<Word>(order_field));
            carry(rx);
            multiply(expected, rx, zz);
            normalize(expected);
            match = f::or_(match, f::and_(overflow, equal(expected, sum.x)));
        }

        const auto matches = unpack(match);
        const auto infinities = unpack(sum.infinity);
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            if (row >= count || is_zero(valid[row]) ||
                is_zero(on[lane]))
                continue;

            if (is_nonzero(faults[lane]))
            {
                const auto& signature = signatures[row];
                affine_t<uint64_t> alone{};
                results[row] = to_int<uint8_t>(
                    from_bytes(alone, keys[row]) &&
                    verify_ecdsa(alone, hashes[row],
                        array_cast<uint8_t, size>(signature),
                        array_cast<uint8_t, size, size>(signature)));
            }
            else
            {
                results[row] = to_int<uint8_t>(is_nonzero(matches[lane]) &&
                    is_zero(infinities[lane]));
            }
        }
    }

    return std::all_of(results.begin(), results.end(),
        [](uint8_t result) NOEXCEPT { return is_nonzero(result); });
}

// Rows computed in lanes convert to affine together by one inversion, and
// lanes with an exceptional addition verify alone.
template <typename Word>
bool algorithm::verify_schnorr(data_chunk& results,
    std::span<const ec_xonly> keys, std::span<const hash_digest> messages,
    std::span<const ec_signature> signatures) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    constexpr auto size = array_count<bytes_t>;
    const auto count = keys.size();
    results.assign(count, uint8_t{});

    std::vector<size_t> pending{};
    std::vector<jacobian_t<uint64_t>> sums{};
    pending.reserve(count);
    sums.reserve(count);

    for (size_t base{}; base < count; base += width)
    {
        std_array<field_t<uint64_t>, width> xs{};
        std_array<bool, width> used{};
        scalars_t<Word> s{}, e{};
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            xs[lane] = generator.x;
            s[lane] = { 1 };
            e[lane] = { 1 };
            if (row >= count)
                continue;

            const auto& key = keys[row];
            const auto& signature = signatures[row];
            const auto& r = array_cast<uint8_t, size>(signature);
            field_t<uint64_t> rx{};
            if (!from_bytes(xs[lane], key) || !from_bytes(rx, r) ||
                !from_bytes(s[lane], array_cast<uint8_t, size, size>(signature)))
            {
                xs[lane] = generator.x;
                s[lane] = { 1 };
                continue;
            }

            /* bool */ from_bytes(e[lane], challenge(r, key, messages[row]));
            negate(e[lane], e[lane]);
            used[lane] = true;
        }

        field_t<Word> x{};
        pack(x, xs);
        affine_t<Word> point{};
        const auto on = unpack(lift(point, x, f::broadcast<Word>(0_u64)));

        jacobian_t<Word> sum{};
        const auto faults = unpack(multiply(sum, s, point, e));

        std_array<jacobian_t<uint64_t>, width> rows{};
        unpack(rows, sum);
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            if (!used[lane] || is_zero(on[lane]))
                continue;

            if (is_nonzero(faults[lane]))
            {
                const auto& key = keys[row];
                const auto& signature = signatures[row];
                const auto& r = array_cast<uint8_t, size>(signature);
                results[row] = to_int<uint8_t>(verify_schnorr(key,
                    challenge(r, key, messages[row]), r,
                    array_cast<uint8_t, size, size>(signature)));
            }
            else if (is_zero(rows[lane].infinity))
            {
                pending.push_back(row);
                sums.push_back(rows[lane]);
            }
        }
    }

    std::vector<field_t<uint64_t>> inverses(sums.size());
    for (size_t index{}; index < sums.size(); ++index)
        inverses[index] = sums[index].z;

    inverse(inverses);

    for (size_t index{}; index < sums.size(); ++index)
    {
        const auto row = pending[index];
        affine_t<uint64_t> point{};
        field_t<uint64_t> rx{};
        to_affine(point, sums[index], inverses[index]);
        /* bool */ from_bytes(rx, array_cast<uint8_t, size>(signatures[row]));
        results[row] = to_int<uint8_t>(!f::any(is_odd_element(point.y)) &&
            f::any(equal(point.x, rx)));
    }

    return std::all_of(results.begin(), results.end(),
        [](uint8_t result) NOEXCEPT { return is_nonzero(result); });
}

// Batch internals.
// ----------------------------------------------------------------------------
// protected

// Prefix products share one inversion, unwound from the last value.
template <typename Element>
void algorithm::inverse(std::vector<Element>& values) NOEXCEPT
{
    if (values.empty())
        return;

    std::vector<Element> prefix(values.size());
    prefix.front() = values.front();
    for (auto index = one; index < values.size(); ++index)
        multiply(prefix[index], prefix[sub1(index)], values[index]);

    Element inverted{};
    inverse(inverted, prefix.back());
    for (auto index = sub1(values.size()); is_nonzero(index); --index)
    {
        Element value{};
        multiply(value, inverted, prefix[sub1(index)]);
        multiply(inverted, inverted, values[index]);
        values[index] = value;
    }

    values.front() = inverted;
}

template <typename Word>
std_array<uint64_t, algorithm::lanes<Word>> algorithm::unpack(
    Word value) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        return { value };
    }
    else if constexpr (lanes<Word> == 2)
    {
        return { f::get<uint64_t, 0>(value), f::get<uint64_t, 1>(value) };
    }
    else if constexpr (lanes<Word> == 4)
    {
        return
        {
            f::get<uint64_t, 0>(value), f::get<uint64_t, 1>(value),
            f::get<uint64_t, 2>(value), f::get<uint64_t, 3>(value)
        };
    }
    else
    {
        return
        {
            f::get<uint64_t, 0>(value), f::get<uint64_t, 1>(value),
            f::get<uint64_t, 2>(value), f::get<uint64_t, 3>(value),
            f::get<uint64_t, 4>(value), f::get<uint64_t, 5>(value),
            f::get<uint64_t, 6>(value), f::get<uint64_t, 7>(value)
        };
    }
}

template <typename Word>
void algorithm::pack(field_t<Word>& r,
    const std_array<field_t<uint64_t>, lanes<Word>>& rows) NOEXCEPT
{
    for (size_t limb{}; limb < r.size(); ++limb)
    {
        std_array<uint64_t, lanes<Word>> values{};
        for (size_t lane{}; lane < values.size(); ++lane)
            values[lane] = rows[lane][limb];

        r[limb] = pack<Word>(values);
    }
}

template <typename Word>
void algorithm::unpack(std_array<jacobian_t<uint64_t>, lanes<Word>>& rows,
    const jacobian_t<Word>& a) NOEXCEPT
{
    for (size_t limb{}; limb < a.x.size(); ++limb)
    {
        const auto x = unpack(a.x[limb]);
        const auto y = unpack(a.y[limb]);
        const auto z = unpack(a.z[limb]);
        for (size_t lane{}; lane < rows.size(); ++lane)
        {
            rows[lane].x[limb] = x[lane];
            rows[lane].y[limb] = y[lane];
            rows[lane].z[limb] = z[lane];
        }
    }

    const auto infinity = unpack(a.infinity);
    for (size_t lane{}; lane < rows.size(); ++lane)
        rows[lane].infinity = infinity[lane];
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
