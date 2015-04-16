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
#include <iostream>
#include <string>
#include <bitcoin/bitcoin.hpp>

BC_USE_LIBBITCOIN_MAIN

int bc::main(int argc, char* argv[])
{
    unicode_istream input(std::cin, std::wcin);
    unicode_ostream output(std::cout, std::wcout);
    unicode_ostream error(std::cerr, std::wcerr);

    // Use utf-8 with output|error.
    output << "output : acción.кошка.日本国" << std::endl;
    error << "error : acción.кошка.日本国" << std::endl;

    // input treats file input as utf8 and translates console input to utf8.
    output << "Enter text to input..." << std::endl;
    std::string console;
    input >> console;
    output << "input[0]  : " << console << std::endl;

    if (argc > 1)
        output << "argv[1] : " << argv[1] << std::endl;

    return EXIT_SUCCESS;
}
