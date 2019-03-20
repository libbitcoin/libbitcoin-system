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
#ifndef LIBBITCOIN__MACHINE_OPCODE_STATIC_HPP
#define LIBBITCOIN__MACHINE_OPCODE_STATIC_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/utility_assert.hpp>
#include <machine_opcode.hpp>
#include <p_machine_opcode.hpp>
//#include <bitcoin/utility_data.hpp>

namespace libbitcoin {
//namespace machine {
namespace api {

/// Convert the machine_opcode to a mnemonic string.
BC_API std::string machine_opcode_to_string(machine_opcode value, uint32_t active_forks);

/// Convert a string to an machine_opcode.
BC_API bool machine_opcode_from_string(p_machine_opcode& out_code, const std::string& value);

/// Convert any machine_opcode to a string hexadecimal representation.
BC_API std::string machine_opcode_to_hexadecimal(machine_opcode code);

/// Convert any hexadecimal byte to an machine_opcode.
BC_API bool machine_opcode_from_hexadecimal(p_machine_opcode& out_code,
    const std::string& value);

} // namespace api
//} // namespace machine
} // namespace libbitcoin

#endif
