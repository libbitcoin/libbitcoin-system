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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_INTRINSICS_INTRINSICS_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_INTRINSICS_INTRINSICS_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace intrinsics {

namespace cpu1_0
{
    constexpr uint32_t leaf = 1;
    constexpr uint32_t subleaf = 0;
    constexpr size_t sse4_ecx_bit = 19;
    constexpr size_t xsave_ecx_bit = 27;
    constexpr size_t avx_ecx_bit = 28;
}

namespace cpu7_0
{
    constexpr uint32_t leaf = 7;
    constexpr uint32_t subleaf = 0;
    constexpr size_t avx2_ebx_bit = 5;
    constexpr size_t shani_ebx_bit = 29;
}

namespace xcr0
{
    constexpr uint32_t feature = 0;
    constexpr size_t sse_bit = 1;
    constexpr size_t avx_bit = 2;
}

BC_API bool xgetbv(uint64_t& value, uint32_t index) noexcept;
BC_API bool cpuid_ex(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept;

BC_API void single_sha256_64(uint8_t* out, const uint8_t* in,
    size_t blocks) noexcept;
BC_API void double_sha256_64(uint8_t* out, const uint8_t* in,
    size_t blocks) noexcept;

#if !(defined(__x86_64__) || defined(__amd64__) || defined(__i386__))
#undef WITH_SSE4
#endif

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

#endif