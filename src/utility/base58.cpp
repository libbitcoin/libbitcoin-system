#include <bitcoin/utility/base58.hpp>

#include <bitcoin/utility/big_number.hpp>

namespace libbitcoin {

// Thanks for all the wonderful bitcoin hackers

const char* base58_chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string encode_base58(const data_chunk& unencoded_data)
{                                                                                
    std::string encoded_data;
    // Expected size increase from base58 conversion is approximately 137%
    // use 138% to be safe
    encoded_data.reserve((unencoded_data.size() - 1) * 138 / 100 + 1);

    // Convert big endian data to little endian
    // Extra zero at the end make sure bignum will interpret
    // as a positive number
    data_chunk tmp_data(unencoded_data.size() + 1, 0);
    std::reverse_copy(unencoded_data.begin(), unencoded_data.end(),
        tmp_data.begin());

    big_number long_value;
    long_value.set_data(tmp_data);
    while (long_value > 0)
    {                                                                            
        auto result = divmod(long_value, 58);
        long_value = result.first;
        size_t remainder = result.second.uint32();
        encoded_data += base58_chars[remainder];
    }                                                                            
                                                                                 
    // Leading zeroes encoded as base58 zeros                                    
    for (const uint8_t unencoded_byte: unencoded_data)
    {
        if (unencoded_byte != 0)
            break;
        encoded_data += base58_chars[0];
    }

    // Convert little endian std::string to big endian
    reverse(encoded_data.begin(), encoded_data.end());
    return encoded_data;
}                                                                                

data_chunk decode_base58(const std::string& encoded_data)
{                                                                                
    big_number bn = 0;       
                                                                                 
    // Convert big endian string to bignum                                       
    for (const uint8_t current_char: encoded_data)
    {                                                                            
        bn *= 58;
        bn += std::string(base58_chars).find(current_char);
    }                                                                            
                                                                                 
    // Get bignum as little endian data                                          
    data_chunk temp_data = bn.data();       
                                                                                 
    // Trim off sign byte if present                                             
    if (temp_data.size() >= 2 &&
            temp_data.end()[-1] == 0 && temp_data.end()[-2] >= 0x80)
        temp_data.erase(temp_data.end() - 1);
                                                                                 
    // Restore leading zeros                                                     
    int leading_zeros = 0;
    for (const uint8_t current_char: encoded_data)
    {
        if (current_char != base58_chars[0])
            break;
        leading_zeros++;
    }

    data_chunk unencoded_data;
    unencoded_data.assign(leading_zeros + temp_data.size(), 0);
    // Convert little endian data to big endian                                  
    std::reverse_copy(temp_data.begin(), temp_data.end(),
        unencoded_data.end() - temp_data.size());    
    return unencoded_data;
}                                                             
                                                                                 
} // namespace libbitcoin

