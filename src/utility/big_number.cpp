#include <bitcoin/utility/big_number.hpp>

#include <bitcoin/format.hpp>

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
    initialize();
}
big_number::big_number(const big_number& other)
{
    initialize();
    copy(other);
}
big_number::big_number(uint32_t value)
{
    initialize();
    set_uint32(value);
}
big_number::~big_number()
{
    BN_clear_free(&bignum_);
}

void big_number::initialize()
{
    BN_init(&bignum_);
}

void big_number::copy(const big_number& other)
{
    BN_copy(&bignum_, &other.bignum_);
}

big_number& big_number::operator=(const big_number& other)
{
    copy(other);
    return *this;
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

uint32_t big_number::compact() const
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

void big_number::set_data(data_chunk load_data)
{
    // sigh...
    std::reverse(load_data.begin(), load_data.end());
    size_t size = load_data.size();
    // BIGNUM's byte stream format expects 4 bytes of
    // big endian size data info at the front
    load_data.insert(load_data.begin(), (size >> 24) & 0xff);
    load_data.insert(load_data.begin() + 1, (size >> 16) & 0xff);
    load_data.insert(load_data.begin() + 2, (size >> 8) & 0xff);
    load_data.insert(load_data.begin() + 3, (size >> 0) & 0xff);
    BN_mpi2bn(&load_data[0], load_data.size(), &bignum_);
}

data_chunk big_number::data() const
{
    size_t size = BN_bn2mpi(&bignum_, nullptr);
    if (size <= 4)
        return data_chunk();
    data_chunk result_data(size);
    BN_bn2mpi(&bignum_, &result_data[0]);
    result_data.erase(result_data.begin(), result_data.begin() + 4);
    std::reverse(result_data.begin(), result_data.end());
    return result_data;
}

void big_number::set_hash(hash_digest load_hash)
{
    data_chunk hash_data(load_hash.size());
    std::reverse_copy(load_hash.begin(), load_hash.end(), hash_data.begin());
    set_data(hash_data);
}

hash_digest big_number::hash() const
{
    hash_digest repr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    data_chunk copy_data = data();
    if (copy_data.size() < 4)
        return repr;
    // Copy up to end, not from start to midway
    std::reverse_copy(copy_data.begin(), copy_data.end(),
        repr.end() - copy_data.size());
    return repr;
}

void big_number::set_uint32(uint32_t value)
{
    BN_set_word(&bignum_, value);
}

uint32_t big_number::uint32() const
{
    return BN_get_word(&bignum_);
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

big_number& big_number::operator+=(const big_number& other)
{
    BN_add(&bignum_, &bignum_, &other.bignum_);
    return *this; 
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

const big_number operator+(const big_number& a, const big_number& b)
{
    big_number result;
    BN_add(&result.bignum_, &a.bignum_, &b.bignum_);
    return result;
}
const big_number operator/(const big_number& a, const big_number& b)
{
    return divmod(a, b).first;
}
const big_number operator<<(const big_number& a, unsigned int shift)
{
    big_number result;
    BN_lshift(&result.bignum_, &a.bignum_, shift);
    return result;
}

divmod_result divmod(const big_number& a, const big_number& b)
{
    big_number divider;
    big_number remainder;
    big_number_context ctx;
    BN_div(&divider.bignum_, &remainder.bignum_, 
        &a.bignum_, &b.bignum_, ctx.context());
    return std::make_pair(divider, remainder);
}

} // namespace libbitcoin

