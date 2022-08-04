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
#ifndef LIBBITCOIN_SYSTEM_TEST_PERFORMANCE_PERFORMANCE_HPP
#define LIBBITCOIN_SYSTEM_TEST_PERFORMANCE_PERFORMANCE_HPP

#include "../test.hpp"
#include <chrono>

namespace perf {
    
// format timing results to ostream
// ----------------------------------------------------------------------------

// millirounds
template <size_t Count>
constexpr auto scale = (1.0f * Count) / std::milli::den;

// seconds
template <typename Precision>
constexpr auto total(uint64_t time) noexcept
{
    // Divide precision by seconds/precision => seconds.
    return (1.0f * time) / Precision::period::den;
}

// milliseconds
template <typename Precision>
constexpr auto round(uint64_t time, uint64_t rounds) noexcept
{
    return (total<Precision>(time) / rounds) * std::milli::den;
}

// milliseconds
template <typename Precision>
constexpr auto datum(uint64_t time, uint64_t rounds, size_t size) noexcept
{
    return round<Precision>(time, rounds) / size;
}

// Output performance run to given stream.
template
<
    typename Algorithm,
    typename Precision,
    size_t Size,
    size_t Count,
    bool Concurrent,
    bool Vectorized,
    bool Intrinsic,
    bool Chunked
>
void output(std::ostream& out, uint64_t time, bool csv) noexcept
{
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
        << "k_rounds________: " << serialize(scale<Count>)
        << delimiter
        << "bytes_per_round_: " << serialize(Size)
        << delimiter
        << "concurrent______: " << serialize(Concurrent)
        << delimiter
        << "vectorized______: " << serialize(Vectorized)
        << delimiter
        << "intrinsic_______: " << serialize(Intrinsic)
        << delimiter
        << "chunked_________: " << serialize(Chunked)
        << delimiter
        << "seconds_total___: " << serialize(total<Precision>(time))
        << delimiter
        << "ms_per_round____: " << serialize(round<Precision>(time, Count))
        << delimiter
        << "ms_per_byte_____: " << serialize(datum<Precision>(time, Count, Size))
        << delimiter;
    BC_POP_WARNING()
}

// generate deterministic data from seed
// ----------------------------------------------------------------------------

template <size_t Size, bool Chunk = false>
constexpr auto get_data(uint64_t seed) noexcept
{
    if constexpr (Chunk)
    {
        // minimally-sized chunk.
        auto chunk = to_big_endian_size(seed);
        chunk.resize(Size);
        return chunk;
    }
    else
    {
        // specified-size array.
        return to_big_endian_size<Size>(seed);
    }
}

// timer utility
// ----------------------------------------------------------------------------

template <typename Time = std::chrono::milliseconds,
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

template <size_t Strength, bool Concurrent = true>
using rmd_algorithm = rmd::algorithm<
    iif<Strength == 160, rmd::h160<>, rmd::h128<>>, Concurrent>;

static_assert(is_same_type<rmd_algorithm<128, true>, rmd128>);
static_assert(is_same_type<rmd_algorithm<160, true>, rmd160>);

template <size_t Strength, bool Concurrent = true>
using sha_algorithm = sha::algorithm<
    iif<Strength == 256, sha::h256<>,
    iif<Strength == 512, sha::h512<>, sha::h160>>, Concurrent>;

static_assert(is_same_type<sha_algorithm<160, true>, sha160>);
static_assert(is_same_type<sha_algorithm<256, true>, sha256>);
static_assert(is_same_type<sha_algorithm<512, true>, sha512>);

template <size_t Strength, bool Concurrent, bool Ripemd,
    bool_if<
       (!Ripemd && (Strength == 160 || Strength == 256 || Strength == 512)) ||
        (Ripemd && (Strength == 128 || Strength == 160))> = true>
using hash_selector = iif<Ripemd,
    rmd_algorithm<Strength, Concurrent>,
    sha_algorithm<Strength, Concurrent>>;

static_assert(is_same_type<hash_selector<128, true, true>, rmd128>);
static_assert(is_same_type<hash_selector<160, true, true>, rmd160>);
static_assert(is_same_type<hash_selector<160, true, false>, sha160>);
static_assert(is_same_type<hash_selector<256, true, false>, sha256>);
static_assert(is_same_type<hash_selector<512, true, false>, sha512>);

static_assert(hash_selector<128, true, true>::concurrent);
static_assert(hash_selector<160, true, true>::concurrent);
static_assert(hash_selector<160, true, false>::concurrent);
static_assert(hash_selector<256, true, false>::concurrent);
static_assert(hash_selector<512, true, false>::concurrent);

static_assert(!hash_selector<128, false, true>::concurrent);
static_assert(!hash_selector<160, false, true>::concurrent);
static_assert(!hash_selector<160, false, false>::concurrent);
static_assert(!hash_selector<256, false, false>::concurrent);
static_assert(!hash_selector<512, false, false>::concurrent);

// Algorithm::hash() test runner.
// ----------------------------------------------------------------------------

// hash_digest/hash_chunk overloads are not exposed, only check and array.
// There is no material performance difference between slice and chunk. The
// meaningful performance distinction is between array and non-array, since
// array size is resolved at compile time, allowing for various optimizations.
template<
    size_t Strength,          // algorithm strength (160/256/512|128/160).
    size_t Size = zero,       // 0 = full block, 1 = half, otherwise bytes.
    size_t Count = 500'000,   // test iterations.
    bool Concurrent = false,  // algorithm concurrency.
    bool Vectorized = false,  // algorithm vectorization.
    bool Intrinsic = false,   // intrinsic sha (N/A for rmd).
    bool Chunked = false,     // false for array data.
    bool Ripemd = false>      // false for sha algorithm.
bool hash(std::ostream& out, bool csv = false) noexcept
{
    using Clock = std::chrono::steady_clock;
    using Precision = std::chrono::nanoseconds;
    using Timer = perf::timer<Precision, Clock>;
    using Algorithm = perf::hash_selector<Strength, Concurrent, Ripemd>;
    constexpr auto block_size = array_count<typename Algorithm::block_t>;
    constexpr auto size = is_zero(Size) ? block_size :
        is_zero(sub1(Size)) ? to_half(block_size) : Size;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        // Generate a data_chunk or a data_array of specified size.
        // Each is unique (sequential) to preclude compiler optimization.
        const auto data = perf::get_data<size, Chunked>(seed);

        time += Timer::execution([&]() noexcept
        {
            accumulator<Algorithm>::hash(data);
        });
    }

    // Dumping output also precludes compiler removal.
    perf::output<Algorithm, Precision, size, Count, Concurrent,
        Vectorized, Intrinsic, Chunked>(out, time, csv);

    // Return value, check to preclude compiler removal if output is bypassed.
    return true;
}

} // namespace perf

#endif
