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
#ifndef LIBBITCOIN_SYSTEM_TEST_HASH_PERFORMANCE_PERFORMANCE_HPP
#define LIBBITCOIN_SYSTEM_TEST_HASH_PERFORMANCE_PERFORMANCE_HPP

#include "../../test.hpp"

#if defined(HAVE_PERFORMANCE_TESTS)

#include <algorithm>
#include <chrono>

namespace performance {

template <typename SHA, bool Native, bool Vector, bool Cached = true>
using sha_t = sha::algorithm<SHA, Native, Vector, Cached>;

template <bool Native, bool Vector, bool Cached = true>
using sha256_t = sha_t<sha::h256<>, Native, Vector, Cached>;

using sha256_scalar = sha256_t<false, false>;
using sha256_vector = sha256_t<false, true>;
using sha256_native = sha256_t<true, false>;
using sha256_both = sha256_t<true, true>;
using sha256_uncached = sha256_t<false, false, false>;

class accessor
  : public sha256_both
{
public:
    using iblocks_t = sha256_both::iblocks_t;
    using idigests_t = sha256_both::idigests_t;
    using sha256_both::merkle_hash_vector;
    using sha256_both::merkle_hash_native;
};

constexpr size_t repeats = 5;
constexpr size_t stream_bytes = 1'000'000;
constexpr size_t merkle_blocks = 1024;
constexpr size_t merkle_leaves = 9001;

// timing
// ----------------------------------------------------------------------------

using duration = std::chrono::duration<double, std::nano>;

// Median nanoseconds per call of function() over repeats of calls.
template <typename Function>
double nanoseconds(size_t calls, Function&& function) NOEXCEPT
{
    std_array<double, repeats> times{};
    for (auto& time: times)
    {
        const auto start = std::chrono::steady_clock::now();
        for (size_t call{}; call < calls; ++call)
            function();

        const auto stop = std::chrono::steady_clock::now();
        time = duration(stop - start).count() / calls;
    }

    std::sort(times.begin(), times.end());
    return times[to_half(repeats)];
}

inline void report(const std::string& name, double time,
    const std::string& unit) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::cout << name << ": " << time << " ns/" << unit << std::endl;
    BC_POP_WARNING()
}

// data
// ----------------------------------------------------------------------------

inline data_chunk get_bytes(size_t size) NOEXCEPT
{
    data_chunk out(size);
    for (size_t index{}; index < size; ++index)
        out[index] = narrow_cast<uint8_t>(hash_combine(42u, index));

    return out;
}

inline hashes get_digests(size_t count) NOEXCEPT
{
    hashes out{};
    out.reserve(count);
    for (size_t index{}; index < count; ++index)
        out.push_back(sha256_hash(to_chunk(std::to_string(index))));

    return out;
}

// workloads
// ----------------------------------------------------------------------------
// Each call feeds its result into the next call's input, so no call can be
// elided and each workload's final state identifies the computation.

// Streamed hash of 1,000,000 bytes, digest written over the leading bytes.
template <typename Algorithm>
struct stream_1m
{
    static data_chunk run(const std::string& name, size_t calls) NOEXCEPT
    {
        auto data = get_bytes(stream_bytes);
        report(name, nanoseconds(calls, [&]() NOEXCEPT
        {
            const auto digest = accumulator<Algorithm>::hash(data);
            std::copy(digest.begin(), digest.end(), data.begin());
        }) / stream_bytes, "byte");

        return data;
    }
};

// Streamed hash of 32 bytes, digest replaces the input.
template <typename Algorithm>
struct stream_32
{
    static hash_digest run(const std::string& name, size_t calls) NOEXCEPT
    {
        hash_digest data{};
        report(name, nanoseconds(calls, [&]() NOEXCEPT
        {
            data = accumulator<Algorithm>::hash(data);
        }) / hash_size, "byte");

        return data;
    }
};

// Fixed size hash of 32 bytes, digest replaces the input.
template <typename Algorithm>
struct fixed_32
{
    static hash_digest run(const std::string& name, size_t calls) NOEXCEPT
    {
        hash_digest data{};
        report(name, nanoseconds(calls, [&]() NOEXCEPT
        {
            data = Algorithm::hash(data);
        }) / hash_size, "byte");

        return data;
    }
};

// Merkle root of 9001 leaves, root written over the first leaf.
template <typename Algorithm>
struct root_9001
{
    static hash_digest run(const std::string& name, size_t calls) NOEXCEPT
    {
        auto leaves = get_digests(merkle_leaves);
        report(name, nanoseconds(calls, [&]() NOEXCEPT
        {
            hashes copy{};
            copy.reserve(add1(leaves.size()));
            copy.assign(leaves.begin(), leaves.end());
            leaves.front() = Algorithm::merkle_root(std::move(copy));
        }) / merkle_leaves, "leaf");

        return leaves.front();
    }
};

// Double hash of 1024 64 byte blocks, digests written over the leading bytes.
template <typename Algorithm>
hashes double_64(const std::string& name, size_t calls) NOEXCEPT
{
    constexpr auto size = two * merkle_blocks * hash_size;
    auto data = get_digests(two * merkle_blocks);
    report(name, nanoseconds(calls, [&]() NOEXCEPT
    {
        for (size_t block{}; block < merkle_blocks; ++block)
        {
            const auto& left = data[two * block];
            const auto& right = data[add1(two * block)];
            data[block] = Algorithm::double_hash(left, right);
        }
    }) / size, "byte");

    return data;
}

// As double_64, with all blocks hashed in xWord lanes.
template <typename xWord>
hashes double_64_lanes(const std::string& name, size_t calls) NOEXCEPT
{
    constexpr auto size = two * merkle_blocks * hash_size;
    auto data = get_digests(two * merkle_blocks);
    report(name, nanoseconds(calls, [&]() NOEXCEPT
    {
        const auto start = data.front().data();
        auto blocks = accessor::iblocks_t{ size, start };
        auto digests = accessor::idigests_t{ to_half(size), start };
        accessor::merkle_hash_vector<xWord>(digests, blocks);
    }) / size, "byte");

    return data;
}

// As double_64, with all blocks hashed in native pairs.
inline hashes double_64_native(const std::string& name, size_t calls) NOEXCEPT
{
    constexpr auto size = two * merkle_blocks * hash_size;
    auto data = get_digests(two * merkle_blocks);
    report(name, nanoseconds(calls, [&]() NOEXCEPT
    {
        const auto start = data.front().data();
        auto blocks = accessor::iblocks_t{ size, start };
        auto digests = accessor::idigests_t{ to_half(size), start };
        accessor::merkle_hash_native(digests, blocks);
    }) / size, "byte");

    return data;
}

// runners
// ----------------------------------------------------------------------------

// Run each available dispatch of Workload over SHA and check that all agree.
template <typename SHA, template <typename> class Workload>
void run_sha(const std::string& name, size_t calls) NOEXCEPT
{
    using scalar_t = sha_t<SHA, false, false>;
    using vector_t = sha_t<SHA, false, true>;
    using native_t = sha_t<SHA, true, false>;
    using both_t = sha_t<SHA, true, true>;

    const auto expected = Workload<scalar_t>::run(name + " scalar", calls);

    if constexpr (vector_t::vector)
    {
        const auto vector = Workload<vector_t>::run(name + " vector", calls);
        BOOST_CHECK_EQUAL(vector, expected);
    }

    if constexpr (native_t::native)
    {
        const auto native = Workload<native_t>::run(name + " native", calls);
        BOOST_CHECK_EQUAL(native, expected);
    }

    if constexpr (both_t::native && both_t::vector)
    {
        const auto both = Workload<both_t>::run(name + " native vector", calls);
        BOOST_CHECK_EQUAL(both, expected);
    }
}

// Run stream_1m over rmd160 and check it against the chain as whole blocks.
inline void run_rmd160(const std::string& name, size_t calls) NOEXCEPT
{
    static_assert(is_zero(stream_bytes % array_count<rmd160::block_t>));

    auto expected = get_bytes(stream_bytes);
    for (size_t call{}; call < repeats * calls; ++call)
    {
        rmd160::iblocks_t blocks{ expected.size(), expected.data() };
        const auto digest = rmd160::hash(std::move(blocks));
        std::copy(digest.begin(), digest.end(), expected.begin());
    }

    BOOST_CHECK_EQUAL(stream_1m<rmd160>::run(name, calls), expected);
}

template <typename xWord>
void run_double_64_lanes(const std::string& name, size_t calls,
    const hashes& expected) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        BOOST_CHECK_EQUAL(double_64_lanes<xWord>(name, calls), expected);
    }
}

} // namespace performance

#endif

#endif
