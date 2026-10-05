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
// fill their lanes with the generator and distinct small scalars, and are not
// read. Lanes with an exceptional addition verify alone.
template <typename Word>
bool algorithm::verify_ecdsa(data_chunk& results,
    const std::span<const ec_compressed>& keys,
    const std::span<const hash_digest>& hashes,
    const std::span<const ec_signature>& signatures) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    constexpr auto size = array_count<bytes_t>;
    const auto count = keys.size();
    results.assign(count, uint8_t{});

    data_chunk valid(count);
    std_vector<scalar_t> r(count), w(count), z(count);
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
            u2[lane] = { 2 };
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
    const std::span<const ec_xonly>& keys,
    const std::span<const hash_digest>& challenges,
    const std::span<const ec_signature>& signatures) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    constexpr auto size = array_count<bytes_t>;
    const auto count = keys.size();
    results.assign(count, uint8_t{});

    std_vector<size_t> pending{};
    std_vector<jacobian_t<uint64_t>> sums{};
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
            e[lane] = { 2 };
            if (row >= count)
                continue;

            const auto& key = keys[row];
            const auto& signature = signatures[row];
            const auto& r = array_cast<uint8_t, size>(signature);
            field_t<uint64_t> rx{};
            if (!from_bytes(xs[lane], key) || !from_bytes(rx, r) ||
                !from_bytes(s[lane],
                    array_cast<uint8_t, size, size>(signature)))
            {
                xs[lane] = generator.x;
                s[lane] = { 1 };
                continue;
            }

            /* bool */ from_bytes(e[lane], challenges[row]);
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
                const auto& signature = signatures[row];
                results[row] = to_int<uint8_t>(verify_schnorr(keys[row],
                    challenges[row], array_cast<uint8_t, size>(signature),
                    array_cast<uint8_t, size, size>(signature)));
            }
            else if (is_zero(rows[lane].infinity))
            {
                pending.push_back(row);
                sums.push_back(rows[lane]);
            }
        }
    }

    std_vector<field_t<uint64_t>> inverses(sums.size());
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

// Batch multiplication.
// ----------------------------------------------------------------------------
// protected

// Rows without a valid point fill their lanes with the generator and are not
// read. Lanes with an exceptional addition compute alone.
template <typename Word>
void algorithm::multiply(data_chunk& valid, std_vector<ec_compressed>& out,
    const std::span<const ec_compressed>& points, const scalar_t& k) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    const auto count = points.size();
    valid.assign(count, uint8_t{});
    out.assign(count, ec_compressed{});

    std_vector<size_t> pending{};
    std_vector<jacobian_t<uint64_t>> sums{};
    pending.reserve(count);
    sums.reserve(count);

    scalars_t<Word> ks{};
    ks.fill(k);

    for (size_t base{}; base < count; base += width)
    {
        std_array<field_t<uint64_t>, width> xs{};
        std_array<uint64_t, width> odds{};
        std_array<bool, width> used{};
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            xs[lane] = generator.x;
            if (row >= count)
                continue;

            const auto& key = points[row];
            const auto sign = key.front();
            if ((sign != ec_even_sign && sign != ec_odd_sign) ||
                !from_bytes<one>(xs[lane], key))
            {
                xs[lane] = generator.x;
                continue;
            }

            odds[lane] = sign == ec_odd_sign ? max_uint64 : 0_u64;
            used[lane] = true;
        }

        field_t<Word> x{};
        pack(x, xs);
        affine_t<Word> point{};
        const auto on = unpack(lift(point, x, pack<Word>(odds)));

        jacobian_t<Word> sum{};
        const auto faults = unpack(multiply(sum, point, ks));

        std_array<jacobian_t<uint64_t>, width> rows{};
        unpack(rows, sum);
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            if (!used[lane] || is_zero(on[lane]))
                continue;

            if (is_nonzero(faults[lane]))
            {
                affine_t<uint64_t> alone{}, product{};
                if (from_bytes(alone, points[row]) &&
                    linear(product, {}, alone, k))
                {
                    to_bytes(out[row], product);
                    valid[row] = 1;
                }
            }
            else if (is_zero(rows[lane].infinity))
            {
                pending.push_back(row);
                sums.push_back(rows[lane]);
                valid[row] = 1;
            }
        }
    }

    to_keys(out, pending, sums);
}

// Each row is t * G by comb (one addition per digit, no doubling) plus the
// point and each addend, and a row with an exceptional addition computes alone.
inline void algorithm::tweak_comb(data_chunk& valid,
    std_vector<ec_xonly>& out, const std::span<const scalar_t>& tweaks,
    const affine_t<uint64_t>& point,
    const std::span<const affine_t<uint64_t>>& addends) NOEXCEPT
{
    const auto count = tweaks.size();
    const auto stride = add1(addends.size());
    valid.assign(count, uint8_t{});
    out.assign(count * stride, ec_xonly{});

    std_vector<size_t> pending{};
    std_vector<jacobian_t<uint64_t>> sums{};
    pending.reserve(count * stride);
    sums.reserve(count * stride);

    std_vector<jacobian_t<uint64_t>> row(stride);
    for (size_t index{}; index < count; ++index)
    {
        uint64_t faults{};
        auto& sum = row.front();
        sum = {};
        sum.infinity = max_uint64;
        add_comb(sum, tweaks[index], faults);
        add_point<false>(sum, point, faults);
        for (size_t addend{}; addend < addends.size(); ++addend)
        {
            row[add1(addend)] = sum;
            add_point<false>(row[add1(addend)], addends[addend], faults);
        }

        if (is_nonzero(faults))
        {
            const std::span<ec_xonly> keys{ &out[index * stride], stride };
            const auto& t = tweaks[index];
            valid[index] = to_int<uint8_t>(tweak(keys, t, point, addends));
            continue;
        }

        const auto infinity = std::any_of(row.cbegin(), row.cend(),
            [](const auto& value) NOEXCEPT
            {
                return is_nonzero(value.infinity);
            });

        if (infinity)
            continue;

        for (size_t position{}; position < stride; ++position)
        {
            pending.push_back(index * stride + position);
            sums.push_back(row[position]);
        }

        valid[index] = 1;
    }

    to_keys(out, pending, sums);
}

// Integral rows are faster by comb, and lanes by multiplication.
template <typename Word>
void algorithm::tweak(data_chunk& valid, std_vector<ec_xonly>& out,
    const std::span<const scalar_t>& tweaks,
    const affine_t<uint64_t>& point,
    const std::span<const affine_t<uint64_t>>& addends) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
        tweak_comb(valid, out, tweaks, point, addends);
    else
        tweak_lanes<Word>(valid, out, tweaks, point, addends);
}

// Each addend is one lane addition to the sum of a row, and a row with an
// exceptional addition computes alone.
template <typename Word>
void algorithm::tweak_lanes(data_chunk& valid, std_vector<ec_xonly>& out,
    const std::span<const scalar_t>& tweaks,
    const affine_t<uint64_t>& point,
    const std::span<const affine_t<uint64_t>>& addends) NOEXCEPT
{
    constexpr auto width = lanes<Word>;
    const auto count = tweaks.size();
    const auto stride = add1(addends.size());
    valid.assign(count, uint8_t{});
    out.assign(count * stride, ec_xonly{});

    std_vector<size_t> pending{};
    std_vector<jacobian_t<uint64_t>> sums{};
    pending.reserve(count * stride);
    sums.reserve(count * stride);

    const auto lane_point = to_lanes<Word>(point);
    std_vector<affine_t<Word>> lane_addends{};
    lane_addends.reserve(addends.size());
    for (const auto& addend: addends)
        lane_addends.push_back(to_lanes<Word>(addend));

    scalars_t<Word> ones{};
    ones.fill({ 1 });

    std_vector<std_array<jacobian_t<uint64_t>, width>> rows(stride);
    for (size_t base{}; base < count; base += width)
    {
        scalars_t<Word> ts{};
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            ts[lane] = row < count ? tweaks[row] : scalar_t{ 1 };
        }

        jacobian_t<Word> sum{};
        auto faults = multiply(sum, ts, lane_point, ones);
        unpack(rows.front(), sum);
        for (size_t index{}; index < addends.size(); ++index)
        {
            auto added = sum;
            add_point<false>(added, lane_addends[index], faults);
            unpack(rows[add1(index)], added);
        }

        const auto exceptional = unpack(faults);
        for (size_t lane{}; lane < width; ++lane)
        {
            const auto row = base + lane;
            if (row >= count)
                continue;

            const std::span<ec_xonly> keys{ &out[row * stride], stride };
            if (is_nonzero(exceptional[lane]))
            {
                const auto& t = tweaks[row];
                valid[row] = to_int<uint8_t>(tweak(keys, t, point, addends));
                continue;
            }

            const auto infinity = std::any_of(rows.cbegin(), rows.cend(),
                [lane](const auto& lane_sums) NOEXCEPT
                {
                    return is_nonzero(lane_sums[lane].infinity);
                });

            if (infinity)
                continue;

            for (size_t index{}; index < stride; ++index)
            {
                pending.push_back(row * stride + index);
                sums.push_back(rows[index][lane]);
            }

            valid[row] = 1;
        }
    }

    to_keys(out, pending, sums);
}

// Batch internals.
// ----------------------------------------------------------------------------
// protected

constexpr void algorithm::to_bytes(ec_compressed& out,
    const affine_t<uint64_t>& a) NOEXCEPT
{
    bytes_t x{};
    to_bytes(x, a.x);
    out.front() = f::any(is_odd_element(a.y)) ? ec_odd_sign : ec_even_sign;
    for (size_t byte{}; byte < x.size(); ++byte)
        out[add1(byte)] = x[byte];
}

template <typename Word>
constexpr algorithm::affine_t<Word> algorithm::to_lanes(
    const affine_t<uint64_t>& a) NOEXCEPT
{
    return { broadcast<Word>(a.x), broadcast<Word>(a.y) };
}

// Sums convert to affine by prefix products that share one inversion.
template <typename Key>
void algorithm::to_keys(std_vector<Key>& out,
    const std_vector<size_t>& pending,
    const std_vector<jacobian_t<uint64_t>>& sums) NOEXCEPT
{
    std_vector<field_t<uint64_t>> inverses(sums.size());
    for (size_t index{}; index < sums.size(); ++index)
        inverses[index] = sums[index].z;

    inverse(inverses);

    for (size_t index{}; index < sums.size(); ++index)
    {
        affine_t<uint64_t> point{};
        to_affine(point, sums[index], inverses[index]);
        if constexpr (is_same_type<Key, ec_compressed>)
            to_bytes(out[pending[index]], point);
        else
            to_bytes(out[pending[index]], point.x);
    }
}

inline bool algorithm::tweak(const std::span<ec_xonly>& out, const scalar_t& t,
    const affine_t<uint64_t>& point,
    const std::span<const affine_t<uint64_t>>& addends) NOEXCEPT
{
    affine_t<uint64_t> sum{};
    if (!linear(sum, t, point))
        return false;

    jacobian_t<uint64_t> base{};
    to_jacobian(base, sum);
    to_bytes(out.front(), sum.x);
    for (size_t index{}; index < addends.size(); ++index)
    {
        jacobian_t<uint64_t> added{};
        add_complete(added, base, addends[index]);
        if (f::any(added.infinity))
            return false;

        affine_t<uint64_t> result{};
        to_affine(result, added);
        to_bytes(out[add1(index)], result.x);
    }

    return true;
}

// Prefix products share one inversion, unwound from the last value.
template <typename Element>
void algorithm::inverse(std_vector<Element>& values) NOEXCEPT
{
    if (values.empty())
        return;

    std_vector<Element> prefix(values.size());
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
algorithm::words_t<Word> algorithm::unpack(Word value) NOEXCEPT
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
        words_t<Word> values{};
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
