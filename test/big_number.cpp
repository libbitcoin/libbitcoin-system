/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef ENABLE_DATAGEN

#include "big_number.hpp"
#include <openssl/bn.h>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

// DEPRECATED: ONLY FOR TEST VALIDATION OF OPENSSL REMOVAL.
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
big_number::big_number(uint32_t value)
{
    initialize();
    set_uint32(value);
}
big_number::big_number(const big_number& other)
{
    initialize();
    copy(other);
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

// The "compact" format is a representation of a whole
// number N using an unsigned 32bit number similar to a
// floating point format.
// The most significant 8 bits are the unsigned exponent of base 256.
// This exponent can be thought of as "number of bytes of N".
// The lower 23 bits are the mantissa.
// Bit number 24 (0x800000) represents the sign of N.
// N = (-1^sign) * mantissa * 256^(exponent-3)
//
// Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
// MPI uses the most significant bit of the first byte as sign.
// Thus 0x1234560000 is compact (0x05123456)
// and  0xc0de000000 is compact (0x0600c0de)
// (0x05c0de00) would be -0x40de000000
//
// Bitcoin only uses this "compact" format for encoding difficulty
// targets, which are unsigned 256bit quantities.  Thus, all the
// complexities of the sign bit and using base 256 are probably an
// implementation accident.
//
// This implementation directly uses shifts instead of going
// through an intermediate MPI representation.

void big_number::set_compact(uint32_t compact)
{
    uint32_t size = compact >> 24;
    bool is_negative = (compact & 0x00800000) != 0;
    uint32_t word = compact & 0x007fffff;
    if (size <= 3)
    {
        word >>= 8 * (3 - size);
        BN_set_word(&bignum_, word);
    }
    else
    {
        BN_set_word(&bignum_, word);
        BN_lshift(&bignum_, &bignum_, 8 * (size - 3));
    }
    BN_set_negative(&bignum_, is_negative);
}
uint32_t big_number::compact() const
{
    uint32_t size = BN_num_bytes(&bignum_);
    uint32_t compact = 0;
    if (size <= 3)
        compact = BN_get_word(&bignum_) << 8 * (3 - size);
    else
    {
        big_number tmp_number;
        BN_rshift(&tmp_number.bignum_, &bignum_, 8 * (size - 3));
        compact = BN_get_word(&tmp_number.bignum_);
    }
    // The 0x00800000 bit denotes the sign.
    // Thus, if it is already set, divide the mantissa by 256
    // and increase the exponent.
    if (compact & 0x00800000)
    {
        compact >>= 8;
        size++;
    }
    compact |= size << 24;
    compact |= (BN_is_negative(&bignum_) ? 0x00800000 : 0);
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
    BN_mpi2bn(&load_data[0], (int)load_data.size(), &bignum_);
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
    hash_digest repr = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    // Is this safe in 64 bit?
    return BN_get_word(&bignum_);
}

void big_number::set_int32(int32_t value)
{
    if (value >= 0)
        set_uint32(value);
    else
        set_int64(value);
}

int32_t big_number::int32() const
{
    BN_ULONG value = BN_get_word(&bignum_);
    if (!BN_is_negative(&bignum_))
    {
        if (value > max_int32)
            return max_int32;
        return value;
    }
    else
    {
        if (value > max_int32)
            return std::numeric_limits<int32_t>::min();
        // MSVC warning C4146: unary minus operator applied to unsigned type,
        // result still unsigned.
        return -value;
    }
}

void big_number::set_uint64(uint64_t value)
{
    set_uint64_impl(value, false);
}

void big_number::set_int64(int64_t value)
{
    bool is_negative;
    uint64_t flat_value;

    if (value < 0)
    {
        // Since the minimum signed integer cannot be represented as
        // positive so long as its type is signed, and it's not well-defined
        // what happens if you make it unsigned before negating it, we
        // instead increment the negative integer by 1, convert it, then
        // increment the (now positive) unsigned integer by 1 to compensate.
        flat_value = -(value + 1);
        ++flat_value;
        is_negative = true;
    }
    else
    {
        flat_value = value;
        is_negative = false;
    }

    set_uint64_impl(flat_value, is_negative);
}

void big_number::set_uint64_impl(uint64_t value, bool is_negative)
{
    // Numbers are represented in OpenSSL using the MPI format.
    // 4 byte length
    uint8_t raw_mpi[sizeof(value) + 6];
    uint8_t* curr_byte = &raw_mpi[4];
    bool leading_zeros = true;
    for (int i = 0; i < 8; ++i)
    {
        uint8_t c = (value >> 56) & 0xff;
        value <<= 8;
        if (leading_zeros)
        {
            // Skip beginning zeros
            if (c == 0)
                continue;
            if (c & 0x80)
            {
                *curr_byte = (is_negative ? 0x80 : 0);
                ++curr_byte;
            }
            else if (is_negative)
                c |= 0x80;
            leading_zeros = false;
        }
        *curr_byte = c;
        ++curr_byte;
    }
    uint32_t size = static_cast<uint32_t>(curr_byte - (raw_mpi + 4));
    raw_mpi[0] = (size >> 24) & 0xff;
    raw_mpi[1] = (size >> 16) & 0xff;
    raw_mpi[2] = (size >> 8) & 0xff;
    raw_mpi[3] = (size) & 0xff;
    BN_mpi2bn(raw_mpi, (int)(curr_byte - raw_mpi), &bignum_);
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

big_number& big_number::operator-=(const big_number& other)
{
    BN_sub(&bignum_, &bignum_, &other.bignum_);
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

const big_number operator-(const big_number& a, const big_number& b)
{
    big_number result;
    BN_sub(&result.bignum_, &a.bignum_, &b.bignum_);
    return result;
}

const big_number operator-(const big_number& number)
{
    big_number result(number);
    BN_set_negative(&result.bignum_, !BN_is_negative(&result.bignum_));
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

#endif