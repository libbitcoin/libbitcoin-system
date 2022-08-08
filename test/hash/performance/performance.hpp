/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <chrono>

namespace performance {
    
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
    static constexpr bool ripemd{};     // false for sha algorithm.
    static constexpr bool compressed{}; // intrinsic sha (ignored for rmd).
    static constexpr bool vectorized{}; // algorithm vectorization.
    static constexpr bool concurrent{}; // algorithm concurrency.
    static constexpr bool chunked{};    // false for array data.
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

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    out << "test____________: " << TEST_NAME
        << delimiter
        << "algorithm_______: " << algorithm
        << delimiter
        << "test_rounds_____: " << serialize(Count)
        << delimiter
        << "bytes_per_round_: " << serialize(Size)
        << delimiter
        << "compressed______: " << serialize(P::compressed)
        << delimiter
        << "vectorized______: " << serialize(P::vectorized)
        << delimiter
        << "concurrent______: " << serialize(P::concurrent)
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
VCONSTEXPR auto get_data(uint64_t seed) noexcept
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
template <size_t Strength, bool Vectorized, bool Concurrent>
using rmd_algorithm = rmd::algorithm<
    iif<Strength == 160, rmd::h160<>, rmd::h128<>>, Vectorized, Concurrent>;

static_assert(is_same_type<rmd_algorithm<128, true, false>, rmd128>);
static_assert(is_same_type<rmd_algorithm<160, true, false>, rmd160>);

template <size_t Strength, bool Compressed, bool Vectorized, bool Concurrent>
using sha_algorithm = sha::algorithm<
    iif<Strength == 256, sha::h256<>,
    iif<Strength == 512, sha::h512<>, sha::h160>>, Compressed, Vectorized, Concurrent>;

static_assert(is_same_type<sha_algorithm<160, true, true, false>, sha160>);
static_assert(is_same_type<sha_algorithm<256, true, true, false>, sha256>);
static_assert(is_same_type<sha_algorithm<512, true, true, false>, sha512>);

template <size_t Strength, bool Compressed, bool Vectorized, bool Concurrent, bool Ripemd,
    bool_if<
       (!Ripemd && (Strength == 160 || Strength == 256 || Strength == 512)) ||
        (Ripemd && (Strength == 128 || Strength == 160))> = true>
using hash_selector = iif<Ripemd,
    rmd_algorithm<Strength, Vectorized, Concurrent>,
    sha_algorithm<Strength, Compressed, Vectorized, Concurrent>>;

static_assert(is_same_type<hash_selector<128, true, true, false, true>, rmd128>);
static_assert(is_same_type<hash_selector<160, true, true, false, true>, rmd160>);
static_assert(is_same_type<hash_selector<160, true, true, false, false>, sha160>);
static_assert(is_same_type<hash_selector<256, true, true, false, false>, sha256>);
static_assert(is_same_type<hash_selector<512, true, true, false, false>, sha512>);

// RMD does not provide compression.
////static_assert(hash_selector< 128, true,  true, true, true >::compressed);
////static_assert(hash_selector< 160, true,  true, true, true >::compressed);
static_assert(hash_selector< 160, true,  true, true, false>::compressed);
static_assert(hash_selector< 256, true,  true, true, false>::compressed);
static_assert(hash_selector< 512, true,  true, true, false>::compressed);
////static_assert(!hash_selector<128, false, true, true, true >::compressed);
////static_assert(!hash_selector<160, false, true, true, true >::compressed);
static_assert(!hash_selector<160, false, true, true, false>::compressed);
static_assert(!hash_selector<256, false, true, true, false>::compressed);
static_assert(!hash_selector<512, false, true, true, false>::compressed);

static_assert(hash_selector< 128, true, true,  true, true >::vectorized);
static_assert(hash_selector< 160, true, true,  true, true >::vectorized);
static_assert(hash_selector< 160, true, true,  true, false>::vectorized);
static_assert(hash_selector< 256, true, true,  true, false>::vectorized);
static_assert(hash_selector< 512, true, true,  true, false>::vectorized);
static_assert(!hash_selector<128, true, false, true, true >::vectorized);
static_assert(!hash_selector<160, true, false, true, true >::vectorized);
static_assert(!hash_selector<160, true, false, true, false>::vectorized);
static_assert(!hash_selector<256, true, false, true, false>::vectorized);
static_assert(!hash_selector<512, true, false, true, false>::vectorized);

static_assert(hash_selector< 128, true, true, true,  true >::concurrent);
static_assert(hash_selector< 160, true, true, true,  true >::concurrent);
static_assert(hash_selector< 160, true, true, true,  false>::concurrent);
static_assert(hash_selector< 256, true, true, true,  false>::concurrent);
static_assert(hash_selector< 512, true, true, true,  false>::concurrent);
static_assert(!hash_selector<128, true, true, false, true >::concurrent);
static_assert(!hash_selector<160, true, true, false, true >::concurrent);
static_assert(!hash_selector<160, true, true, false, false>::concurrent);
static_assert(!hash_selector<256, true, true, false, false>::concurrent);
static_assert(!hash_selector<512, true, true, false, false>::concurrent);

template <typename Parameters>
using algorithm = hash_selector<
    Parameters::strength,
    Parameters::compressed,
    Parameters::vectorized,
    Parameters::concurrent,
    Parameters::ripemd>;

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
bool hash(std::ostream& out, float ghz = 3.0f, bool csv = false) noexcept
{
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = algorithm<Parameters>;
    using P = Parameters;

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

// Algorithm::hash() test runner parameterization.
// ----------------------------------------------------------------------------

template <bool Compressed, bool Vectorized, bool Concurrent, bool Chunked>
struct sha256_parameters : parameters
{
    static constexpr size_t strength{ 256 };
    static constexpr bool ripemd{ false };
    static constexpr bool compressed{ Compressed };
    static constexpr bool vectorized{ Vectorized };
    static constexpr bool concurrent{ Concurrent };
    static constexpr bool chunked{ Chunked };
};

template <bool Compressed, bool Vectorized, bool Concurrent, bool Chunked>
struct rmd160_parameters : parameters
{
    static constexpr size_t strength{ 160 };
    static constexpr bool ripemd{ false };
    static constexpr bool compressed{ Compressed };
    static constexpr bool vectorized{ Vectorized };
    static constexpr bool concurrent{ Concurrent };
    static constexpr bool chunked{ Chunked };
};


using sha256_optimal = sha256_parameters<true, true, false, false>;
using rmd160_optimal = rmd160_parameters<true, true, false, false>;

} // namespace performance

#endif
