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
#ifndef LIBBITCOIN_MMFILE_HPP
#define LIBBITCOIN_MMFILE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/define.hpp>

namespace libbitcoin {

class mmfile
{
public:
    BC_API mmfile(const std::string& filename);
    BC_API ~mmfile();
    BC_API uint8_t* data();
    BC_API const uint8_t* data() const;
    BC_API size_t size() const;
private:
    int file_handle_ = 0;
    uint8_t* data_ = nullptr;
    size_t size_;
};

} // namespace libbitcoin

#endif
