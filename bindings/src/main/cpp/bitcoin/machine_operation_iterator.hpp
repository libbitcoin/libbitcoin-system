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
#ifndef LIBBITCOIN__MACHINE_OPERATION_ITERATOR_HPP
#define LIBBITCOIN__MACHINE_OPERATION_ITERATOR_HPP

#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace machine {
namespace api {

class BC_API machine_operation_iterator
{
public:

	machine_operation_iterator()
	: value(new machine::operation::iterator())
	{
	}

	machine::operation::iterator getValue() {
		return machine_operation_iterator::value;
	}

	void setValue(machine::operation::iterator t) {
		machine_operation_iterator::value = t;
	}

	machine::operation::iterator getvalue() {
		return value;
	}

//	size_t getSize() {
//		return value.size();
//	}

private:

	 machine::operation::iterator value;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif
