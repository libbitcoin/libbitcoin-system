#include <string.h>

#include "bitcoin/transaction.hpp"

#define LOLHACKS(x) std::string(x,sizeof(x)-1)

int main()
{
    libbitcoin::transaction_output output = {
        5000000000,
        LOLHACKS("\x41\x04\x67\x8A\xFD\xB0\xFE\x55\x48\x27\x19\x67\xF1\xA6\x71\x30\xB7\x10\x5C\xD6\xA8\x28\xE0\x39\x09\xA6\x79\x62\xE0\xEA\x1F\x61\xDE\xB6\x49\xF6\xBC\x3F\x4C\xEF\x38\xC4\xF3\x55\x04\xE5\x1E\xC1\x12\xDE\x5C\x38\x4D\xF7\xBA\x0B\x8D\x57\x8A\x4C\x70\x2B\x6B\xF1\x1D\x5F\xAC")
    };

    libbitcoin::transaction_input input = {
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        4294967295,
        LOLHACKS("\x04\xFF\xFF\x00\x1D\x01\x04\x45\x54\x68\x65\x20\x54\x69\x6D\x65\x73\x20\x30\x33\x2F\x4A\x61\x6E\x2F\x32\x30\x30\x39\x20\x43\x68\x61\x6E\x63\x65\x6C\x6C\x6F\x72\x20\x6F\x6E\x20\x62\x72\x69\x6E\x6B\x20\x6F\x66\x20\x73\x65\x63\x6F\x6E\x64\x20\x62\x61\x69\x6C\x6F\x75\x74\x20\x66\x6F\x72\x20\x62\x61\x6E\x6B\x73"),
        4294967295
    };

    libbitcoin::transaction transaction = {
        {0x3B,0xA3,0xED,0xFD,0x7A,0x7B,0x12,0xB2,0x7A,0xC7,0x2C,0x3E,0x67,0x76,0x8F,0x61,0x7F,0xC8,0x1B,0xC3,0x88,0x8A,0x51,0x32,0x3A,0x9F,0xB8,0xAA,0x4B,0x1E,0x5E,0x4A},
        1,
        0
    };

    transaction.inputs.push_back(input);
    transaction.outputs.push_back(output);
    
    unsigned char hash[32];
    memcpy(hash,transaction.hash,32);
    transaction.calculate_hash();
    printf("%d\n",memcmp(hash,transaction.hash,32));
    return 0;
}
