/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ERROR_ERROR_HPP
#define LIBBITCOIN_SYSTEM_ERROR_ERROR_HPP

#include <bitcoin/system/error/block_error_t.hpp>
#include <bitcoin/system/error/error_t.hpp>
#include <bitcoin/system/error/errorno_t.hpp>
#include <bitcoin/system/error/macros.hpp>
#include <bitcoin/system/error/op_error_t.hpp>
#include <bitcoin/system/error/script_error_t.hpp>
#include <bitcoin/system/error/transaction_error_t.hpp>

#include <system_error>

namespace libbitcoin {
namespace system {

using code = std::error_code;

} // namespace system
} // namespace libbitcoin

#endif
