/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_SIGHASH_HPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_SIGHASH_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

enum sighash : uint32_t
{
    all = 1,
    none = 2,
    single = 3,
    anyone_can_pay = 0x80
};

}

#endif
