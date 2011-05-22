#include <map>
#include <iostream>
#include <algorithm>

#include <boost/detail/endian.hpp>

#include "script.hpp"

namespace libbitcoin {

script::script(std::string data)
{
    this->data = data;
    this->parse();
}

script::script(unsigned char *data, uint64_t length)
{
    this->data.assign(reinterpret_cast<char*>(data),length);
    this->parse();
}

void script::parse()
{
    this->parsed.clear();
    
    std::string::iterator it=this->data.begin();
    
    while(it <= this->data.end())
    {
        operation op;
        
        op.op = static_cast<opcode>(*it++);
        
        switch(op.op)
        {
            case opcode::PUSH_DATA_UINT8:
            {
                uint8_t length = static_cast<uint8_t>(*it++);
                op.data.assign(it,it+length);
                it += length;
                break;
            }
            case opcode::PUSH_DATA_UINT16:
            {
                uint16_t length;
                #ifdef BOOST_LITTLE_ENDIAN
                std::copy(it,it+sizeof(length),&length);
                it += sizeof(length);
                op.data.assign(it,it+length);
                it += length;
                #elif BOOST_BIG_ENDIAN
                #error Platform not supported
                #else
                #error Platform not supported
                #endif
                break;
            }
            case opcode::PUSH_DATA_UINT32:
            {
                uint32_t length;
                #ifdef BOOST_LITTLE_ENDIAN
                std::copy(it,it+sizeof(length),&length);
                it += sizeof(length);
                op.data.assign(it,it+length);
                it += length;
                #elif BOOST_BIG_ENDIAN
                #error Platform not supported
                #else
                #error Platform not supported
                #endif
                break;
            }
        }
        
        this->parsed.push_back(op);
    }
}

} // libbitcoin
