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
#include <bitcoin/system/crypto/external/pbkd_sha512.hpp>

#include <bitcoin/system/crypto/external/hmac_sha512.hpp>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pbkd {
namespace sha512 {
    
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

using algorithm = sha::algorithm<sha::sha512>;
constexpr auto digest_size = array_count<algorithm::digest_t>;

bool hash(const uint8_t* passphrase, size_t passphrase_size,
    const uint8_t* salt, size_t salt_size, uint64_t iterations,
    uint8_t* buffer, size_t buffer_size) NOEXCEPT
{
    if (buffer_size > maximum_size)
        return false;

    hmac::sha512::context salted{};
    hmac::sha512::initialize(salted, passphrase, passphrase_size);
    hmac::sha512::update(salted, salt, salt_size);
    algorithm::digest_t U{};

    // uint32_t not word_t
    for (uint32_t i = 0; i * digest_size < buffer_size; ++i)
    {
        // uint32_t not word_t
        constexpr auto size = sizeof(uint32_t);

        auto context = salted;
        hmac::sha512::update(context, to_big_endian(add1(i)).data(), size);
        hmac::sha512::finalize(context, U.data());
        auto T = U;

        for (size_t j = one; j < iterations; ++j)
        {
            hmac::sha512::initialize(context, passphrase, passphrase_size);
            hmac::sha512::update(context, U.data(), digest_size);
            hmac::sha512::finalize(context, U.data());
            
            for (size_t k = 0; k < digest_size; ++k)
                T[k] ^= U[k];
        }

        const auto offset = i * digest_size;
        const auto length = limit(buffer_size - offset, digest_size);
        std::memcpy(&buffer[offset], T.data(), length);
    }

    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace sha512
} // namespace pbkd
} // namespace system
} // namespace libbitcoin
