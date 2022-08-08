// Copyright (c) 2014-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_SHA256_H
#define BITCOIN_CRYPTO_SHA256_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "../../../test.hpp"
#include "../performance.hpp"

namespace baseline {

class CSHA256
{
private:
    uint32_t s[8];
    unsigned char buf[64];
    uint64_t bytes;

public:
    static const size_t OUTPUT_SIZE = 32;

    CSHA256();
    CSHA256& Write(const unsigned char* data, size_t len);
    void Finalize(unsigned char hash[OUTPUT_SIZE]);
    CSHA256& Reset();
};

void SHA256D64(unsigned char* output, const unsigned char* input, size_t blocks);

} // namespace baseline

namespace performance {

struct base_parameters
{
    static constexpr bool compressed{}; // intrinsic sha (ignored for rmd).
    static constexpr bool vectorized{}; // algorithm vectorization.
    static constexpr bool chunked{};    // false for array data.
};

// TODO: parameterize compression and vectorization in baseline.
struct base_default : base_parameters
{
    static constexpr bool compressed{ false };
    static constexpr bool vectorized{ false };
    static constexpr bool chunked{ false };
};

// Equivalent to:
// hash_digest accumulator<sha256>::hash(data);
template <typename Algorithm>
inline auto base_accumulator(auto& data) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    hash_digest out{};
    Algorithm hasher{};
    hasher.Write(data.data(), data.size());
    hasher.Finalize(out.data());
    return out;
    BC_POP_WARNING()
}

// Defaults to 1Mi rounds over 1KiB data (1GiB).
template<typename Parameters,
    size_t Count = 1024 * 1024, // test iterations (1Mi)
    size_t Size = 1024,         // bytes per iteration (1KiB)
    if_base_of<performance::base_parameters, Parameters> = true>
bool base_hash(std::ostream& out, float ghz = 3.0f, bool csv = false) noexcept
{
    using parameters = sha256_parameters<Parameters::compressed,
        Parameters::vectorized, false,  Parameters::chunked>;
    using Precision = std::chrono::nanoseconds;
    using Timer = timer<Precision>;
    using Algorithm = baseline::CSHA256;
    using P = Parameters;

    uint64_t time = zero;
    for (size_t seed = 0; seed < Count; ++seed)
    {
        const auto data = get_data<Size, P::chunked>(seed);
        time += Timer::execution([&data]() noexcept
        {
            base_accumulator<Algorithm>(*data);
        });
    }

    // Dumping output also precludes compiler removal.
    // Return value, check to preclude compiler removal if output is bypassed.
    output<parameters, Count, Size, Algorithm, Precision>(out, time, ghz, csv);
    return true;
}

} // namespace performance

#endif // BITCOIN_CRYPTO_SHA256_H
