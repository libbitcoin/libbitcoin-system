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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/random.hpp>

#include <cstdint>
#include <random>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

// DO NOT USE srand() and rand() on MSVC as srand must be called per thread.
// As a result it is difficult to use safely.

// Not testable due to lack of random engine injection.
// This may be truly random depending on the underlying device.
uint64_t pseudo_random()
{
    std::random_device device;
    std::uniform_int_distribution<uint64_t> distribution;
    return distribution(device);
}

// Not testable due to lack of random engine injection.
// This may be truly random depending on the underlying device.
void pseudo_random_fill(data_chunk& chunk)
{
    std::random_device device;
    std::uniform_int_distribution<uint16_t> distribution;
    for (uint8_t& byte: chunk)
    {
        // uniform_int_distribution is undefined for sizes < 16 bits,
        // so we generate a 16 bit value and reduce it to 8 bits.
        byte = distribution(device) % std::numeric_limits<uint8_t>::max();
    }
}

} // namespace libbitcoin
