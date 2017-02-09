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
#include <bitcoin/bitcoin/unicode/ifstream.hpp>

#include <fstream>
#include <string>
#include <bitcoin/bitcoin/unicode/unicode.hpp>

namespace libbitcoin {

// Construct bc::ifstream.
ifstream::ifstream(const std::string& path, std::ifstream::openmode mode)
#ifdef _MSC_VER
  : std::ifstream(bc::to_utf16(path), mode)
#else
  : std::ifstream(path, mode)
#endif
{
}

} // namespace libbitcoin
