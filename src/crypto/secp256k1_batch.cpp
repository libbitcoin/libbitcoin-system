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
#include <shared_mutex>
#include <span>
#if defined(HAVE_ULTRAFAST)
    #include <ufsecp_libbitcoin.h>
#endif
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

using namespace batchy;

// polymorphic namespace selectors (template support)
// ----------------------------------------------------------------------------
// local

inline bool verify_signature(const schnorr::batch& single) NOEXCEPT
{
    return schnorr::verify_signature(single.point, single.digest,
        single.signature);
}

inline bool verify_signature(const threshold::batch& single) NOEXCEPT
{
    return schnorr::verify_signature(single.point, single.digest,
        single.signature);
}

inline bool verify_signature(const ecdsa::batch& single) NOEXCEPT
{
    return ecdsa::verify_signature(single.point, single.digest,
        single.signature);
}

inline bool verify_signature(const multisig::batch& single) NOEXCEPT
{
    return ecdsa::verify_signature(single.point, single.digest,
        single.signature);
}

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// batch_verify
// ----------------------------------------------------------------------------

#if defined(HAVE_ULTRAFAST)
    
template <typename Batch>
data_chunk batch_verify(const std::span<Batch>& batch, bool) NOEXCEPT
{
    // OOM is unrecoverable.
    static thread_local ufsecp::lbtc::Controller context{ UFSECP_LBTC_AUTO };
    if (!context.ok()) std::abort();

    const auto count = batch.size();
    data_chunk results(count);
    const auto out = results.data();
    const auto in = pointer_cast<const uint8_t>(batch.data());

    if constexpr (is_same_type<Batch, schnorr::batch>)
    {
        constexpr auto extra_size = sizeof(Batch) - (sizeof(hash_digest) +
            sizeof(ec_xonly) + sizeof(ec_signature));
        ufsecp_lbtc_verify_schnorr(context.get(), in, count, extra_size, out);
    }
    else
    {
        constexpr auto extra_size = sizeof(Batch) - (sizeof(hash_digest) +
            sizeof(ec_compressed) + sizeof(ec_signature));
        ufsecp_lbtc_verify_ecdsa(context.get(), in, count, extra_size, out);
    }

    return results;
}

#else

template <typename Batch>
data_chunk batch_verify(const std::span<Batch>& batch, bool turbo) NOEXCEPT
{
    // par_if doesn't throw.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    const auto policy = poolstl::execution::par_if(turbo);
    BC_POP_WARNING()

    // Used only to produce order for concurrency.
    std::vector<size_t> it(batch.size());
    std::iota(it.begin(), it.end(), zero);

    // Collect signature validation results as corresponding integer booleans.
    data_chunk results(batch.size());
    std::for_each(policy, it.cbegin(), it.cend(), [&](size_t row) NOEXCEPT
    {
        results.at(row) = to_int<uint8_t>(verify_signature(batch[row]));
    });

    return results;
}

#endif

// utility
// ----------------------------------------------------------------------------
// local

using multisig_matrix = std::array<uint16_t, bits<uint16_t>>;

// O(1) as m and n are bounded at 16.
inline bool has_valid_path(uint8_t m_sigs, uint8_t n_keys,
    const multisig_matrix& success) NOEXCEPT
{
    multisig_matrix matrix{};

    for (size_t key{}; key < n_keys; ++key)
    {
        for (size_t sig{}; sig < m_sigs; ++sig)
        {
            if (get_right(success.at(sig), key))
            {
                uint16_t length{};
                for (size_t subkey{}; subkey < key; ++subkey)
                    length = greater(matrix.at(subkey), length);

                if (++length >= m_sigs) return true;
                matrix.at(key) = greater(length, matrix.at(key));
            }
        }
    }

    return false;
}
    
// to_links
// ----------------------------------------------------------------------------

// Trivial single signature correlation.
template <typename Batch>
links_t to_links(const data_chunk& out,
    const std::span<const Batch>& in) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());

    // Used only to produce order for concurrency.
    std::vector<size_t> it(in.size());
    std::iota(it.begin(), it.end(), zero);

    links_t fails(zero);
    std::shared_mutex mutex{};

    std::for_each(poolstl::execution::par, it.cbegin(), it.cend(),
        [&](size_t row) NOEXCEPT
        {
            // Failure is *extremely* rare, so this is very efficient.
            if (!to_bool(out.at(row)))
            {
                std::unique_lock lock{ mutex };
                fails.push_back(from_little_array<link_t>(in[row].id));
            }
        });

    return fails;
}

// Specialized threshold correlation.
// O(n) over the sig set, ~100 bytes of stack, no heap.
template <>
links_t to_links<threshold::batch>(const data_chunk& out,
    const threshold::batch::span& in) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());

    if (in.empty())
        return {};

    size_t group{};
    links_t fails{};
    for (auto index = one; index <= in.size(); ++index)
    {
        // Find the start of the next group (or end).
        if ((index != in.size()) &&
            (in[index].id == in[group].id) &&
            (in[index].group == in[group].group))
            continue;

        // Count successes in this group.
        size_t successes{};
        for (auto row = group; row < index; ++row)
            if (to_bool(out.at(row)))
                ++successes;

        // Fail if group successes < required.
        if (successes < in[group].required)
            fails.push_back(from_little_array<link_t>(in[group].id));

        group = index;
    }

    return fails;
}

// Specialized multisig correlation.
// O(n) over the sig set, ~100 bytes of stack, no heap.
template <>
links_t to_links<multisig::batch>(const data_chunk& out, 
    const multisig::batch::span& in) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());

    if (in.empty())
        return {};

    size_t group{};
    links_t fails{};
    for (auto index = one; index <= in.size(); ++index)
    {
        // Find the start of the next group (or end).
        if ((index != in.size()) &&
            (in[index].id == in[group].id) &&
            (in[index].group == in[group].group))
            continue;

        // Process the previous group.
        multisig_matrix matrix{};
        uint8_t max_sig{}, max_key{};
        for (auto row = group; row < index; ++row)
        {
            static_assert(is_same_type<pairing_t, uint8_t>);
            const auto [sig, key] = unpack_word<uint8_t>(in[row].pair);
            if (to_bool(out.at(row))) set_right_into(matrix.at(sig), key);
            max_sig = greater(sig, max_sig);
            max_key = greater(key, max_key);
        }

        // Evaluate matrix for valid solution.
        if (!has_valid_path(++max_sig, ++max_key, matrix))
            fails.push_back(from_little_array<link_t>(in[group].id));

        group = index;
    }

    return fails;
}

BC_POP_WARNING()

// evaluate
// ----------------------------------------------------------------------------
// static/protected

data_chunk schnorr::batch::evaluate(const schnorr::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return batch_verify(batch, turbo);
}

data_chunk threshold::batch::evaluate(const threshold::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return batch_verify(batch, turbo);
}

data_chunk ecdsa::batch::evaluate(const ecdsa::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return batch_verify(batch, turbo);
}

data_chunk multisig::batch::evaluate(const multisig::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return batch_verify(batch, turbo);
}

// correlate
// ----------------------------------------------------------------------------
// static/protected

links_t schnorr::batch::correlate(const data_chunk& out,
    const schnorr::batch::span& in) NOEXCEPT
{
    return to_links(out, in);
}

links_t threshold::batch::correlate(const data_chunk& out,
    const threshold::batch::span& in) NOEXCEPT
{
    return to_links(out, in);
}

links_t ecdsa::batch::correlate(const data_chunk& out,
    const ecdsa::batch::span& in) NOEXCEPT
{
    return to_links(out, in);
}

links_t multisig::batch::correlate(const data_chunk& out,
    const multisig::batch::span& in) NOEXCEPT
{
    return to_links(out, in);
}

// verify
// ----------------------------------------------------------------------------
// static/public

links_t schnorr::batch::verify(const schnorr::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return correlate(evaluate(batch, turbo), batch);
}

links_t threshold::batch::verify(const threshold::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return correlate(evaluate(batch, turbo), batch);
}

links_t ecdsa::batch::verify(const ecdsa::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return correlate(evaluate(batch, turbo), batch);
}

links_t multisig::batch::verify(const multisig::batch::span& batch,
    bool turbo) NOEXCEPT
{
    return correlate(evaluate(batch, turbo), batch);
}

} // namespace system
} // namespace libbitcoin
