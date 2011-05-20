#include "script.h"

#include <iostream>

int main()
{
    libbitcoin::script script("\x4c\x01\x02");
    
    for(std::vector<libbitcoin::script::operation>::iterator it= script.parsed.begin();
        it != script.parsed.end(); ++it)
    {
        std::cout << static_cast<int>(it->op) << " " << std::hex << it->data << std::endl;
    }   
    
    return 0;
}
