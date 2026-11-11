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

// The "try" functions are safe for instructions not compiled in.
// selector                     Write each supported build tag, one per line.
// selector <program>           Write the release file name for this machine.
// selector <program> <release> Write the release download url for this machine.
int main(int argc, char* argv[])
{
    const auto tags = detected_tags();

    if (argc < 2)
    {
        const std::array<std::pair<const char*, bool>, 8> supported
        {
            std::pair{ "sse41",      tags.sse41 },
            std::pair{ "avx2",       tags.avx2 },
            std::pair{ "avx512",     tags.avx512 },
            std::pair{ "avxifma",    tags.avxifma },
            std::pair{ "avx512ifma", tags.avx512ifma },
            std::pair{ "shani",      tags.shani },
            std::pair{ "sha2",       tags.sha2 },
            std::pair{ "sha3",       tags.sha3 }
        };

        for (const auto& [name, value]: supported)
            if (value)
                std::cout << name << std::endl;

        return EXIT_SUCCESS;
    }

    const std::string program{ argv[1] };
    const std::string repository = program == "bs" ? "libbitcoin-server" : "";

    const auto name = build_name(program, tags);
    if (repository.empty() || name.empty() || argc > 3)
    {
        std::cerr << "usage: selector [bs [<release>]]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        std::cout << name << std::endl;
        return EXIT_SUCCESS;
    }

    std::cout << "https://github.com/libbitcoin/" << repository
        << "/releases/download/" << argv[2] << "/" << name << std::endl;

    return EXIT_SUCCESS;
}
