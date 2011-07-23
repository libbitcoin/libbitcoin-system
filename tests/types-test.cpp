#include <bitcoin/types.hpp>
#include <algorithm>
#include <iostream>
using namespace libbitcoin;

int main()
{
    data_chunk bs;
    // 0 f2 5 2a 1 0 0 0
    bs.push_back(0x00);
    bs.push_back(0xf2);
    bs.push_back(0x05);
    bs.push_back(0x2a);
    bs.push_back(0x01);
    bs.push_back(0x00);
    bs.push_back(0x00);
    bs.push_back(0x00);

    uint64_t val = cast_chunk<uint64_t>(bs, true);
    std::cout << val << "\n";
    std::cout << std::hex << val << "\n";

    std::reverse(bs.begin(), bs.end());
    val = 0;
    for (size_t i = 0; i < bs.size(); i++)
    {
        val += static_cast<uint64_t>(bs[i]) << (8*i);
    }

    std::cout << std::dec << val << "\n";
    std::cout << std::hex << val << "\n";
    return 0;
}

