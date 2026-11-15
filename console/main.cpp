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

// A console owned by this process alone (double click) closes on exit.
static void hold_console()
{
#if defined(HAVE_WINDOWS)
    DWORD process{};
    if (GetConsoleProcessList(&process, 1) == 1)
    {
        std::cout << "Press <enter> to exit..." << std::endl;
        std::cin.get();
    }
#endif
}

// The "try" functions are safe for instructions not compiled in.
// selector           Write the bs release file name for this machine.
// selector <release> Write the bs release download url for this machine.
// selector --tags    Write each supported build tag, one per line.
int main(int argc, char* argv[])
{
    const auto tags = detected_tags();
    const std::string option{ argc == 2 ? argv[1] : "" };

    if (option == "--tags")
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

    const auto name = build_name("bs", tags);
    if (name.empty() || argc > 2 || option.starts_with("-"))
    {
        std::cerr << "usage: selector [<release> | --tags]" << std::endl;
        hold_console();
        return EXIT_FAILURE;
    }

    if (option.empty())
        std::cout << name << std::endl;
    else
        std::cout << "https://github.com/libbitcoin/libbitcoin-server"
            << "/releases/download/" << option << "/" << name << std::endl;

    hold_console();
    return EXIT_SUCCESS;
}
