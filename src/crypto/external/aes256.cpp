/**
 *   Byte-oriented AES-256 implementation.
 *   All lookup tables replaced with 'on the fly' calculations. 
 *
 *   Copyright (c) 2007-2009 Ilya O. Levin, http://www.literatecode.com
 *   Other contributors: Hal Finney
 *
 *   Permission to use, copy, modify, and distribute this software for any
 *   purpose with or without fee is hereby granted, provided that the above
 *   copyright notice and this permission notice appear in all copies.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <bitcoin/system/crypto/external/aes256.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace aes256 {

struct context
{
    secret key;
    secret enckey;
    secret deckey;
};

constexpr size_t rounds = 14;

constexpr std::array<uint8_t, to_bits(secret_size)> sbox
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

constexpr std::array<uint8_t, to_bits(secret_size)> sbox_inverse
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

constexpr uint8_t f_enc_key(uint8_t byte) NOEXCEPT
{
    return shift_left(byte) ^ (get_left(byte) ? 0b0001'1011_u8 : zero);
}

constexpr uint8_t f_dec_key(uint8_t byte) NOEXCEPT
{
    return shift_right(byte) ^ (get_right(byte) ? 0b1000'1101_u8 : zero);
}

BC_PUSH_WARNING(USE_GSL_AT)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr void sub_bytes(block& bytes) NOEXCEPT
{
    auto i = block_size;
    while (to_bool(i--))
        bytes[i] = sbox[bytes[i]];
}

constexpr void sub_bytes_inverse(block& bytes) NOEXCEPT
{
    auto i = block_size;
    while (to_bool(i--))
        bytes[i] = sbox_inverse[bytes[i]];
}

constexpr void add_round_key_lower(block& bytes, secret& key) NOEXCEPT
{
    auto i = block_size;
    while (to_bool(i--))
        bytes[i] ^= key[i];
}

constexpr void add_round_key_upper(block& bytes, secret& key) NOEXCEPT
{
    auto i = block_size;
    while (to_bool(i--))
        bytes[i] ^= key[i + block_size];
}

constexpr void add_round_key_copy(block& bytes, secret& key,
    secret& copy) NOEXCEPT
{
    auto i = block_size;
    while (to_bool(i--))
    {
        const auto j = i + block_size;
        copy[j] = key[j];
        copy[i] = key[i];
        bytes[i] ^= key[i];
    }
}

constexpr void shift_rows(block& bytes) NOEXCEPT
{
    uint8_t i, j;

    i = bytes[1];
    bytes[ 1] = bytes[5];
    bytes[ 5] = bytes[9];
    bytes[ 9] = bytes[13];
    bytes[13] = i;

    i = bytes[10];
    bytes[10] = bytes[2];
    bytes[ 2] = i;

    j = bytes[3];
    bytes[ 3] = bytes[15];
    bytes[15] = bytes[11];
    bytes[11] = bytes[7];
    bytes[ 7] = j;

    j = bytes[14];
    bytes[14] = bytes[6];
    bytes[ 6] = j;
}

constexpr void shift_rows_inverse(block& bytes) NOEXCEPT
{
    uint8_t i, j;

    i = bytes[1];
    bytes[ 1] = bytes[13];
    bytes[13] = bytes[9];
    bytes[ 9] = bytes[5];
    bytes[ 5] = i;

    i = bytes[2];
    bytes[ 2] = bytes[10];
    bytes[10] = i;

    j = bytes[3];
    bytes[ 3] = bytes[7];
    bytes[ 7] = bytes[11];
    bytes[11] = bytes[15];
    bytes[15] = j;

    j = bytes[6];
    bytes[ 6] = bytes[14];
    bytes[14] = j;
}

constexpr void mix_columns(block& bytes) NOEXCEPT
{
    uint8_t a, b, c, d, e;

    for (size_t i = 0; i < block_size; i += 4_size)
    {
        a = bytes[i + 0];
        b = bytes[i + 1];
        c = bytes[i + 2];
        d = bytes[i + 3];

        e = a ^ b ^ c ^ d;
        bytes[i + 0] ^= e ^ f_enc_key(a ^ b);
        bytes[i + 1] ^= e ^ f_enc_key(b ^ c);
        bytes[i + 2] ^= e ^ f_enc_key(c ^ d);
        bytes[i + 3] ^= e ^ f_enc_key(d ^ a);
    }
}

constexpr void mix_columns_inverse(block& bytes) NOEXCEPT
{
    uint8_t a, b, c, d, e, x, y, z;

    for (size_t i = 0; i < block_size; i += 4_size)
    {
        a = bytes[i + 0];
        b = bytes[i + 1];
        c = bytes[i + 2];
        d = bytes[i + 3];

        e = a ^ b ^ c ^ d;
        z = f_enc_key(e);
        x = e ^ f_enc_key(f_enc_key(z ^ a ^ c));
        y = e ^ f_enc_key(f_enc_key(z ^ b ^ d));

        bytes[i + 0] ^= x ^ f_enc_key(a ^ b);
        bytes[i + 1] ^= y ^ f_enc_key(b ^ c);
        bytes[i + 2] ^= x ^ f_enc_key(c ^ d);
        bytes[i + 3] ^= y ^ f_enc_key(d ^ a);
    }
}

constexpr void expand_key(secret& key, uint8_t& round) NOEXCEPT
{
    key[0] ^= sbox[key[29]] ^ round;
    key[1] ^= sbox[key[30]];
    key[2] ^= sbox[key[31]];
    key[3] ^= sbox[key[28]];
    round = f_enc_key(round);

    for (size_t i = 4; i < 16_size; i += 4_size)
    {
        key[i + 0] ^= key[i - 4];
        key[i + 1] ^= key[i - 3];
        key[i + 2] ^= key[i - 2];
        key[i + 3] ^= key[i - 1];
    }

    key[16] ^= sbox[key[12]];
    key[17] ^= sbox[key[13]];
    key[18] ^= sbox[key[14]];
    key[19] ^= sbox[key[15]];

    for (size_t i = 20; i < 32_size; i += 4_size)
    {
        key[i + 0] ^= key[i - 4];
        key[i + 1] ^= key[i - 3];
        key[i + 2] ^= key[i - 2];
        key[i + 3] ^= key[i - 1];
    }
}

constexpr void expand_key_inverse(secret& key, uint8_t& round) NOEXCEPT
{
    for (size_t i = 28; i > 16_size; i -= 4_size)
    {
        key[i + 0] ^= key[i - 4];
        key[i + 1] ^= key[i - 3];
        key[i + 2] ^= key[i - 2];
        key[i + 3] ^= key[i - 1];
    }

    key[16] ^= sbox[key[12]];
    key[17] ^= sbox[key[13]];
    key[18] ^= sbox[key[14]];
    key[19] ^= sbox[key[15]];

    for (size_t i = 12; i > 0_size; i -= 4_size)
    {
        key[i + 0] ^= key[i - 4];
        key[i + 1] ^= key[i - 3];
        key[i + 2] ^= key[i - 2];
        key[i + 3] ^= key[i - 1];
    }

    round = f_dec_key(round);
    key[0] ^= sbox[key[29]] ^ round;
    key[1] ^= sbox[key[30]];
    key[2] ^= sbox[key[31]];
    key[3] ^= sbox[key[28]];
}

BC_POP_WARNING()
BC_POP_WARNING()

constexpr void initialize(aes256::context& context, const secret& key) NOEXCEPT
{
    context.deckey = key;
    context.enckey = key;

    auto round = bit_lo<uint8_t>;
    for (size_t i = 0; i < sub1(bits<uint8_t>); ++i)
        expand_key(context.deckey, round);
}

constexpr void encrypt_block(aes256::context& context, block& bytes) NOEXCEPT
{
    add_round_key_copy(bytes, context.enckey, context.key);

    auto round = bit_lo<uint8_t>;
    for (size_t i = 0; i < sub1(rounds); ++i)
    {
        sub_bytes(bytes);
        shift_rows(bytes);
        mix_columns(bytes);

        if (is_even(i))
        {
            add_round_key_upper(bytes, context.key);
        }
        else
        {
            expand_key(context.key, round);
            add_round_key_lower(bytes, context.key);
        }
    }

    sub_bytes(bytes);
    shift_rows(bytes);
    expand_key(context.key, round);
    add_round_key_lower(bytes, context.key);
}

constexpr void decrypt_block(aes256::context& context, block& bytes) NOEXCEPT
{
    add_round_key_copy(bytes, context.deckey, context.key);

    shift_rows_inverse(bytes);
    sub_bytes_inverse(bytes);

    auto round = bit_hi<uint8_t>;
    for (size_t i = 0; i < sub1(rounds); ++i)
    {
        if (is_even(i))
        {
            expand_key_inverse(context.key, round);
            add_round_key_upper(bytes, context.key);
        }
        else
        {
            add_round_key_lower(bytes, context.key);
        }

        mix_columns_inverse(bytes);
        shift_rows_inverse(bytes);
        sub_bytes_inverse(bytes);
    }

    add_round_key_lower(bytes, context.key);
}

////constexpr void zeroize(aes256::context& context) NOEXCEPT
////{
////    context.key.fill(0);
////    context.enckey.fill(0);
////    context.deckey.fill(0);
////}

// published
// ----------------------------------------------------------------------------

void encrypt(block& bytes, const secret& key) NOEXCEPT
{
    aes256::context context;
    initialize(context, key);
    encrypt_block(context, bytes);
    ////zeroize(context);
}

void decrypt(block& bytes, const secret& key) NOEXCEPT
{
    aes256::context context;
    initialize(context, key);
    decrypt_block(context, bytes);
    ////zeroize(context);
}

} // namespace aes256
} // namespace system
} // namespace libbitcoin
