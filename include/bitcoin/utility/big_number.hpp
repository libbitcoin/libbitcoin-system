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
#ifndef LIBBITCOIN_BIG_NUMBER_HPP
#define LIBBITCOIN_BIG_NUMBER_HPP

#include <openssl/bn.h>
#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

class big_number;
typedef std::pair<big_number, big_number> divmod_result;

// TODO: Lots of things *should* throw on error in this class
class big_number
{
public:
    BC_API big_number();
    BC_API big_number(uint32_t value);
    BC_API big_number(const big_number& other);
    BC_API ~big_number();

    BC_API big_number& operator=(const big_number& other);

    BC_API void set_compact(uint32_t compact);
    BC_API uint32_t compact() const;

    BC_API void set_data(data_chunk load_data);
    BC_API data_chunk data() const;

    BC_API void set_hash(hash_digest load_hash);
    BC_API hash_digest hash() const;

    BC_API void set_uint32(uint32_t value);
    BC_API uint32_t uint32() const;

    BC_API void set_int32(int32_t value);
    BC_API int32_t int32() const;

    // Used by script system
    BC_API void set_uint64(uint64_t value);
    BC_API void set_int64(int64_t value);

    BC_API bool operator==(const big_number& other);
    BC_API bool operator!=(const big_number& other);
    BC_API bool operator<=(const big_number& other);
    BC_API bool operator>=(const big_number& other);
    BC_API bool operator<(const big_number& other);
    BC_API bool operator>(const big_number& other);

    BC_API big_number& operator+=(const big_number& other);
    BC_API big_number& operator-=(const big_number& other);
    BC_API big_number& operator*=(const big_number& other);
    BC_API big_number& operator/=(const big_number& other);

private:
    BC_API friend divmod_result divmod(const big_number& a, const big_number& b);
    BC_API friend const big_number operator+(
        const big_number& a, const big_number& b);
    BC_API friend const big_number operator-(
        const big_number& a, const big_number& b);
    BC_API friend const big_number operator-(const big_number& number);
    BC_API friend const big_number operator/(const big_number& a,
        const big_number& b);
    BC_API friend const big_number operator<<(const big_number& a,
        unsigned int shift);

    void initialize();
    void copy(const big_number& other);

    void set_uint64_impl(uint64_t value, bool is_negative);

    BIGNUM bignum_;
};

BC_API const big_number operator+(const big_number& a, const big_number& b);
BC_API const big_number operator-(const big_number& a, const big_number& b);
BC_API const big_number operator-(const big_number& number);
BC_API const big_number operator/(const big_number& a, const big_number& b);
BC_API const big_number operator<<(const big_number& a, unsigned int shift);

BC_API divmod_result divmod(const big_number& a, const big_number& b);

} // namespace libbitcoin

#endif

