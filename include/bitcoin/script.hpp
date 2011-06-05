#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <string>
#include <vector>
#include <stack>

#include <bitcoin/transaction.hpp>

namespace libbitcoin {

class script
{
public:
    script(std::string data);
    script(unsigned char *data, uint64_t length);
    
    enum class signature_type
    {
        ALL = 1,
        NONE = 2,
        SINGLE = 3,
        ANYONE_CAN_PAY = 0x80,
    };

    enum class opcode
    {
        PUSH_DATA_UINT8 = 76,
        PUSH_DATA_UINT16 = 77,
        PUSH_DATA_UINT32 = 78,
        NEGATIVE_1 = 79,
        ZERO = 0,
        
        NOP = 97,
        IF = 99,
        NOTIF = 100,
        ELSE = 103,
        ENDIF = 104,
        VERIFY = 105,
        RETURN = 106,
        
        TO_ALT_STACK = 107,
        FROM_ALT_STACK = 108,
        IF_DUP = 115,
        DEPTH = 116,
        DROP = 117,
        DUPLICATE = 118,
        NIP = 119,
        OVER = 120,
        PICK = 121,
        ROLL = 122,
        ROTATE = 123,
        SWAP = 124,
        TUCK = 125,
        DRTWO = 109,
        DUPLICATE_TWO = 110,
        DUPLICATE_THREE = 111,
        COPY_OVER_TWO = 112,
        ROTATE_TWO = 113,
        SWAP_TWO = 114,
        
        CAT = 126,
        SUBSTR = 127,
        LEFT = 128,
        RIGHT = 129,
        SIZE = 130,
        
        INVERT = 131,
        AND = 132,
        OR = 133,
        XOR = 134,
        EQUAL = 135,
        EQUALVERIFY = 136,
        
        HASH160 = 169,
        CHECKSIG = 172
    };

    struct operation
    {
        opcode op;
        std::string data;
    };
    
    bool run(transaction);
    
    
    const std::vector<operation>& operations();
    void operations(const std::vector<operation>&);
private:
    void parse();
    
    static int64_t parse_bignum(const std::string);
    static std::string build_bignum(const int64_t);
    
    std::string data_;
    std::vector<operation> operations_;
};

} // libbitcoin

#endif

