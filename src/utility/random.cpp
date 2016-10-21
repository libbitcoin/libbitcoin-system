/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/random.hpp>

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <random>
#include <thread>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

// DO NOT USE srand() and rand() on MSVC as srand must be called per thread.
// Values may be truly random depending on the underlying device.
// TODO: convert pseudo_random/nonzero_pseudo_random methods to templates.

uint64_t pseudo_random()
{
    return pseudo_random(0, max_uint64);
}

uint64_t pseudo_random(uint64_t begin, uint64_t end)
{
    std::uniform_int_distribution<uint64_t> distribution(begin, end);
    std::random_device device;
    return distribution(device);
}

uint64_t nonzero_pseudo_random()
{
    return pseudo_random(1, max_uint64);
}

uint64_t nonzero_pseudo_random(uint64_t end)
{
    return pseudo_random(1, end);
}

void pseudo_random_fill(data_chunk& chunk)
{
    // uniform_int_distribution is undefined for sizes < 16 bits.
    std::uniform_int_distribution<uint16_t> distribution(0, max_uint8);
    std::random_device device;

    for (auto& byte: chunk)
        byte = static_cast<uint8_t>(distribution(device));
}

// Randomly select a time duration in the range:
// [(expiration - expiration / ratio) .. expiration]
// Not fully testable due to lack of random engine injection.
asio::duration pseudo_randomize(const asio::duration& expiration,
    uint8_t ratio)
{
    using namespace bc::asio;
    using namespace std::chrono;

    if (ratio == 0)
        return expiration;

    // Uses milliseconds level resolution.
    const auto max_expire = duration_cast<milliseconds>(expiration).count();

    // [10 secs, 4] => 10000 / 4 => 2500
    const auto limit = max_expire / ratio;

    if (limit == 0)
        return expiration;

    // [0..2^64) % 2500 => [0..2500]
    const auto random_offset = static_cast<int>(pseudo_random(0, limit));

    // (10000 - [0..2500]) => [7500..10000]
    const auto expires = max_expire - random_offset;

    // [7.5..10] second duration.
    return milliseconds(expires);
}

} // namespace libbitcoin
