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
#include <bitcoin/system/crypto/maybe_random.hpp>

#include <random>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Values may or may not be truly random depending on the underlying device.
std::random_device& maybe_random::get_device() NOEXCEPT
{
    // Thread storage duration: each thread has its own instance, allocated
    // when the thread begins and deallocated when it ends (static implied).
    thread_local std::random_device device{};

    // Reference remains valid for the lifetime of the calling thread.
    return device;
}

// Gather 256 bits of device entropy, conditioned by sha256.
static hash_digest entropy(std::random_device& device) NOEXCEPT
{
    using word = std::random_device::result_type;
    constexpr auto words = hash_size / sizeof(word);

    data_array<words * sizeof(word)> seed{};
    auto it = seed.begin();

    for (size_t count{}; count < words; ++count)
    {
        const auto value = to_little_endian(device());
        it = std::copy(value.begin(), value.end(), it);
    }

    return sha256_hash(seed);
}

void maybe_random::fill(byte_span out) NOEXCEPT
{
    // Filled in independently drawn blocks of conditioned entropy.
    for (size_t offset{}; offset < out.size(); offset += hash_size)
    {
        const auto block = entropy(get_device());
        const auto size = std::min(hash_size, out.size() - offset);
        std::copy_n(block.begin(), size, std::next(out.begin(), offset));
    }
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
