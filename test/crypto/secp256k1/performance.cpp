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
#include "../../test.hpp"

#if defined(HAVE_PERFORMANCE_TESTS)

#include <chrono>

BOOST_AUTO_TEST_SUITE(secp256k1_performance_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    template <typename Word>
    using affine_t = algorithm::affine_t<Word>;
    template <typename Word>
    using jacobian_t = algorithm::jacobian_t<Word>;
    template <typename Word>
    using scalars_t = algorithm::scalars_t<Word>;
    using scalar_t = algorithm::scalar_t;
    using bytes_t = algorithm::bytes_t;
    using algorithm::multiply;
    using algorithm::pack;
    using algorithm::from_bytes;
    using algorithm::verify_ecdsa;
    using algorithm::verify_schnorr;
    using algorithm::square;
    using algorithm::is_zero_element;
    using algorithm::double_;
    using algorithm::add;
    using algorithm::to_jacobian;
};

using affine = accessor::affine_t<uint64_t>;
using scalar = accessor::scalar_t;
using bytes = accessor::bytes_t;

constexpr size_t count = 1024;
constexpr scalar left{ 0x123456789abcdef0, 0x0fedcba987654321, 0x1111111111111111, 0x2222222222222222 };
constexpr scalar right{ 0x0fedcba987654321, 0x123456789abcdef0, 0x3333333333333333, 0x4444444444444444 };

// helpers
// ----------------------------------------------------------------------------

struct vectors
{
    std::vector<ec_compressed> keys{};
    std::vector<ec_xonly> xonlys{};
    std::vector<hash_digest> hashes{};
    std::vector<ec_signature> ecdsas{};
    std::vector<ec_signature> canonicals{};
    std::vector<ec_signature> schnorrs{};
    std::vector<affine> points{};
};

static const vectors& signed_vectors() NOEXCEPT
{
    static const auto instance = []() NOEXCEPT
    {
        vectors out{};
        for (size_t index{}; index < count; ++index)
        {
            const auto secret = sha256_hash(to_chunk(std::to_string(index)));
            const auto hash = sha256_hash(secret);
            ec_compressed key{};
            ec_signature ecdsa{}, canonical{}, schnorr{};
            affine point{};
            secret_to_public(key, secret);
            ecdsa::sign(ecdsa, secret, hash);
            ecdsa::canonicalize_signature(canonical, ecdsa);
            schnorr::sign(schnorr, secret, hash, hash);
            accessor::from_bytes(point, key);
            out.keys.push_back(key);
            out.xonlys.push_back(array_cast<uint8_t, ec_xonly_size, one>(key));
            out.hashes.push_back(hash);
            out.ecdsas.push_back(ecdsa);
            out.canonicals.push_back(canonical);
            out.schnorrs.push_back(schnorr);
            out.points.push_back(point);
        }

        return out;
    }();

    return instance;
}

// BIP340 challenge hash of r, x-only key, and message.
static hash_digest challenge(const ec_signature& signature,
    const ec_xonly& key, const hash_digest& message) NOEXCEPT
{
    accumulator<sha256> context{ tagged_midstate<"BIP0340/challenge">, one };
    context.write(array_cast<uint8_t, ec_secret_size>(signature));
    context.write(key);
    context.write(message);
    return context.flush();
}

static std::vector<hash_digest> challenges(const vectors& in) NOEXCEPT
{
    std::vector<hash_digest> out{};
    out.reserve(count);
    for (size_t index{}; index < count; ++index)
        out.push_back(challenge(in.schnorrs[index], in.xonlys[index],
            in.hashes[index]));

    return out;
}

// Microseconds per call of function(index), which is expected to be true.
template <typename Function>
static double microseconds(size_t calls, Function&& function) NOEXCEPT
{
    size_t valid{};
    const auto start = std::chrono::steady_clock::now();
    for (size_t call{}; call < calls; ++call)
        valid += to_int<size_t>(function(call));

    const auto stop = std::chrono::steady_clock::now();
    BOOST_CHECK_EQUAL(valid, calls);
    return std::chrono::duration<double, std::micro>(stop - start).count() /
        calls;
}

static void report(const std::string& name, double time) NOEXCEPT
{
    std::cout << name << ": " << time << " us" << std::endl;
}

template <typename xWord>
static void report_lanes(const std::string& name) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        constexpr auto lanes = capacity<xWord, uint64_t>;
        const auto& in = signed_vectors();

        accessor::affine_t<xWord> points{};
        accessor::scalars_t<xWord> lefts{}, rights{};
        std_array<std_array<uint64_t, lanes>, 5> x{}, y{};
        for (size_t lane{}; lane < lanes; ++lane)
        {
            lefts[lane] = left;
            rights[lane] = right;
            for (size_t limb{}; limb < x.size(); ++limb)
            {
                x[limb][lane] = in.points[lane].x[limb];
                y[limb][lane] = in.points[lane].y[limb];
            }
        }

        for (size_t limb{}; limb < x.size(); ++limb)
        {
            points.x[limb] = accessor::pack<xWord>(x[limb]);
            points.y[limb] = accessor::pack<xWord>(y[limb]);
        }

        const auto time = microseconds(count / lanes, [&](size_t) NOEXCEPT
        {
            accessor::jacobian_t<xWord> out{};
            return !f::any(accessor::multiply(out, lefts, points, rights));
        });

        report(name, time / lanes);
    }
}

// field and group
// ----------------------------------------------------------------------------

constexpr size_t operations = 1'000'000;

BOOST_AUTO_TEST_CASE(secp256k1_performance__field__integral)
{
    const auto& in = signed_vectors();
    auto value = in.points[0].x;
    const auto& factor = in.points[1].y;
    report("field multiply", microseconds(operations, [&](size_t) NOEXCEPT
    {
        accessor::multiply(value, value, factor);
        return true;
    }));

    report("field square", microseconds(operations, [&](size_t) NOEXCEPT
    {
        accessor::square(value, value);
        return true;
    }));

    BOOST_CHECK(!f::any(accessor::is_zero_element(value)));
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__group__integral)
{
    const auto& in = signed_vectors();
    accessor::jacobian_t<uint64_t> sum{};
    accessor::to_jacobian(sum, in.points[0]);
    report("group double", microseconds(operations, [&](size_t) NOEXCEPT
    {
        accessor::double_(sum, sum);
        return true;
    }));

    report("group add", microseconds(operations, [&](size_t) NOEXCEPT
    {
        accessor::jacobian_t<uint64_t> out{};
        const auto faults = accessor::add(out, sum, in.points[1]);
        sum = out;
        return !f::any(faults);
    }));

    BOOST_CHECK(!f::any(sum.infinity));
}

// verify
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify_ecdsa__libsecp256k1)
{
    const auto& in = signed_vectors();
    report("ecdsa libsecp256k1", microseconds(count, [&](size_t index) NOEXCEPT
    {
        return ecdsa::verify_signature(in.keys[index], in.hashes[index], in.ecdsas[index]);
    }));
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify_ecdsa__local)
{
    const auto& in = signed_vectors();
    report("ecdsa local", microseconds(count, [&](size_t index) NOEXCEPT
    {
        const auto& signature = in.canonicals[index];
        const auto& r = array_cast<uint8_t, ec_secret_size>(signature);
        const auto& s = array_cast<uint8_t, ec_secret_size, ec_secret_size>(signature);
        affine point{};
        return accessor::from_bytes(point, in.keys[index]) && accessor::verify_ecdsa(point, in.hashes[index], r, s);
    }));
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify_schnorr__libsecp256k1)
{
    const auto& in = signed_vectors();
    report("schnorr libsecp256k1", microseconds(count, [&](size_t index) NOEXCEPT
    {
        return schnorr::verify_signature(in.xonlys[index], in.hashes[index], in.schnorrs[index]);
    }));
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify_schnorr__local)
{
    const auto& in = signed_vectors();
    report("schnorr local", microseconds(count, [&](size_t index) NOEXCEPT
    {
        const auto& signature = in.schnorrs[index];
        const auto& key = in.xonlys[index];
        const auto& r = array_cast<uint8_t, ec_secret_size>(signature);
        const auto& s = array_cast<uint8_t, ec_secret_size, ec_secret_size>(signature);
        return accessor::verify_schnorr(key, challenge(signature, key, in.hashes[index]), r, s);
    }));
}

// multiply
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_performance__multiply__integral)
{
    const auto& in = signed_vectors();
    report("multiply integral", microseconds(count, [&](size_t index) NOEXCEPT
    {
        accessor::jacobian_t<uint64_t> out{};
        return !f::any(accessor::multiply(out, accessor::scalars_t<uint64_t>{ left }, in.points[index], accessor::scalars_t<uint64_t>{ right }));
    }));
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__multiply__lanes)
{
    report_lanes<xint128_t>("multiply 2 lanes per signature");
    report_lanes<xint256_t>("multiply 4 lanes per signature");
    report_lanes<xint512_t>("multiply 8 lanes per signature");
}

// batch
// ----------------------------------------------------------------------------

template <typename Word>
static void report_batch(const std::string& name) NOEXCEPT
{
    const auto& in = signed_vectors();
    data_chunk results{};
    const auto ecdsa = microseconds(one, [&](size_t) NOEXCEPT
    {
        return accessor::verify_ecdsa<Word>(results, in.keys, in.hashes, in.canonicals);
    });

    const auto schnorr = microseconds(one, [&](size_t) NOEXCEPT
    {
        return accessor::verify_schnorr<Word>(results, in.xonlys, challenges(in), in.schnorrs);
    });

    report("ecdsa batch " + name + " per signature", ecdsa / count);
    report("schnorr batch " + name + " per signature", schnorr / count);
}

template <typename Word>
static void report_batches(const std::string& name) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
        report_batch<Word>(name);
    else if constexpr (have<Word>)
        report_batch<Word>(name);
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify_schnorr__combined)
{
    const auto& in = signed_vectors();
    const auto time = microseconds(one, [&](size_t) NOEXCEPT
    {
        return accessor::verify_schnorr(in.xonlys, challenges(in), in.schnorrs);
    });

    report("schnorr combined per signature", time / count);
}

BOOST_AUTO_TEST_CASE(secp256k1_performance__verify__batch)
{
    report_batches<uint64_t>("integral");
    report_batches<xint128_t>("2 lanes");
    report_batches<xint256_t>("4 lanes");
    report_batches<xint512_t>("8 lanes");
}

BOOST_AUTO_TEST_SUITE_END()

#endif
