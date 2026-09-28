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

#if defined(HAVE_PERFORMANCE_TESTS)

using namespace performance;

BOOST_AUTO_TEST_SUITE(performance_cipher_tests)

// A tls record.
constexpr size_t record = 16 * 1024;
constexpr size_t rounds = 16 * 1024;
constexpr size_t bytes = record * rounds;

template <typename Function>
static void report(const std::string& name, const Function& function)
{
    const auto data = get_data<record, true>(42);
    data_chunk out(record + 16u);

    using Timer = timer<std::chrono::nanoseconds>;
    const auto time = Timer::execution([&]() noexcept
    {
        for (size_t round{}; round < rounds; ++round)
            function(*data, out);
    });

    const auto seconds = seconds_total<std::chrono::nanoseconds>(time);
    std::cout << name
        << " mib_per_second: " << mib_per_second<bytes>(seconds)
        << " cycles_per_byte (3 ghz): " << cycles_per_byte<bytes>(seconds, 3.0f)
        << std::endl;
}

template <typename Algorithm>
static void report_ctr(const std::string& name)
{
    const auto schedule = Algorithm::expand({});
    report(name, [&](const data_chunk& in, data_chunk& out) noexcept
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
    report("aes128_gcm", [&](const data_chunk& in, data_chunk& out) noexcept
    {
        gcm.encrypt(in, {}, {}, out);
    });

    chacha20_poly1305 chacha{ {} };
    report("chacha20_poly1305", [&](const data_chunk& in, data_chunk& out) noexcept
    {
        chacha.encrypt(in, {}, 0, 0, out);
    });
}

constexpr size_t operations = 1024;

template <typename Function>
static void report_operations(const std::string& name, const Function& function)
{
    size_t valid{};
    using Timer = timer<std::chrono::nanoseconds>;
    const auto time = Timer::execution([&]() noexcept
    {
        for (size_t operation{}; operation < operations; ++operation)
            valid += to_int<size_t>(function());
    });

    const auto seconds = seconds_total<std::chrono::nanoseconds>(time);
    std::cout << name
        << " operations_per_second: " << (operations / seconds)
        << " valid: " << valid
        << std::endl;
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

BOOST_AUTO_TEST_SUITE_END()

#endif
