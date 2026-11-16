/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_POOL_LABEL_HPP
#define LIBBITCOIN_SYSTEM_POOL_LABEL_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/pool/algorithm.hpp>
#include <bitcoin/system/pool/report.hpp>

namespace libbitcoin {
namespace system {
namespace pool {

/// Each present cell labeled by the lowest row of its component, if any.
BC_API report_t label(const algorithm::columns_t& columns) NOEXCEPT;

} // namespace pool
} // namespace system
} // namespace libbitcoin

#endif
