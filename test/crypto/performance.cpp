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
#include "../test.hpp"
#include "../hash/performance/performance.hpp"

#include <cmath>
#include <random>

#if defined(HAVE_PERFORMANCE_TESTS)

using namespace performance;

BOOST_AUTO_TEST_SUITE(performance_cipher_tests)

// A tls record.
constexpr size_t record = 16 * 1024;
constexpr size_t records = 1024;

template <typename Function>
static void report_cipher(const std::string& name, const Function& function)
{
    const auto data = get_bytes(record);
    data_chunk out(record + 16u);
    report(name, nanoseconds(records, [&]() noexcept
    {
        function(data, out);
    }) / record, "byte");
}

template <typename Algorithm>
static void report_ctr(const std::string& name)
{
    const auto schedule = Algorithm::expand({});
    report_cipher(name, [&](const data_chunk& in, data_chunk& out) noexcept
    {
        typename Algorithm::block_t counter{};
        Algorithm::ctr(std::span{ out }.first(in.size()), in, counter, schedule);
    });
}

BOOST_AUTO_TEST_CASE(performance__aes128_ctr__variants)
{
    report_ctr<aes::algorithm<aes::k128, true, true>>("aes128_ctr_native");
    report_ctr<aes::algorithm<aes::k128, false, true>>("aes128_ctr_vector");
    report_ctr<aes::algorithm<aes::k128, false, false>>("aes128_ctr_sliced");
}

BOOST_AUTO_TEST_CASE(performance__aead__aes128_gcm_chacha20_poly1305)
{
    aes128_gcm gcm{ {} };
    report_cipher("aes128_gcm", [&](const data_chunk& in, data_chunk& out) noexcept
    {
        gcm.encrypt(in, {}, {}, out);
    });

    chacha20_poly1305 chacha{ {} };
    report_cipher("chacha20_poly1305", [&](const data_chunk& in, data_chunk& out) noexcept
    {
        chacha.encrypt(in, {}, 0, 0, out);
    });
}

constexpr size_t operations = 1024;

template <typename Function>
static void report_operations(const std::string& name, const Function& function)
{
    size_t valid{};
    report(name, nanoseconds(operations, [&]() noexcept
    {
        valid += to_int<size_t>(function());
    }), "operation");

    BOOST_CHECK_EQUAL(valid, repeats * operations);
}

template <typename Curve>
static void report_ecdsa(const std::string& name)
{
    const auto secret = Curve::generate();
    const data_array<Curve::size> digest{ 42 };

    typename Curve::point_t point{};
    typename Curve::signature_t signature{};
    BOOST_REQUIRE(Curve::public_key(point, secret));
    BOOST_REQUIRE(Curve::sign(signature, secret, digest));

    report_operations(name + "_sign", [&]() noexcept
    {
        return Curve::sign(signature, secret, digest);
    });

    report_operations(name + "_verify", [&]() noexcept
    {
        return Curve::verify(signature, point, digest);
    });
}

BOOST_AUTO_TEST_CASE(performance__ecdsa__secp256r1_secp384r1)
{
    report_ecdsa<secp256r1>("secp256r1");
    report_ecdsa<secp384r1>("secp384r1");
}

// Welch's t statistic of signing time, a fixed secret against random secrets
// (dudect), samples above the pooled percentile cropped. |t| > 4.5 is a leak.
template <typename Curve>
static double sign_timing(const typename Curve::secret_t& fixed,
    size_t samples, double percentile)
{
    const data_array<Curve::size> digest{ 42 };
    std::mt19937_64 source{ 42 };
    std::vector<bool> fixeds(samples);
    std::vector<typename Curve::secret_t> secrets(samples);
    for (size_t index{}; index < samples; ++index)
    {
        fixeds[index] = is_odd(source());
        secrets[index] = fixeds[index] ? fixed : Curve::generate();
    }

    std::vector<double> times(samples);
    typename Curve::signature_t signature{};
    for (size_t index{}; index < samples; ++index)
    {
        const auto start = std::chrono::steady_clock::now();
        Curve::sign(signature, secrets[index], digest);
        const auto stop = std::chrono::steady_clock::now();
        times[index] = duration(stop - start).count();
    }

    auto sorted = times;
    const auto rank = to_floored_integer<ptrdiff_t>(percentile * samples);
    const auto cut = std::next(sorted.begin(), rank);
    std::nth_element(sorted.begin(), cut, sorted.end());

    std_array<double, 2> count{}, mean{}, square{};
    for (size_t index{}; index < samples; ++index)
    {
        if (times[index] > *cut)
            continue;

        // Welford's running mean and sum of squared deviations.
        const auto group = to_int<size_t>(fixeds[index]);
        const auto delta = times[index] - mean[group];
        count[group] += 1.0;
        mean[group] += delta / count[group];
        square[group] += delta * (times[index] - mean[group]);
    }

    const auto error0 = square[0] / (count[0] - 1.0) / count[0];
    const auto error1 = square[1] / (count[1] - 1.0) / count[1];
    return (mean[0] - mean[1]) / std::sqrt(error0 + error1);
}

BOOST_AUTO_TEST_CASE(performance__ecdsa__secp256r1_sign__constant_time)
{
    secp256r1::secret_t fixed{};
    fixed.back() = 1;
    const auto t = sign_timing<secp256r1>(fixed, 40000, 0.9);
    std::cout << "secp256r1_sign_welch_t: " << t << std::endl;
    BOOST_CHECK_LT(std::abs(t), 4.5);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
