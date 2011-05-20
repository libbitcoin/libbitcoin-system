#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <string>
#include <vector>

namespace libbitcoin {

class script
{
    std::string data;
    
    void parse();
public:
    enum class opcode
    {
        PUSH_DATA_UINT8 = 76,
        PUSH_DATA_UINT16 = 77,
        PUSH_DATA_UINT32 = 78
    };

    struct operation
    {
        opcode op;
        std::string data;
    };
    
    std::vector<operation> parsed;
    
    script(std::string data);
    script(unsigned char *data, uint64_t length);
};

}
#endif
