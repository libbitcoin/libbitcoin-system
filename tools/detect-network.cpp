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