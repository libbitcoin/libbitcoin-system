/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_PATHS_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_PATHS_HPP

#include <filesystem>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Get the default configuration file path with subdirectory.
BC_API std::filesystem::path default_config_path(
    const std::filesystem::path& subdirectory) NOEXCEPT;

/// These allow std::*fstream(to_extended_path(std::filesystem::path)),
/// due to a vc++ fstream extension that accepts std::wstring path.

#ifdef HAVE_MSC

/// Convert to std::wstring path, extended for win32.
/// Not thread safe. If another thread calls SetCurrentDirectory during this
/// call, the result may be corrupted as static storage is used for directory.
BC_API std::wstring to_extended_path(
    const std::filesystem::path& path) NOEXCEPT;

#else

/// Return std::string path, for consistency with std::wstring version.
BC_API std::string to_extended_path(
    const std::filesystem::path& path) NOEXCEPT;

#endif

} // namespace system
} // namespace libbitcoin

#endif
