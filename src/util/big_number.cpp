#include <bitcoin/util/big_number.hpp>

namespace libbitcoin {

class big_number_context
{
public:
    big_number_context();
    ~big_number_context();
    BN_CTX* context();
private:
    BN_CTX* ctx_;
};

big_number_context::big_number_context()
{
    ctx_ = BN_CTX_new();
}

big_number_context::~big_number_context()
{
    BN_CTX_free(ctx_);
}

BN_CTX* big_number_context::context()
{
    return ctx_;
}

//------------

big_number::big_number()
{
    BN_init(&bignum_);
}
big_number::big_number(uint64_t value)
{
    BN_init(&bignum_);
    set_uint64(value);
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

uint32_t big_number::get_compact() const
{
    size_t size = BN_bn2mpi(&bignum_, NULL);
    data_chunk number_repr(size);
    size -= 4;
    BN_bn2mpi(&bignum_, &number_repr[0]);
    uint32_t compact = size << 24;
    if (size >= 1) 
        compact |= (number_repr[4] << 16);
    if (size >= 2) 
        compact |= (number_repr[5] << 8);
    if (size >= 3) 
        compact |= (number_repr[6] << 0);
    return compact;
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

data_chunk big_number::get_data() const
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

hash_digest big_number::get_hash() const
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
    return BN_cmp(&bignum_, &other.bignum_) == 0; 
}
bool big_number::operator!=(const big_number& other) 
{ 
    return BN_cmp(&bignum_, &other.bignum_) != 0; 
}
bool big_number::operator<=(const big_number& other) 
{ 
    return BN_cmp(&bignum_, &other.bignum_) <= 0; 
}
bool big_number::operator>=(const big_number& other) 
{ 
    return BN_cmp(&bignum_, &other.bignum_) >= 0; 
}
bool big_number::operator<(const big_number& other)  
{ 
    return BN_cmp(&bignum_, &other.bignum_) < 0; 
}
bool big_number::operator>(const big_number& other)  
{ 
    return BN_cmp(&bignum_, &other.bignum_) > 0; 
}

big_number& big_number::operator*=(const big_number& other)
{
    big_number_context ctx;
    BN_mul(&bignum_, &bignum_, &other.bignum_, ctx.context());
    return *this;
}

big_number& big_number::operator/=(const big_number& other)
{
    big_number_context ctx;
    BN_div(&bignum_, nullptr, &bignum_, &other.bignum_, ctx.context());
    return *this;
}

void big_number::set_uint64(uint64_t value)
{
    // TODO: figure out WTF this does then replace with my own code
    // I doubt all this code is needed.
    unsigned char pch[sizeof(value) + 6];
    unsigned char* p = pch + 4;
    bool fLeadingZeroes = true;
    for (int i = 0; i < 8; i++)
    {
        unsigned char c = (value >> 56) & 0xff;
        value <<= 8;
        if (fLeadingZeroes)
        {
            if (c == 0)
                continue;
            if (c & 0x80)
                *p++ = 0;
            fLeadingZeroes = false;
        }
        *p++ = c;
    }
    unsigned int nSize = p - (pch + 4);
    pch[0] = (nSize >> 24) & 0xff;
    pch[1] = (nSize >> 16) & 0xff;
    pch[2] = (nSize >> 8) & 0xff;
    pch[3] = (nSize) & 0xff;
    BN_mpi2bn(pch, p - pch, &bignum_);
}

} // libbitcoin

