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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_PIPPENGER_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_PIPPENGER_IPP

// Based on:
// github.com/bitcoin/bips/blob/master/bip-0340.mediawiki (batch verification)
// Pippenger bucket method of multiscalar multiplication.

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Batch verification.
// ----------------------------------------------------------------------------
// protected

// All rows are valid if sum(a R) + sum(a e P) - sum(a s)G is infinity, for
// weights a of one (first) and 128 random bits (others), derived by hashing
// the batch.
inline bool algorithm::verify_schnorr(std::span<const ec_xonly> keys,
    std::span<const hash_digest> challenges,
    std::span<const ec_signature> signatures) NOEXCEPT
{
    constexpr auto size = array_count<bytes_t>;
    const auto count = keys.size();

    accumulator<sha256> seeder{};
    for (size_t row{}; row < count; ++row)
    {
        seeder.write(keys[row]);
        seeder.write(challenges[row]);
        seeder.write(signatures[row]);
    }

    const auto seed = seeder.flush();
    std_vector<term_t> terms{};
    terms.reserve(add1(two) * count + two);
    scalar_t total{};

    for (size_t row{}; row < count; ++row)
    {
        const auto& key = keys[row];
        const auto& signature = signatures[row];
        const auto& r = array_cast<uint8_t, size>(signature);

        field_t<uint64_t> key_x{}, r_x{};
        affine_t<uint64_t> point{}, nonce{};
        scalar_t s{}, e{};
        if (!from_bytes(key_x, key) || !from_bytes(r_x, r) ||
            !from_bytes(s, array_cast<uint8_t, size, size>(signature)) ||
            !f::any(lift(point, key_x, 0_u64)) ||
            !f::any(lift(nonce, r_x, 0_u64)))
            return false;

        scalar_t weight{ 1 };
        if (is_nonzero(row))
        {
            accumulator<sha256> weigher{};
            weigher.write(seed);
            weigher.write(to_little(possible_wide_cast<uint64_t>(row)));
            const auto hash = weigher.flush();
            from_little<0>(weight[0], hash);
            from_little<sizeof(uint64_t)>(weight[1], hash);
            set_right_into(weight[0]);
        }

        /* bool */ from_bytes(e, challenges[row]);
        multiply(e, e, weight);
        multiply(s, s, weight);
        add(total, total, s);
        append(terms, point, e);
        terms.push_back({ nonce, weight });
    }

    negate(total, total);
    append(terms, generator, total);

    jacobian_t<uint64_t> sum{};
    multiply(sum, terms);
    return f::any(sum.infinity);
}

// Multiscalar multiplication.
// ----------------------------------------------------------------------------
// protected

// Each window of bits is a signed digit, with a carry into the next window
// for digits above half the window, and each point adds into the bucket of
// its digit. Buckets sum by running sums from the top, and windows by Horner.
inline void algorithm::multiply(jacobian_t<uint64_t>& r,
    std::span<const term_t> terms) NOEXCEPT
{
    const auto width = bucket_bits(terms.size());
    const auto windows = add1(ceilinged_divide(half_bits, width));
    const auto half = power2<uint64_t>(sub1(width));
    const auto full = power2<uint64_t>(width);
    const auto mask = unmask_right<uint64_t>(width);

    jacobian_t<uint64_t> empty{};
    empty.infinity = max_uint64;
    std_vector<jacobian_t<uint64_t>> buckets(possible_narrow_cast<size_t>(half));
    std_vector<jacobian_t<uint64_t>> sums(windows);
    data_chunk carries(terms.size());

    for (size_t window{}; window < windows; ++window)
    {
        std::fill(buckets.begin(), buckets.end(), empty);
        const auto offset = window * width;
        const auto limb = offset / bits<uint64_t>;
        const auto shift = offset % bits<uint64_t>;

        for (size_t index{}; index < terms.size(); ++index)
        {
            const auto& term = terms[index];
            auto value = uint64_t{ carries[index] };
            if (limb < array_count<scalar_t>)
            {
                auto window_bits = term.scalar[limb] >> shift;
                if (is_nonzero(shift) && add1(limb) < array_count<scalar_t>)
                    window_bits |= term.scalar[add1(limb)] <<
                        (bits<uint64_t> - shift);

                value += window_bits & mask;
            }

            const auto negative = value > half;
            carries[index] = to_int<uint8_t>(negative);
            if (negative)
                value = full - value;

            if (is_zero(value))
                continue;

            affine_t<uint64_t> addend{ term.point };
            if (negative)
                negate(addend, term.point);

            auto& bucket = buckets[possible_narrow_cast<size_t>(sub1(value))];
            if (f::any(bucket.infinity))
                to_jacobian(bucket, addend);
            else
                add_complete(bucket, bucket, addend);
        }

        auto running = empty;
        auto window_sum = empty;
        for (auto bucket = buckets.size(); is_nonzero(bucket--);)
        {
            if (!f::any(buckets[bucket].infinity))
            {
                if (f::any(running.infinity))
                    running = buckets[bucket];
                else
                    add_complete(running, running, buckets[bucket]);
            }

            if (!f::any(running.infinity))
            {
                if (f::any(window_sum.infinity))
                    window_sum = running;
                else
                    add_complete(window_sum, window_sum, running);
            }
        }

        sums[window] = window_sum;
    }

    auto sum = empty;
    for (auto window = windows; is_nonzero(window--);)
    {
        if (!f::any(sum.infinity))
            for (size_t bit{}; bit < width; ++bit)
                double_(sum, sum);

        add_complete(sum, sum, sums[window]);
    }

    r = sum;
}

// Additions of a window are the count of terms and two per bucket.
constexpr size_t algorithm::bucket_bits(size_t count) NOEXCEPT
{
    constexpr size_t limit = 16;
    size_t best{ one }, least{ max_size_t };
    for (auto width = one; width <= limit; ++width)
    {
        const auto windows = add1(ceilinged_divide(half_bits, width));
        const auto additions = windows * (count + power2(width));
        if (additions < least)
        {
            least = additions;
            best = width;
        }
    }

    return best;
}

// Halves of k = k1 + k2 lambda add as magnitudes, the point negated if high.
inline void algorithm::append(std_vector<term_t>& terms,
    const affine_t<uint64_t>& point, const scalar_t& scalar) NOEXCEPT
{
    scalar_t first{}, second{};
    split(first, second, scalar);

    affine_t<uint64_t> mapped{};
    endomorphism(mapped, point);

    const auto push = [&](scalar_t& half, const affine_t<uint64_t>& base)
        NOEXCEPT
    {
        affine_t<uint64_t> term{ base };
        if (is_high(half))
        {
            negate(half, half);
            negate(term, base);
        }

        terms.push_back({ term, half });
    };

    push(first, point);
    push(second, mapped);
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
