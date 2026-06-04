/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/crypto/secp256k1_batch.hpp>

#include <algorithm>
#include <numeric>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/hash/hash.hpp>

// par_if() doesn't throw, array indexing is required for span<> in c++20.
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

namespace libbitcoin {
namespace system {

// schnorr
// ----------------------------------------------------------------------------

namespace schnorr {

bool verify_signature(const triple& single) NOEXCEPT
{
    return verify_signature(single.point, single.digest, single.signature);
}

triple::tokens verify_signatures(const triples& batch,
    bool NOT_ULTRAFAST(turbo)) NOEXCEPT
{
#if defined(HAVE_ULTRAFAST)
    static thread_local ufsecp::lbtc::Controller context{ UFSECP_LBTC_AUTO };

    // Unrecoverable (OOM).
    if (!context.ok())
        std::abort();

    // The results vector is the only allocation.
    const auto count = batch.size();
    std::vector<uint8_t> results(count);
    const auto in = pointer_cast<const uint8_t>(batch.data());
    const auto out = results.data();
    ufsecp_lbtc_verify_schnorr(context.get(), in, count, triple::id_size, out);
#else
    const auto policy = poolstl::execution::par_if(turbo);

    // Used only to produce order for concurrency.
    std::vector<size_t> index(batch.size());
    std::iota(index.begin(), index.end(), zero);

    // Collect signature validation results as corresponding integer booleans.
    std::vector<uint8_t> results(batch.size());
    std::for_each(policy, index.begin(), index.end(), [&](size_t row) NOEXCEPT
    {
        results.at(row) = to_int<uint8_t>(verify_signature(batch[row]));
    });
#endif

    // Map success results to failures only.
    triple::tokens tokens{};
    for (size_t row{}; row < results.size(); ++row)
        if (!to_bool(results.at(row)))
            tokens.push_back(batch[row].identifier);

    return tokens;
}

} // namespace schnorr

// ecdsa
// ----------------------------------------------------------------------------

namespace ecdsa {

bool verify_signature(const triple& single) NOEXCEPT
{
    return verify_signature(single.point, single.digest, single.signature);
}

triple::tokens verify_signatures(const triples& batch,
    bool NOT_ULTRAFAST(turbo)) NOEXCEPT
{
#if defined(HAVE_ULTRAFAST)
    static thread_local ufsecp::lbtc::Controller context{ UFSECP_LBTC_AUTO };

    // Unrecoverable (OOM).
    if (!context.ok())
        std::abort();

    // The results vector is the only allocation.
    const auto count = batch.size();
    std::vector<uint8_t> results(count);
    const auto in = pointer_cast<const uint8_t>(batch.data());
    const auto out = results.data();
    ufsecp_lbtc_verify_ecdsa(context.get(), in, count, triple::id_size, out);
#else
    const auto policy = poolstl::execution::par_if(turbo);

    // Used only to produce order for concurrency.
    std::vector<size_t> index(batch.size());
    std::iota(index.begin(), index.end(), zero);

    // Collect signature validation results as corresponding integer booleans.
    std::vector<uint8_t> results(batch.size());
    std::for_each(policy, index.begin(), index.end(), [&](size_t row) NOEXCEPT
    {
        results.at(row) = to_int<uint8_t>(verify_signature(batch[row]));
    });
#endif

    // Map success results to failures only.
    triple::tokens tokens{};
    for (size_t row{}; row < results.size(); ++row)
        if (!to_bool(results.at(row)))
            tokens.push_back(batch[row].identifier);

    return tokens;
}

} // namespace ecdsa

// multisig
// ----------------------------------------------------------------------------

namespace multisig {

bool verify_signature(const triple& single) NOEXCEPT
{
    return ecdsa::verify_signature(single.point, single.digest,
        single.signature);
}

triple::tokens verify_signatures(const triples& batch,
    bool NOT_ULTRAFAST(turbo)) NOEXCEPT
{
#if defined(HAVE_ULTRAFAST)
    static thread_local ufsecp::lbtc::Controller context{ UFSECP_LBTC_AUTO };

    // Unrecoverable (OOM).
    if (!context.ok())
        std::abort();

    // The results vector is the only allocation.
    const auto count = batch.size();
    std::vector<uint8_t> results(count);
    const auto in = pointer_cast<const uint8_t>(batch.data());
    const auto out = results.data();
    ufsecp_lbtc_verify_ecdsa(context.get(), in, count, triple::id_size, out);
#else
    const auto policy = poolstl::execution::par_if(turbo);

    // Used only to produce order for concurrency.
    std::vector<size_t> index(batch.size());
    std::iota(index.begin(), index.end(), zero);

    // Collect signature validation results as corresponding integer booleans.
    std::vector<uint8_t> results(batch.size());
    std::for_each(policy, index.begin(), index.end(), [&](size_t row) NOEXCEPT
    {
        results.at(row) = to_int<uint8_t>(verify_signature(batch[row]));
    });
#endif

    // Map success results to failures only.
    triple::tokens tokens{};
    for (size_t row{}; row < results.size(); ++row)
        if (!to_bool(results.at(row)))
            tokens.push_back(batch[row].identifier);

    return tokens;
}

} // namespace multisig

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()
BC_POP_WARNING()
