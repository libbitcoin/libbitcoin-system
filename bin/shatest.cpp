#include <iostream>

#include "sha256.h"

int main()
{
    libbitcoin::SHA256 sha256;
    sha256.update("hello");
    std::cout << sha256.final() << "\n";
    return 0;
}

