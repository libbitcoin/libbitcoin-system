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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/data/data.hpp>
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

extern const std::array<uint32_t, 8> sha256_initial;
extern const std::array<uint8_t, 64> sha256_padding;
extern const std::array<uint8_t, 64> sha256x2_padding;
extern const std::array<uint8_t, 64> sha256x2_buffer;

BC_API bool xgetbv(uint64_t& value, uint32_t index) noexcept;
BC_API bool cpuid_ex(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept;

typedef struct
{
    std::array<uint32_t, 8> state;
    uint32_t count[2];
    uint8_t buffer[64];
} sha256_context;

BC_API void sha256_initialize(sha256_context& context) noexcept;
BC_API void sha256_x1_portable(uint32_t state[8], const uint8_t block[64]) noexcept;
BC_API void sha256_update(sha256_context& context, const uint8_t* input, size_t size) noexcept;
BC_API void sha256_pad(sha256_context& context) noexcept;
BC_API void sha256_finalize(sha256_context& context, uint8_t digest[32]) noexcept;
BC_API void sha256(const uint8_t* input, size_t size, uint8_t digest[32]) noexcept;

BC_API void sha256_single(uint32_t state[8], const uint8_t block[64]) noexcept;
BC_API void sha256_paired_double(uint8_t* out, const uint8_t* in, size_t blocks) noexcept;
BC_API void double_sha256_x1_portable(uint8_t* out, const uint8_t in[1 * 64]) noexcept;

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

#endif