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
#include <cstdlib>
#include <string>
#include <bitcoin/bitcoin.hpp>

BC_USE_LIBBITCOIN_MAIN

int bc::main(int argc, char* argv[])
{
    bc::cout << "output : acción.кошка.日本国" << std::endl;
    bc::cerr << "error : acción.кошка.日本国" << std::endl;

    bc::cout << "Enter text to input..." << std::endl;
    std::string console;
    bc::cin >> console;
    bc::cout << "input[0]  : " << console << std::endl;

    if (argc > 1)
        bc::cout << "argv[1] : " << argv[1] << std::endl;

#ifndef __MACH__
    if (environ[0] != nullptr)
        bc::cout << "environ[0] : " << environ[0] << std::endl;
#endif

    return EXIT_SUCCESS;
}
