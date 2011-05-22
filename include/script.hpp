#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <string>
#include <vector>

#include "transaction.h"

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
        PUSH_DATA_UINT32 = 78,
        NEGATIVE_1 = 79,
        ZERO = 0,
        
        OP_NOP = 97,
        OP_IF = 99,
        OP_NOTIF = 100,
        OP_ELSE = 103,
        OP_ENDIF = 104,
        OP_VERIFY = 105,
        OP_RETURN = 106,
        
        OP_TO_ALT_STACK = 107,
        OP_FROM_ALT_STACK = 108,
        OP_IF_DUP = 115,
        OP_DEPTH = 116,
        OP_DROP = 117,
        OP_DUPLICATE = 118,
        OP_NIP = 119,
        OP_OVER = 120,
        OP_PICK = 121,
        OP_ROLL = 122,
        OP_ROTATE = 123,
        OP_SWAP = 124,
        OP_TUCK = 125,
        OP_DROP_TWO = 109,
        OP_DUPLICATE_TWO = 110,
        OP_DUPLICATE_THREE = 111,
        OP_COPY_OVER_TWO = 112,
        OP_ROTATE_TWO = 113,
        OP_SWAP_TWO = 114,
        
        OP_CAT = 126,
        OP_SUBSTR = 127,
        OP_LEFT = 128,
        OP_RIGHT = 129,
        OP_SIZE = 130,
        
        OP_INVERT = 131,
        OP_AND = 132,
        OP_OR = 133,
        OP_XOR = 134,
        OP_EQUAL = 135,
        OP_EQUALVERIFY = 136,
    };

    struct operation
    {
        opcode op;
        std::string data;
    };
    
    std::vector<operation> parsed;
    
    script(std::string data);
    script(unsigned char *data, uint64_t length);
    
    bool run(transaction);
};

}
#endif
