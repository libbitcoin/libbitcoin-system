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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP

#include <memory>
#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/crypto/pbkdf2_sha256.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// scrypt argument constraints.
/// [W]ork must be a power of 2 greater than 1.
/// [R]esources and [P]arallelism must satisfy [(R * P) < 2^30].
template<size_t W, size_t R, size_t P>
constexpr auto is_scrypt_args =
    ((W > one) && (power2(floored_log2(W)) == W)) &&
    (safe_multiply(R, P) < power2<uint32_t>(30u));

/// Concurrent increases memory consumption from peak minimum as a tradeoff for
/// decreased processing time. Memory is heap allocated 
template<size_t W, size_t R, size_t P, bool Concurrent = false,
    bool_if<is_scrypt_args<W, R, P>> If = true>
class scrypt
{
public:
    static constexpr auto block_size = power2(6u);

    /// Peak memory consumption for non-concurrent execution.
    static constexpr auto minimum_memory =
        (3 * (1 * 1 * block_size)) +
        (1 * (2 * R * block_size))+
        (P * (2 * R * block_size)) +
        (W * (2 * R * block_size));

    /// Peak memory consumption for fully-concurrent execution.
    static constexpr auto maximum_memory =
        (3 * P * (1 * 1 * block_size)) +
        (1 * P * (2 * R * block_size)) +
        (1 * P * (2 * R * block_size)) +
        (W * P * (2 * R * block_size));

    /// False if out of memory or size > pbkdf2::maximum_size.
    static bool hash(const data_slice& phrase, const data_slice& salt,
        uint8_t* buffer, size_t size) NOEXCEPT;

    /// Returns null hash if out of memory.
    template<size_t Size,
        if_not_greater<Size, pbkdf2::maximum_size> = true>
    static data_array<Size> hash(const data_slice& phrase,
        const data_slice& salt) NOEXCEPT
    {
        data_array<Size> out{};
        hash(phrase, salt, out.data(), Size);
        return out;
    }

private:
    static constexpr auto rblock = R * block_size * two;
    static constexpr auto count = block_size / sizeof(uint32_t);

    using integers    = std_array<uint32_t, count>;
    using block_type  = data_array<block_size>;
    using rblock_type = data_array<1 * rblock>;
    using pblock_type = data_array<P * rblock>;
    using wblock_type = data_array<W * rblock>;
    using wblock_ptr  = std::shared_ptr<wblock_type>;

    template<size_t Size>
    static constexpr void set(uint8_t* to, const uint8_t* from) NOEXCEPT;

    template<size_t Size>
    static constexpr void exc(uint8_t* to, const uint8_t* from) NOEXCEPT;

    template<typename Block>
    static inline auto allocate() NOEXCEPT;

    static constexpr auto parallel() NOEXCEPT;
    static constexpr void add(integers& to, const integers& from) NOEXCEPT;
    static constexpr size_t offset(auto a, auto b, auto c) NOEXCEPT;
    static constexpr uint8_t* get(uint8_t* p) NOEXCEPT;
    static inline uint64_t get64(uint8_t* p) NOEXCEPT;
    static block_type& salsa_1(block_type& p) NOEXCEPT;
    static bool salsa_p(uint8_t* p) NOEXCEPT;
    static bool mix(uint8_t* p) NOEXCEPT;
};

/// Litecoin/BIP38 scrypt arguments.
static_assert(is_scrypt_args< 1024, 1, 1>);
static_assert(is_scrypt_args<16384, 8, 8>);

/// words.filippo.io/the-scrypt-parameters
/// Litecoin/BIP38 minimum peak memory consumption.
static_assert(scrypt< 1024, 1, 1>::minimum_memory == 131'520);
static_assert(scrypt<16384, 8, 8>::minimum_memory == 16'786'624);

/// Litecoin/BIP38 minimum peak memory consumption.
static_assert(scrypt< 1024, 1, 1>::maximum_memory == 131'520);
static_assert(scrypt<16384, 8, 8>::maximum_memory == 134'235'648);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/scrypt.ipp>

#endif
