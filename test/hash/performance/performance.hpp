/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include "baseline/rmd160.h"
#include "baseline/sha256.h"
#include <chrono>

namespace performance {

constexpr bool use_csv = false;
    
// format timing results to ostream
// ----------------------------------------------------------------------------

template <typename Precision>
constexpr auto seconds_total(uint64_t time) noexcept
{
    return (1.0f * time) / Precision::period::den;
}

template <size_t Count>
constexpr auto ms_per_round(float seconds) noexcept
{
    return (seconds * std::milli::den) / Count;
}

template <size_t Bytes>
constexpr auto ms_per_byte(float seconds) noexcept
{
    return (seconds * std::milli::den) / Bytes;
}

template <size_t Bytes>
constexpr auto mib_per_second(float seconds) noexcept
{
    return Bytes / seconds / power2(20u);
}

template <size_t Bytes>
constexpr auto cycles_per_byte(float seconds, float ghz) noexcept
{
    return (seconds * ghz * std::giga::num) / Bytes;
}

struct parameters
{
    static constexpr size_t strength{}; // algorithm strength (160/256/512|128/160).
    static constexpr bool native{};     // intrinsic sha (ignored for rmd).
    static constexpr bool vector{};     // algorithm vectorization.
    static constexpr bool cached{};     // scheduled pad caching.
    static constexpr bool chunked{};    // false for array data.
    static constexpr bool ripemd{};     // false for sha algorithm.
};

// Output performance run to given stream.
template <
    typename Parameters,
    size_t Count,
    size_t Size,
    typename Algorithm,
    typename Precision>
void output(std::ostream& out, uint64_t time, float ghz, bool csv) noexcept
{
    using P = Parameters;
    constexpr auto bytes = Size * Count;
    const auto seconds = seconds_total<Precision>(time);
    const auto delimiter = csv ? "," : "\n";
    std::string algorithm{ typeid(Algorithm).name() };
    replace(algorithm, "libbitcoin::system::", "");
    replace(algorithm, "class ", "");
    replace(algorithm, "struct ", "");

    // vector/native parameters of no effect if features not supported.
    // Algorithm actual run is reflected in the type.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    out << delimiter
        << "test____________: " << TEST_NAME
        << delimiter
        << "algorithm_______: " << algorithm
        << delimiter
        << "test_rounds_____: " << serialize(Count)
        << delimiter
        << "bytes_per_round_: " << serialize(Size)
        << delimiter
        << "native__________: " << serialize(P::native)
        << delimiter
        << "vectorized______: " << serialize(P::vector)
        << delimiter
        << "cached__________: " << serialize(P::cached)
        << delimiter
        << "chunked_________: " << serialize(P::chunked)
        << delimiter
        << "seconds_total___: " << serialize(seconds)
        << delimiter
        << "mib_per_second__: " << serialize(mib_per_second<bytes>(seconds))
        << delimiter
        << "cycles_per_byte_: " << serialize(cycles_per_byte<bytes>(seconds, ghz))
        << delimiter
        << "ms_per_round____: " << serialize(ms_per_round<Count>(seconds))
        << delimiter
        << "ms_per_byte_____: " << serialize(ms_per_byte<bytes>(seconds))
        << delimiter;
    BC_POP_WARNING()
}

// generate deterministic data from seed
// ----------------------------------------------------------------------------
// Generate a data_chunk or a data_array of specified size.
// Each is hashed on the seed to preclude compiler/CPU optimization.

template <size_t Size, bool Chunk = false>
auto get_data(size_t seed) noexcept
{
    constexpr auto filler = [](auto seed, auto& data)
    {
        std::for_each(data.begin(), data.end(), [&](auto& byte)
        {
            byte = narrow_cast<uint8_t>((seed = hash_combine(42u, seed)));
        });
    };

    if constexpr (Chunk)
    {
        const auto data = std::make_shared<data_chunk>(Size);
        filler(seed, *data);
        return data;
    }
    else
    {
        const auto data = std::make_shared<data_array<Size>>();
        filler(seed, *data);
        return data;
    }
}

// timer utility
// ----------------------------------------------------------------------------

template <typename Time = std::chrono::nanoseconds,
    class Clock = std::chrono::system_clock>
class timer
{
public:
    /// Returns the duration (in chrono's type system) of the elapsed time.
    template <typename Function, typename ...Args>
    static Time duration(const Function& func, Args&&... args) noexcept
    {
        const auto start = Clock::now();
        
        func(std::forward<Args>(args)...);
        return std::chrono::duration_cast<Time>(Clock::now() - start);
    }

    /// Returns the quantity (count) of the elapsed time as TimeT units.
    template <typename Function, typename ...Args>
    static typename Time::rep execution(const Function& func,
        Args&&... args) noexcept
    {
        return duration(func, std::forward<Args>(args)...).count();
    }
};

// hash selector
// ----------------------------------------------------------------------------

#if !defined(VISIBILE)
template <size_t Strength>
using rmd_algorithm = rmd::algorithm<
    iif<Strength == 160, rmd::h160<>, rmd::h128<>>>;

static_assert(is_same_type<rmd_algorithm<128>, rmd128>);
static_assert(is_same_type<rmd_algorithm<160>, rmd160>);

template <size_t Strength, bool Native, bool Vector, bool Cached>
using sha_algorithm = sha::algorithm<
    iif<Strength == 256, sha::h256<>,
    iif<Strength == 512, sha::h512<>, sha::h160>>, Native, Vector, Cached>;

////static_assert(is_same_type<sha_algorithm<160, true, true, true>, sha160>);
////static_assert(is_same_type<sha_algorithm<256, true, true, true>, sha256>);
////static_assert(is_same_type<sha_algorithm<512, true, true, true>, sha512>);

template <size_t Strength, bool Native, bool Vector, bool Cached, bool Ripemd,
    bool_if<
       (!Ripemd && (Strength == 160 || Strength == 256 || Strength == 512)) ||
        (Ripemd && (Strength == 128 || Strength == 160))> = true>
using hash_selector = iif<Ripemd, rmd_algorithm<Strength>,
    sha_algorithm<Strength, Native, Vector, Cached>>;

////static_assert(is_same_type<hash_selector<128, true, true, false,  true>, rmd128>);
////static_assert(is_same_type<hash_selector<160, true, true, false,  true>, rmd160>);
////static_assert(is_same_type<hash_selector<160, true, true, true, false>, sha160>);
////static_assert(is_same_type<hash_selector<256, true, true, true, false>, sha256>);
////static_assert(is_same_type<hash_selector<512, true, true, true, false>, sha512>);

static_assert(hash_selector< 160, true,  true, true, false>::native == with_shani || with_neon);
static_assert(hash_selector< 256, true,  true, true, false>::native == with_shani || with_neon);
static_assert(hash_selector< 512, true,  true, true, false>::native == /*with_shani || with_neon*/ false);
static_assert(!hash_selector<160, false, true, true, false>::native);
static_assert(!hash_selector<256, false, true, true, false>::native);
static_assert(!hash_selector<512, false, true, true, false>::native);

static_assert(hash_selector< 160, true, true,  true, false>::vector == with_sse41 || with_avx2 || with_avx512);
static_assert(hash_selector< 256, true, true,  true, false>::vector == with_sse41 || with_avx2 || with_avx512);
static_assert(hash_selector< 512, true, true,  true, false>::vector == with_sse41 || with_avx2 || with_avx512);
static_assert(!hash_selector<160, true, false, true, false>::vector);
static_assert(!hash_selector<256, true, false, true, false>::vector);
static_assert(!hash_selector<512, true, false, true, false>::vector);

static_assert(hash_selector< 160, true, true, true,  false>::caching);
static_assert(hash_selector< 256, true, true, true,  false>::caching);
static_assert(hash_selector< 512, true, true, true,  false>::caching);
static_assert(!hash_selector<160, true, true, false, false>::caching);
static_assert(!hash_selector<256, true, true, false, false>::caching);
static_assert(!hash_selector<512, true, true, false, false>::caching);

#endif

// Algorithm::hash() test runner.
// ----------------------------------------------------------------------------
// hash_digest/hash_chunk overloads are not exposed, only check and array.
// There is no material performance difference between slice and chunk. The
// meaningful performance distinction is between array and non-array, since
// array size is resolved at compile time, allowing for various optimizations.

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 1024,         // bytes per iteration (1KiB)
    if_base_of<parameters, Parameters> = true>
bool test_accumulator(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            accumulator<Algorithm>::hash(*data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

template<typename Parameters,
    size_t Count = 1024 * 1024,
    size_t Size = 1024,
    bool_if<Size == 32 || Size == 64> = true,
    bool_if<!Parameters::chunked> = true,
    if_base_of<parameters, Parameters> = true>
bool test_algorithm(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            Algorithm::hash(*data);
        });
    }

    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

template<typename Parameters,
    size_t Count = 1024 * 1024,
    size_t Size = 1024,
    bool_if<Size == 32 || Size == 64> = true,
    bool_if<!Parameters::chunked> = true,
    if_base_of<parameters, Parameters> = true>
bool test_algorithm_pair(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            Algorithm::hash(*data, *data);
        });
    }

    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

template<typename Parameters,
    size_t Count = 1024 * 1024,
    size_t Size = 1024,
    bool_if<Size == 32 || Size == 64> = true,
    bool_if<!Parameters::chunked> = true,
    if_base_of<parameters, Parameters> = true>
bool test_algorithm_pair_double(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            Algorithm::double_hash(*data, *data);
        });
    }

    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

template<typename Parameters,
    size_t Count = 1024 * 1024,
    size_t Size = 1024,
    bool_if<Size == 32 || Size == 64 || Size == 128> = true,
    bool_if<!Parameters::chunked> = true,
    if_base_of<parameters, Parameters> = true>
bool test_algorithm_double(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            Algorithm::double_hash(*data);
        });
    }

    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

template<typename Parameters,
    size_t Count = 1024 * 1024,
    size_t Size = 1024, // count of blocks (digests / 2)
    bool_if<!Parameters::chunked && !Parameters::ripemd> = true,
    if_base_of<parameters, Parameters> = true>
bool test_merkle(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = hash_selector<
        P::strength,
        P::native,
        P::vector,
        P::cached,
        P::ripemd>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        constexpr auto size = array_count<typename Algorithm::digest_t>;
        std::vector<typename Algorithm::digest_t> digests{};
        digests.reserve(Size * two);

        for (size_t blocks = 0; blocks < Size; ++blocks)
        {
            digests.push_back(*get_data<size, false>(blocks + seed));
            digests.push_back(*get_data<size, false>(blocks + add1(seed)));
        }

        time += Timer::execution([&]() noexcept
        {
            Algorithm::merkle_hash(digests);
        });
    }

    output<Parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

// Algorithm::hash() test runner parameterization.
// ----------------------------------------------------------------------------

template <bool Native, bool Vector, bool Cached, bool Chunked>
struct sha256_parameters : parameters
{
    static constexpr size_t strength{ 256 };
    static constexpr bool native{ Native };
    static constexpr bool vector{ Vector };
    static constexpr bool cached{ Cached };
    static constexpr bool chunked{ Chunked };
    static constexpr bool ripemd{};
};

template <bool Chunked>
struct rmd160_parameters : parameters
{
    static constexpr size_t strength{ 160 };
    static constexpr bool native{};
    static constexpr bool vector{};
    static constexpr bool cached{};
    static constexpr bool chunked{ Chunked };
    static constexpr bool ripemd{ true };
};

// baseline for performance tests
// ----------------------------------------------------------------------------
// The satoshi client hash implementation is the performance test baseline.
// All script hashing is performed through the accumulator, which always
// buffers a remainder and/or padding. There is no optimization for full or
// half block sizes. All data cycles through the stream writer objects.

////case OP_RIPEMD160:
////case OP_SHA1:
////case OP_SHA256:
////case OP_HASH160:
////case OP_HASH256:
////{
////    if (stack.size() < 1)
////        return set_error(serror, SCRIPT_ERR_INVALID_STACK_OPERATION);
////    valtype& vch = stacktop(-1);
////    valtype vchHash((opcode == OP_RIPEMD160 || opcode == OP_SHA1 || opcode == OP_HASH160) ? 20 : 32);
////    if (opcode == OP_RIPEMD160)
////        CRIPEMD160().Write(vch.data(), vch.size()).Finalize(vchHash.data());
////    else if (opcode == OP_SHA1)
////        CSHA1().Write(vch.data(), vch.size()).Finalize(vchHash.data());
////    else if (opcode == OP_SHA256)
////        CSHA256().Write(vch.data(), vch.size()).Finalize(vchHash.data());
////    else if (opcode == OP_HASH160)
////        CHash160().Write(vch).Finalize(vchHash);
////    else if (opcode == OP_HASH256)
////        CHash256().Write(vch).Finalize(vchHash);
////    popstack(stack);
////    stack.push_back(vchHash);
////}
////break;

// Baseline test runner helper.
// ----------------------------------------------------------------------------

// Equivalent to:
// hash_digest accumulator<sha256>::hash(data);
// short_hash accumulator<rmd160>::hash(data);
template <typename Algorithm>
inline typename Algorithm::digest_t base_merkle(
    std::vector<typename Algorithm::digest_t>& data) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    const auto bytes = system::pointer_cast<uint8_t>(data.data());
    baseline::SHA256D64(bytes, bytes, to_half(data.size()));
    return system::unsafe_array_cast<uint8_t, Algorithm::digest_bytes>(bytes);
    BC_POP_WARNING()
}

// Equivalent to:
// hash_digest accumulator<sha256>::hash(data);
// short_hash accumulator<rmd160>::hash(data);
template <typename Algorithm>
inline auto base_accumulator(auto& data) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std_array<uint8_t, Algorithm::digest_bytes> out{};
    Algorithm hasher{};
    hasher.Write(data.data(), data.size());
    hasher.Finalize(out.data());
    return out;
    BC_POP_WARNING()
}

// Equivalent to:
// hash_digest accumulator<sha256>::hash(left, left);
template <typename Algorithm>
inline auto base_accumulator(const auto& left,
    const auto& right) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std_array<uint8_t, Algorithm::digest_bytes> out{};
    Algorithm hasher{};
    hasher.Write(left.data(), left.size());
    hasher.Write(right.data(), right.size());
    hasher.Finalize(out.data());
    return out;
    BC_POP_WARNING()
}

// Equivalent to:
// hash_digest accumulator<sha256>::double_hash(left, left);
template <typename Algorithm>
inline auto base_accumulator_double(const auto& left,
    const auto& right) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std_array<uint8_t, Algorithm::digest_bytes> out{};
    Algorithm hasher{};
    hasher.Write(left.data(), left.size());
    hasher.Write(right.data(), right.size());
    hasher.Finalize(out.data());
    hasher.Reset();
    hasher.Write(out.data(), out.size());
    hasher.Finalize(out.data());
    return out;
    BC_POP_WARNING()
}

// Equivalent to:
// hash_digest accumulator<sha256>::double_hash(data);
template <typename Algorithm>
inline auto base_accumulator_double(const auto& data) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std_array<uint8_t, Algorithm::digest_bytes> out{};
    Algorithm hasher{};
    hasher.Write(data.data(), data.size());
    hasher.Finalize(out.data());
    hasher.Reset();
    hasher.Write(out.data(), out.size());
    hasher.Finalize(out.data());
    return out;
    BC_POP_WARNING()
}

// baseline test runner.
// ----------------------------------------------------------------------------

namespace base {
    
template <typename Algorithm, bool Chunked>
struct parameters
{
    // baseline::CSHA256 or baseline::CRIPEMD160
    using algorithm = Algorithm;
    static constexpr bool chunked{ Chunked };
};

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 1024>         // bytes per iteration (1KiB)
bool test_hash(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using algorithm = typename P::algorithm;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using params = iif<is_same_type<algorithm, baseline::CSHA256>,
        sha256_parameters<false, false, false, P::chunked>,
        rmd160_parameters<P::chunked>>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            base_accumulator<algorithm>(*data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<params, Count, Size, algorithm, Precision>(out, time, ghz, csv);
    return true;
}

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 32>           // bytes per iteration
bool test_hash_pair(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using algorithm = typename P::algorithm;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using params = iif<is_same_type<algorithm, baseline::CSHA256>,
        sha256_parameters<false, false, false, P::chunked>,
        rmd160_parameters<P::chunked>>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            base_accumulator<algorithm>(*data, *data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<params, Count, Size, algorithm, Precision>(out, time, ghz, csv);
    return true;
}

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 32>           // bytes per iteration
bool test_hash_pair_double(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using algorithm = typename P::algorithm;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using params = iif<is_same_type<algorithm, baseline::CSHA256>,
        sha256_parameters<false, false, false, P::chunked>,
        rmd160_parameters<P::chunked>>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            base_accumulator_double<algorithm>(*data, *data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<params, Count, Size, algorithm, Precision>(out, time, ghz, csv);
    return true;
}

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 32>           // bytes per iteration
bool test_hash_double(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using algorithm = typename P::algorithm;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using params = iif<is_same_type<algorithm, baseline::CSHA256>,
        sha256_parameters<false, false, false, P::chunked>,
        rmd160_parameters<P::chunked>>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            base_accumulator_double<algorithm>(*data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<params, Count, Size, algorithm, Precision>(out, time, ghz, csv);
    return true;
}

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 1024>         // number of blocks
bool test_merkle(std::ostream& out, bool csv = use_csv,
    float ghz = 3.0f) noexcept
{
    using P = Parameters;
    using algorithm = typename P::algorithm;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using params = iif<is_same_type<algorithm, baseline::CSHA256>,
        sha256_parameters<false, false, false, P::chunked>,
        rmd160_parameters<P::chunked>>;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        constexpr auto size = array_count<typename algorithm::digest_t>;
        std::vector<typename algorithm::digest_t> digests{};
        digests.reserve(Size * two);

        for (size_t blocks = 0; blocks < Size; ++blocks)
        {
            digests.push_back(*get_data<size, false>(blocks + seed));
            digests.push_back(*get_data<size, false>(blocks + add1(seed)));
        }

        time += Timer::execution([&]() noexcept
        {
            base_merkle<algorithm>(digests);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<params, Count, Size, algorithm, Precision>(out, time, ghz, csv);
    return true;
}

} // namespace base
} // namespace performance

#endif
