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
#ifndef LIBBBITCOIN__MATH_UINT256_T_HPP
#define LIBBBITCOIN__MATH_UINT256_T_HPP

#include <bitcoin/bitcoin/math/hash.hpp>
//#include <boost/multiprecision/cpp_int.hpp>

namespace libbitcoin {
namespace api {

class math_uint256_t {
public:
	math_uint256_t() :
			value_(new uint256_t) {
	}

//	math_uint256_t(long double value) :
//			value_(new uint256_t(value)) {
//	}

	math_uint256_t(uint256_t* value) {
		value_ = value;
	}

//	bool operator>(const long double value);
//	bool operator>(math_uint256_t value);

	uint256_t* getValue() {
		return value_;
	}

	void setValue(uint256_t* value) {
		value_ = value;
	}
private:
	uint256_t* value_;

};

} // namespace api
} // namespace libbitcoin

#endif
