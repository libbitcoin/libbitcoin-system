/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN__UTILITY_NONCOPYABLE_HPP
#define LIBBITCOIN__UTILITY_NONCOPYABLE_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/noncopyable.hpp>

namespace libbitcoin {
namespace api {

class BC_API utility_noncopyable
{
public:
    utility_noncopyable(const utility_noncopyable&) = delete;
    void operator=(const utility_noncopyable&) = delete;

public:
    noncopyable* getValue() {
        return value_;
    }

    void setValue(noncopyable* value) {
        value_ = value;
    }
//protected:
//    utility_noncopyable()
//    {
//    }
private:
    noncopyable* value_;

};

} // namespace api
} // namespace libbitcoin

#endif
