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
#include "../../../test.hpp"

#if defined(HAVE_PERFORMANCE_TESTS)

#include <chrono>

BOOST_AUTO_TEST_SUITE(secp256k1_batch_performance_tests)

// Set CUDA_VISIBLE_DEVICES to empty to measure the processor path.

constexpr size_t repeats = 5;
constexpr size_t signers = 4096;

// helpers
// ----------------------------------------------------------------------------

struct rows
{
    std_vector<ecdsa::batch::correlate_t> ecdsa_correlates{};
    std_vector<schnorr::batch::correlate_t> schnorr_correlates{};
    std_vector<ecdsa::batch::row_t> ecdsa_rows{};
    std_vector<schnorr::batch::row_t> schnorr_rows{};
    hashes digests{};
    ec_compresseds keys{};
    ec_xonlys xonlys{};
    ec_signatures ecdsas{};
    ec_signatures schnorrs{};
};

static batched::link link_of(size_t row) NOEXCEPT
{
    return
    {
        narrow_cast<uint8_t>(row),
        narrow_cast<uint8_t>(row >> 8),
        narrow_cast<uint8_t>(row >> 16)
    };
}

// Rows of distinct signers, repeated to fill the batch.
static rows signed_rows(size_t count) NOEXCEPT
{
    static const auto signed_ = []() NOEXCEPT
    {
        rows out{};
        for (size_t index{}; index < signers; ++index)
        {
            const auto secret = sha256_hash(to_chunk(std::to_string(index)));
            const auto hash = sha256_hash(secret);
            ec_compressed key{};
            ec_signature ecdsa{}, schnorr{};
            secret_to_public(key, secret);
            ecdsa::sign(ecdsa, secret, hash);
            schnorr::sign(schnorr, secret, hash, hash);
            out.digests.push_back(hash);
            out.keys.push_back(key);
            out.xonlys.push_back(array_cast<uint8_t, ec_xonly_size, one>(key));
            out.ecdsas.push_back(ecdsa);
            out.schnorrs.push_back(schnorr);
        }

        return out;
    }();

    rows out{};
    for (size_t row{}; row < count; ++row)
    {
        const auto index = row % signers;
        out.ecdsa_correlates.push_back({ link_of(row), 0, 0 });
        out.schnorr_correlates.push_back({ link_of(row) });
        out.ecdsa_rows.push_back(
        {
            signed_.digests[index],
            signed_.keys[index],
            signed_.ecdsas[index]
        });
        out.schnorr_rows.push_back(
        {
            signed_.digests[index],
            signed_.xonlys[index],
            signed_.schnorrs[index]
        });
    }

    return out;
}

// Median seconds per call of function(), which is expected to be true.
template <typename Function>
static double seconds(Function&& function) NOEXCEPT
{
    using duration = std::chrono::duration<double>;
    std_array<double, repeats> times{};
    for (auto& time: times)
    {
        const auto start = std::chrono::steady_clock::now();
        BOOST_CHECK(function());
        time = duration(std::chrono::steady_clock::now() - start).count();
    }

    std::sort(times.begin(), times.end());
    return times[to_half(repeats)];
}

static void report(const std::string& name, size_t count,
    double time) NOEXCEPT
{
    std::cout << name << " " << count << " rows: "
        << (count / time) << " signatures per second" << std::endl;
}

static void report_batches(size_t count) NOEXCEPT
{
    const auto in = signed_rows(count);
    const ecdsa::batch ecdsa_rows
    {
        { in.ecdsa_correlates.data(), count },
        { in.ecdsa_rows.data(), count }
    };

    const schnorr::batch schnorr_rows
    {
        { in.schnorr_correlates.data(), count },
        { in.schnorr_rows.data(), count }
    };

    const stopper cancel{};
    report("ecdsa", count, seconds([&]() NOEXCEPT
    {
        return ecdsa::batch::verify(cancel, ecdsa_rows).empty();
    }));

    report("schnorr", count, seconds([&]() NOEXCEPT
    {
        return schnorr::batch::verify(cancel, schnorr_rows).empty();
    }));
}

// verify
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_batch_performance__accelerated__reported)
{
    std::cout << "compiled: " << batched::compiled() << ", accelerated: " << batched::accelerated() << std::endl;
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_performance__verify__1k)
{
    report_batches(1'024);
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_performance__verify__64k)
{
    report_batches(65'536);
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_performance__verify__1m)
{
    report_batches(1'048'576);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
