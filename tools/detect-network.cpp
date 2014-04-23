/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
/*
  Detect which network libbitcoin was compiled for.
*/
#include <bitcoin/bitcoin.hpp>

using namespace bc;
int main(int argc, char** argv)
{
	switch(magic_value())
	{
		case 0xd9b4bef9:
			std::cout << "Network: mainnet" << std::endl;
			break;

		case 0x0709110b:
			std::cout << "Network: testnet3" << std::endl;
			break;

		default:
			std::cout << "Network: unknown" << std::endl;
	}
	std::cout << "Magic Value: " << magic_value() << std::endl;
}

