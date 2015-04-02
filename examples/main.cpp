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
// assert
#include <cassert>
// _O_U8TEXT
#include <fcntl.h>
// _setmode, _fileno, _isatty
#include <io.h>
// std::wcout, std::wcerr, std::endl, getchar
#include <iostream>
// SetConsoleCP, GetStdHandle, GetConsoleMode, SetCurrentConsoleFontEx
#include <windows.h>
// wstring
#include <string>
// wcscpy
#include <string.h>
// getwchar
#include <wchar.h>
// widen, narrow
#include <bitcoin/bitcoin.hpp>

// Must save file as utf8 (CP65001) without signature/BOM.
int wmain(int argc, wchar_t* argv[])
{
#ifdef _MSC_VER
    // Set console code page to UTF-8.
    auto set_code_page = ::SetConsoleCP(65001);
    assert(set_code_page != FALSE);

    // Set console font to Lucida Console 16.
    CONSOLE_FONT_INFOEX font;
    font.cbSize = sizeof(font);
    font.nFont = 0;
    font.dwFontSize.X = 0;
    font.dwFontSize.Y = 16;
    font.FontFamily = FF_DONTCARE;
    font.FontWeight = FW_NORMAL;
    auto result = wcscpy(font.FaceName, L"Lucida Console");
    assert(result != NULL);

    auto console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    assert(console != INVALID_HANDLE_VALUE && console != NULL);
    auto set_font = ::SetCurrentConsoleFontEx(console, FALSE, &font);
    assert(set_font != FALSE);

    console = ::GetStdHandle(STD_ERROR_HANDLE);
    assert(console != INVALID_HANDLE_VALUE && console != NULL);
    set_font = ::SetCurrentConsoleFontEx(console, FALSE, &font);
    assert(set_font != FALSE);

    // Set stdout/stderr/stdin file stream mode to utf8.
    auto set_mode = _setmode(_fileno(stdout), _O_U8TEXT);
    assert(set_mode != -1);
    set_mode = _setmode(_fileno(stderr), _O_U8TEXT);
    assert(set_mode != -1);
    set_mode = _setmode(_fileno(stdin), _O_U8TEXT);
    assert(set_mode != -1);
#endif

    // Must use widen vs. L when the source is UTF-8 w/out BOM.
    const auto utf8to16 = bc::widen("acción.кошка.日本国");

    // Use wcin for input (needs fix to not mangle).
    std::wcout << "Enter text to stdin..." << std::endl;
    std::wstring stdin16;
    std::wcin >> stdin16;
    std::wcout << "\nwcin  : " << stdin16 << std::endl;

    // Use wmain to get args as wchar_t (only way to get unicode args).
    if (argc > 1)
    {
        const auto argv16 = argv[1];
        std::wcout << "wargv : " << argv16 << std::endl;
    }

    // Use narrow or wide with wcout/wcerr.
    std::wcout << "wcout : " << utf8to16 << std::endl;
    std::wcerr << "wcerr : " << utf8to16 << std::endl;

    // Use getwchar when stdin is set to wide streaming.
    std::wcout << "\nPress enter to continue" << std::endl;
    getwchar();
    getwchar();

    // NOTES:

    // With _O_U8TEXT on stdout
    // Don't use C FILE level narrow characters operations on cout (assert).
    //std::cout << "cout : " << "cout" << std::endl;

    // With _O_U8TEXT on stderr
    // Don't use non-ASCII characters in narrow operations on wcerr (mangle).
    //std::wcerr << "acción.кошка.日本国" << std::endl;
    // Don't use C FILE level narrow characters operations on cerr (assert).
    //std::cerr << "cerr : " << "cerr" << std::endl;

    // With _O_U8TEXT on stdin
    // Don't use cin for input (assert).
    //std::string ina;
    //std::cin >> ina;

    // Always use wmain (argv), wcin, wcout, wcerr.
    // Do not need to translate to wide for wcout.
    // Cannot use -w stdio, will crash (without hack).
    // Must translate to wide for wcerr (if not ASCII) without hack.
    // Use of wcin requires small hack to not mangle.
    // Don't use wide literals.
    // Save source files as UTF8 w/out BOM.

    // TODO: 
    // Test file I/O and streaming.
    // Normalize arg array to UTF-8.

    return 0;
}
