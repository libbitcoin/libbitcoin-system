/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/concurrency/interprocess_lock.hpp>

#include <string>
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <bitcoin/system/unicode/conversion.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_ofstream.hpp>


namespace libbitcoin {
namespace system {

using namespace boost::interprocess;
static const auto invalid = ipcdetail::invalid_file();

// local
// ----------------------------------------------------------------------------

#ifdef _MSC_VER

static file_handle_t open_file(const std::string& name)
{
    // This is why we're here.
    const auto filename = to_utf16(name).c_str();

    static const auto access = FILE_FLAG_OVERLAPPED | FILE_FLAG_WRITE_THROUGH;
    static const auto mode = FILE_SHARE_READ | FILE_SHARE_WRITE |
        FILE_SHARE_DELETE;

    for (auto attempt = 0; attempt < 3u; ++attempt)
    {
        const auto handle = CreateFileW(filename, access, mode, NULL,
            OPEN_EXISTING, 0u, NULL);
        
        if (handle != INVALID_HANDLE_VALUE)
            return handle;

        if (GetLastError() != ERROR_SHARING_VIOLATION)
            return handle;

        Sleep(250);
    }

    return INVALID_HANDLE_VALUE;
}

#else

static file_handle_t open_file(const std::string& name)
{
    return ipcdetail::open_existing_file(name.c_str(), read_write);
}

#endif

// construct/destruct
// ----------------------------------------------------------------------------

interprocess_lock::interprocess_lock(const path& file)
  : filename_(file.string()), handle_(invalid)
{
    // This is a behavior change, we no longer open file (or throw) here.
}

interprocess_lock::~interprocess_lock()
{
    unlock();
}

// public
// ----------------------------------------------------------------------------

// Lock is not idempotent, returns false if already locked (or error).
// This succeeds if no other process has exclusive or sharable ownership.
bool interprocess_lock::lock()
{
    if (handle_ != invalid)
        return false;

    // An easy way to create the file.
    ofstream stream(filename_);
    if (!stream.good())
        return false;

    // Close the stream now that the file exists.
    stream.close();

    // Get a handle to the file.
    const auto handle = open_file(filename_);

    // Obtain exclusive access to the file.
    bool result;
    if (ipcdetail::try_acquire_file_lock(handle, result) && result)
    {
        handle_ = handle;
        return true;
    }

    handle_ = invalid;
    return false;
}

// Unlock is idempotent, returns true if unlocked on return (or success).
// This may leave the lock file behind, which is not a problem.
bool interprocess_lock::unlock()
{
    if (handle_ == invalid)
        return true;

    // Release exclusive access to the file.
    ipcdetail::close_file(handle_);
    handle_ = invalid;

    // Delete file (boost path should have been configured for utf8).
    return boost::filesystem::remove(filename_);
}

} // namespace system
} // namespace libbitcoin
