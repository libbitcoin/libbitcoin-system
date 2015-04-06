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
    // tcin treats file input as utf8 and translates console input to wide.
    tcout << "Enter text to stdin..." << std::endl;
    bc::tstring tstdin;
    tcin >> tstdin;
    tcout << "tcin  : " << tstdin << std::endl;

    if (argc > 1)
    {
        const auto utf = bc::to_utf(argv[1]);
        tcout << "targv : " << utf << std::endl;
    }

    // Use wide with tcout|tcerr.
    const auto utf = bc::to_utf("acción.кошка.日本国");
    tcout << "tcout : " << utf << std::endl;
    tcerr << "tcerr : " << utf << std::endl;

    // Use ascii narrow with tcout|tcerr (but not non-ascii narrow).
    tcout << "tcout : " << "narrow" << std::endl;
    tcerr << "tcerr : " << "narrow" << std::endl;

    // Use ascii wide with wcout|wcerr (but not non-ascii wide).
    std::wcout << "tcout : " << L"wide" << std::endl;
    std::wcerr << "tcerr : " << L"wide" << std::endl;

    return EXIT_SUCCESS;
}
