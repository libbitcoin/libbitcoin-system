/*
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
#include <iostream>
#include <stdlib.h>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin.hpp>

BC_USE_LIBBITCOIN_MAIN

int bc::main(int argc, char* argv[])
{
    // wcin translates input to wide.
    // Raw (non-text) inputs should use binary/cin/char.
    std::wcout << "Enter text to stdin..." << std::endl;
    std::wstring stdin16;
    std::wcin >> stdin16;
    std::wcout << "\nwcin  : " << stdin16 << std::endl;

    if (argc > 1)
    {
        const auto argv16 = bc::widen(argv[1]);
        std::wcout << "wargv : " << argv16 << std::endl;
    }

    // Use ascii narrow or wide with wcout|wcerr.
    const auto utf8to16 = bc::widen("acción.кошка.日本国");
    std::wcout << "wcout : " << utf8to16 << std::endl;
    std::wcerr << "wcerr : " << utf8to16 << std::endl;
    std::wcout << "wcout : " << "ascii" << std::endl;
    std::wcerr << "wcerr : " << "ascii" << std::endl;

    return EXIT_SUCCESS;
}