#include <map>
#include <iostream>
#include <algorithm>
#include <stack>
#include <stdlib.h>

#include <boost/detail/endian.hpp>

#include <openssl/sha.h>
#include <openssl/ripemd.h>

#include "script.hpp"
#include "transaction.hpp"

namespace libbitcoin {

script::script(std::string data)
 : data_(data)
{
    parse();
}

script::script(unsigned char *data, uint64_t length)
{
    data_.assign(reinterpret_cast<char*>(data), length);
    parse();
}

void script::parsed(std::vector<script::operation> parsed)
{
    parsed_ = parsed;
}

std::vector<script::operation> script::parsed()
{
    return parsed_;
}

void script::parse()
{
    parsed_.clear();
    
    std::string::iterator it = data_.begin();
    
    while(it <= data_.end())
    {
        operation op;
        
        op.op = static_cast<opcode>(*it++);
        
        switch(op.op)
        {
            case opcode::PUSH_DATA_UINT8:
            {
                uint8_t length = static_cast<uint8_t>(*it++);
                op.data.assign(it, it + length);
                it += length;
                break;
            }
            case opcode::PUSH_DATA_UINT16:
            {
                uint16_t length;
            #ifdef BOOST_LITTLE_ENDIAN
                std::copy(it, it + sizeof(length), &length);
                it += sizeof(length);
                op.data.assign(it, it + length);
                it += length;
            #elif BOOST_BIG_ENDIAN
                #error "Platform not supported"
            #else
                #error "Platform not supported"
            #endif
                break;
            }
            case opcode::PUSH_DATA_UINT32:
            {
                uint32_t length;
            #ifdef BOOST_LITTLE_ENDIAN
                std::copy(it, it + sizeof(length), &length);
                it += sizeof(length);
                op.data.assign(it, it + length);
                it += length;
            #elif BOOST_BIG_ENDIAN
                #error "Platform not supported"
            #else
                #error "Platform not supported"
            #endif
                break;
            }
        }
        
        parsed_.push_back(op);
    }
}

int64_t script::parse_bignum(std::string data)
{
    /*
    the bignum in bitcoin scripts is a big endian sign & magnitude format
    the sign is the most significant bit of the least significant byte
    */
    
    int64_t value = 0;
    bool negative = data[data.size()-1] & 0x80;
    
    //strip sign
    data[data.size()-1] &= 0x7F;
    
    for(int i=(data.size()-1);i>=0;--i)
    {
        value += data[i] << (data.size()-1-i)*8;
    }
    
    if(negative)
        value = -value;
    
    return value;
}

std::string script::build_bignum(int64_t value)
{
    std::string data;
    bool negative = (value < 0);
    
    value = abs(value);
    
    #ifdef BOOST_LITTLE_ENDIAN
        std::reverse_copy(&value,&value+sizeof(value),data.begin());
    #elif BOOST_BIG_ENDIAN
        std::copy(&value,&value+sizeof(value),data.begin());
    #else
        #error Platform not supported
    #endif
    
    if(negative)
        data[0] |= 0x80;
    
    return data;
}

bool script::run(transaction parent)
{
    std::stack<std::string> stack;
    
    std::vector<operation>::iterator it;
    for(it = parsed_.begin(); it < parsed_.end(); ++it)
    {
        switch(it->op)
        {
            case opcode::PUSH_DATA_UINT8:
            case opcode::PUSH_DATA_UINT16:
            case opcode::PUSH_DATA_UINT32:
                stack.push(it->data);
                break;
            case opcode::ZERO:
            {
                stack.push(build_bignum(0));
                break;
            }
            case opcode::NOP:
                break;
            case opcode::DUPLICATE:
                stack.push(stack.top());
            case opcode::EQUAL:
            {
                std::string a,b;
                a = stack.top();
                stack.pop();
                
                b = stack.top();
                stack.pop();
                
                if (a==b)
                    stack.push(build_bignum(1));
                else
                    stack.push(build_bignum(0));
            }
            case opcode::EQUALVERIFY:
            {
                std::string a,b;
                a = stack.top();
                stack.pop();
                
                b = stack.top();
                stack.pop();
                
                return a == b;
            }
            case opcode::VERIFY:
                return parse_bignum(stack.top());
            case opcode::RETURN:
                return false;
            case opcode::HASH160:
            {
                std::string data = stack.top();
                stack.pop();
                
                unsigned char sha256_md[SHA256_DIGEST_LENGTH];
                
                SHA256_CTX sha256_ctx;
                SHA256_Init(&sha256_ctx);
                SHA256_Update(&sha256_ctx,data.c_str(),data.size());
                SHA256_Final(sha256_md,&sha256_ctx);
                
                unsigned char ripemd160_md[RIPEMD160_DIGEST_LENGTH];
                RIPEMD160_CTX ripemd160_ctx;
                RIPEMD160_Init(&ripemd160_ctx);
                RIPEMD160_Update(&ripemd160_ctx,&sha256_md,SHA256_DIGEST_LENGTH);
                RIPEMD160_Final(ripemd160_md,&ripemd160_ctx);
                
                stack.push(std::string(reinterpret_cast<char*>(ripemd160_md),RIPEMD160_DIGEST_LENGTH));
            }
            case opcode::CHECKSIG:
            {
                std::string public_key,signature;
                
                public_key = stack.top();
                stack.pop();
                
                signature = stack.top();
                stack.pop();
                
                //TODO insanity
            }
        }
    }
}

} // libbitcoin

