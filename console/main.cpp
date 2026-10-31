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
#include <cstdlib>
#include <bitcoin/system.hpp>

using namespace bc::system;

struct feature
{
    const char* name;
    bool(*supported)() NOEXCEPT;
};

// The "try" functions are safe for instructions not compiled in.
constexpr std::array features
{
    // Intel/AMD
    feature{ "aesni",      &try_aesni<> },
    feature{ "vaes",       &try_vaes<> },
    feature{ "shani",      &try_shani<> },
    feature{ "sha512",     &try_sha512<> },
    feature{ "sse41",      &try_sse41<> },
    feature{ "avx2",       &try_avx2<> },
    feature{ "avx512",     &try_avx512<> },
    feature{ "avxifma",    &try_avxifma<> },
    feature{ "avx512ifma", &try_avx512ifma<> },

    // Arm
    feature{ "aes",        &try_neon_aes<> },
    feature{ "sha",        &try_neon_sha<> },
    feature{ "sha3",       &try_neon_sha3<> },
    feature{ "neon",       &try_neon<> }
};

// Write the instruction set extensions this processor supports, one per line.
int main()
{
    for (const auto& extension: features)
        if (extension.supported())
            std::cout << extension.name << std::endl;

    return EXIT_SUCCESS;
}
