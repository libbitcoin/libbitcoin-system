#include <bitcoin/util/big_number.hpp>

// TODO Debug
#include <iostream>

namespace libbitcoin {

big_number::big_number()
{
    BN_init(&bignum_);
}

void big_number::set_compact(uint32_t compact)
{
    size_t size = compact >> 24;
    data_chunk number_repr(4 + size);
    number_repr[3] = size;
    if (size >= 1) 
        number_repr[4] = (compact >> 16) & 0xff;
    if (size >= 2) 
        number_repr[5] = (compact >> 8) & 0xff;
    if (size >= 3) 
        number_repr[6] = (compact >> 0) & 0xff;
    BN_mpi2bn(&number_repr[0], number_repr.size(), &bignum_);
}

void big_number::set_data(data_chunk data)
{
    size_t size = data.size();
    // BIGNUM's byte stream format expects 4 bytes of
    // big endian size data info at the front
    data.insert(data.begin(), (size >> 24) & 0xff);
    data.insert(data.begin() + 1, (size >> 16) & 0xff);
    data.insert(data.begin() + 2, (size >> 8) & 0xff);
    data.insert(data.begin() + 3, (size >> 0) & 0xff);
    BN_mpi2bn(&data[0], data.size(), &bignum_);
}

data_chunk big_number::get_data()
{
    size_t size = BN_bn2mpi(&bignum_, NULL);
    if (size < 4)
        return data_chunk();
    data_chunk data(size);
    BN_bn2mpi(&bignum_, &data[0]);
    data.erase(data.begin(), data.begin() + 4);
    return data;
}

void big_number::set_hash(hash_digest hash)
{
    data_chunk hash_data(hash.size());
    std::copy(hash.begin(), hash.end(), hash_data.begin());
    set_data(hash_data);
}

hash_digest big_number::get_hash()
{
    hash_digest repr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    data_chunk data = get_data();
    if (data.size() < 4)
        return repr;
    // Copy up to end, not from start to midway
    std::copy(data.begin(), data.end(), repr.end() - data.size());
    return repr;
}

bool big_number::operator==(const big_number& other) 
{ 
    return (BN_cmp(&bignum_, &other.bignum_) == 0); 
}
bool big_number::operator!=(const big_number& other) 
{ 
    return (BN_cmp(&bignum_, &other.bignum_) != 0); 
}
bool big_number::operator<=(const big_number& other) 
{ 
    return (BN_cmp(&bignum_, &other.bignum_) <= 0); 
}
bool big_number::operator>=(const big_number& other) 
{ 
    return (BN_cmp(&bignum_, &other.bignum_) >= 0); 
}
bool big_number::operator<(const big_number& other)  
{ 
    return (BN_cmp(&bignum_, &other.bignum_) < 0); 
}
bool big_number::operator>(const big_number& other)  
{ 
    return (BN_cmp(&bignum_, &other.bignum_) > 0); 
}

} // libbitcoin

