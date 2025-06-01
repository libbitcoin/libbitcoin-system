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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TAPSCRIPT_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_TAPSCRIPT_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Move of the shared pointer object is intended.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)

inline tapscript::tapscript(chunk_cptr&& control) NOEXCEPT
  : control_(is_control(*control) ? std::move(control) : nullptr)
{
}

BC_POP_WARNING()

inline tapscript::tapscript(const chunk_cptr& control) NOEXCEPT
  : control_(is_control(*control) ? control : nullptr)
{
}

// static
inline bool tapscript::is_control(const data_chunk& control) NOEXCEPT
{
    // [v:1][p:32][[k:32] 0..128] or [v:1][[n:32] 1..129].
    constexpr auto min = add1(ec_xonly_size * add1(zero));
    constexpr auto max = add1(ec_xonly_size * add1(taproot_max_keys));

    const auto size = control.size();
    return !is_limited(size, min, max) &&
        is_zero(floored_modulo(sub1(size), ec_xonly_size));
}

inline bool tapscript::is_valid() const NOEXCEPT
{
    return !is_null(control_);
}

inline bool tapscript::is_tapscript() const NOEXCEPT
{
    return version() == tapscript_version;
}

// v -> c[0] & ~0xfe
inline bool tapscript::parity() const NOEXCEPT
{
    BC_ASSERT(is_valid() && is_control(*control_));
    constexpr auto parity_mask = bit_not(tapscript_mask);

    return to_bool(bit_and(control_->front(), parity_mask));
}

// v -> c[0] & 0xfe
inline uint8_t tapscript::version() const NOEXCEPT
{
    BC_ASSERT(is_valid() && is_control(*control_));

    return bit_and(control_->front(), tapscript_mask);
}

// p -> c[1:33]
inline const ec_xonly& tapscript::key() const NOEXCEPT
{
    BC_ASSERT(is_valid() && is_control(*control_));

    const auto start = std::next(control_->data(), one);
    return unsafe_array_cast<uint8_t, ec_xonly_size>(start);
}

// e -> c[33:65]...
inline size_t tapscript::count() const NOEXCEPT
{
    BC_ASSERT(is_valid() && is_control(*control_));
    constexpr auto first = add1(ec_xonly_size);

    const auto bytes = floored_subtract(control_->size(), first);
    return floored_divide(bytes, ec_xonly_size);
}

// e -> c[33:65]...
inline const tapscript::keys_t& tapscript::keys() const NOEXCEPT
{
    BC_ASSERT(is_valid() && is_control(*control_));
    constexpr auto first = add1(ec_xonly_size);

    // This is an exceptional case of casting the contiguous buffer to an array
    // reference of a potentially larger (bounded) size. The actual extent of
    // the buffer is exposed by the count() member, and the caller must ensure
    // that the resulting array reference is not read past the count() value.
    // This is the purpose of vector cast however that imposes allocation cost.
    const auto start = std::next(control_->data(), first);
    return unsafe_array_cast<ec_xonly, taproot_max_keys>(start);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
