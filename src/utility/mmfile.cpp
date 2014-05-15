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
#include <bitcoin/utility/mmfile.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

mmfile::mmfile(const std::string& filename)
{
#ifndef _MSC_VER
    // Not yet MSVC portable (maybe windows).
    BITCOIN_ASSERT_MSG(sizeof (void*) == 8, "Not a 64bit system!");
    file_handle_ = open(filename.c_str(),
        O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (file_handle_ == -1)
        return;
    struct stat sbuf;
    if (fstat(file_handle_, &sbuf) == -1)
        return;
    size_ = sbuf.st_size;
    // You can static_cast void* pointers.
    data_ = static_cast<uint8_t*>(mmap(
        0, size_, PROT_READ | PROT_WRITE, MAP_SHARED, file_handle_, 0));
    if (data_ == MAP_FAILED)
        data_ = nullptr;
#endif
}
mmfile::~mmfile()
{
    munmap(data_, size_);
    close(file_handle_);
}

uint8_t* mmfile::data()
{
    return data_;
}
const uint8_t* mmfile::data() const
{
    return data_;
}
size_t mmfile::size() const
{
    return size_;
}

bool mmfile::resize(size_t new_size)
{
    // Resize underlying file.
    if (ftruncate(file_handle_, new_size) == -1)
        return false;
    // Readjust memory map.
    data_ = static_cast<uint8_t*>(mremap(
        data_, size_, new_size, MREMAP_MAYMOVE));
    if (data_ == MAP_FAILED)
        return false;
    size_ = new_size;
    return true;
}

} // namespace libbitcoin
