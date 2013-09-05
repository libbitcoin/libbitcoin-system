#include <bitcoin/bitcoin.hpp>
using namespace bc;

int main()
{
    elliptic_curve_key ec;
    ec.new_key_pair();
    private_data raw_private_key = ec.private_key();
    std::cout << std::string(raw_private_key.begin(), raw_private_key.end());
    return 0;
}

