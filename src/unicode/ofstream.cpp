/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/unicode/ofstream.hpp>

#include <fstream>
#include <string>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {

// Construct bc::system::ofstream.
ofstream::ofstream(const std::string& path, std::ofstream::openmode mode)
#ifdef _MSC_VER
  : std::ofstream(system::to_utf16(path), mode)
#else
  : std::ofstream(path, mode)
#endif
{
}

} // namespace system
} // namespace libbitcoin
