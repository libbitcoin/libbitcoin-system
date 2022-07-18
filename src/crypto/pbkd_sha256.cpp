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
#include <bitcoin/system/crypto/pbkd_sha256.hpp>

#include <bitcoin/system/crypto/hmac_sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pbkd {
namespace sha256 {

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
bool hash(const uint8_t* passphrase, size_t passphrase_size,
    const uint8_t* salt, size_t salt_size, uint64_t iterations,
    uint8_t* buffer, size_t buffer_size) NOEXCEPT
BC_POP_WARNING()
{
    if (buffer_size > maximum_size)
        return false;

    hmac::sha256::context salted;
    hmac::sha256::initialize(salted, passphrase, passphrase_size);
    hmac::sha256::update(salted, salt, salt_size);
    
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    system::sha256::digest U;
    BC_POP_WARNING()

    for (uint32_t i = 0; i * system::sha256::digest_size < buffer_size; ++i)
    {
        constexpr auto size = sizeof(uint32_t);

        auto context = salted;
        hmac::sha256::update(context, to_big_endian(add1(i)).data(), size);
        hmac::sha256::finalize(context, U.data());
        auto T = U;

        for (size_t j = one; j < iterations; ++j)
        {
            hmac::sha256::initialize(context, passphrase, passphrase_size);
            hmac::sha256::update(context, U.data(), system::sha256::digest_size);
            hmac::sha256::finalize(context, U.data());
            
            BC_PUSH_WARNING(NO_ARRAY_INDEXING)
            BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
            for (size_t k = 0; k < system::sha256::digest_size; ++k)
                T[k] ^= U[k];
            BC_POP_WARNING()
            BC_POP_WARNING()
        }

        const auto offset = i * system::sha256::digest_size;
        const auto length = limit(buffer_size - offset, system::sha256::digest_size);

        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        std::memcpy(&buffer[offset], T.data(), length);
        BC_POP_WARNING()
        BC_POP_WARNING()
        BC_POP_WARNING()
    }

    return true;
}

} // namespace sha256
} // namespace pbkd
} // namespace system
} // namespace libbitcoin
