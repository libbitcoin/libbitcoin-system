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
#include <bitcoin/bitcoin.hpp>

// Must save all non-ascii files as utf8 (CP65001) without signature/BOM.
// int wmain(int argc, wchar_t* argv[])
int main(int argc, char* argv[])
{
    // Establish UTF8 stdio and patch wcin for keyboard input.
    bc::unicode_streams::initialize();

    // Pass the UTF args to internal processing.
    // TODO: confirm boost::program_options handling of UTF8 values.
    //auto buffer = bc::narrow(argc, argv);
    //auto args = reinterpret_cast<char**>(&buffer[0]);

    // wcin translates input to wide.
    // Raw (non-text) inputs should use binary/cin/char.
    std::wcout << "Enter text to stdin..." << std::endl;
    std::wstring stdin16;
    std::wcin >> stdin16;
    std::wcout << "\nwcin  : " << stdin16 << std::endl;

    //if (argc > 1)
    //{
    //    const auto argv16 = bc::widen(args[1]);
    //    std::wcout << "wargv : " << argv16 << std::endl;
    //}

    // Use ascii narrow or wide with wcout|wcerr.
    const auto utf8to16 = bc::widen("acción.кошка.日本国");
    std::wcout << "wcout : " << utf8to16 << std::endl;
    std::wcerr << "wcerr : " << utf8to16 << std::endl;
    std::wcout << "wcout : " << "ascii" << std::endl;
    std::wcerr << "wcerr : " << "ascii" << std::endl;

    // Don't use cout|cerr|cin (aborts on assertion)
    //std::cout << "cout ascii : " << "racer-x" << std::endl;
    //std::cerr << "cerr ascii : " << "racer-x" << std::endl;
    //std::string ina("test");
    //std::cin >> ina;

    // Don't use L translation when the source is UTF-8 w/out BOM (mangles).
    //const auto utf16 = L"acción.кошка.日本国";
    //std::wcout << "wcout : " << utf16 << std::endl;

    return EXIT_SUCCESS;
}
