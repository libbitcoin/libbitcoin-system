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
#ifndef LIBBITCOIN_SYSTEM_FILTER_BLOOM_ARRAY_HPP
#define LIBBITCOIN_SYSTEM_FILTER_BLOOM_ARRAY_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Bloom over an array of words, limited to 32 bit selection.
template <size_t M, size_t K,
    if_not_greater<K, M> = true,
    if_not_greater<M, power2<uint64_t>(bits<uint32_t>)> = true>
class bloom_array
{
public:
    /// Zero initialized words are an empty bloom.
    using type = std_array<uint64_t, ceilinged_divide(M, bits<uint64_t>)>;

    /// Bloom is bypassed.
    static constexpr bool disabled = is_zero(M) || is_zero(K);

    /// Is potential collision.
    static constexpr bool is_screened(const type& value,
        uint64_t entropy) NOEXCEPT;

    /// Add fingerprint to bloom.
    static constexpr void screen(type& value, uint64_t entropy) NOEXCEPT;

    /// Is potential collision (runtime k selections, 0 < k <= K).
    static constexpr bool is_screened(const type& value, uint64_t entropy,
        size_t k) NOEXCEPT;

    /// Add fingerprint to bloom (runtime k selections, 0 < k <= K).
    static constexpr void screen(type& value, uint64_t entropy,
        size_t k) NOEXCEPT;

protected:
    /// Return the k bit selection for the entropy.
    static constexpr size_t get_bit(size_t k, uint64_t entropy) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <size_t M, size_t K, \
    if_not_greater<K, M> If1, \
    if_not_greater<M, power2<uint64_t>(bits<uint32_t>)> If2>

#define CLASS bloom_array<M, K, If1, If2>

#include <bitcoin/system/impl/filter/bloom_array.ipp>

#undef CLASS
#undef TEMPLATE

#endif
