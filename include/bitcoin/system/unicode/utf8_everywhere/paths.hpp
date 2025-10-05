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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_PATHS_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_PATHS_HPP

#include <filesystem>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// std::filesystem::path maintains path internally as unicode, constructing
/// from the current ANSI code page on Win32. This must be avoided when using
/// utf8-everywhere. These utilities simplify proper contruction and extraction.
BC_API std::string cast_to_string(const std::u8string& value) NOEXCEPT;
BC_API std::u8string cast_to_u8string(const std::string& value) NOEXCEPT;
BC_API std::string from_path(const std::filesystem::path& value) NOEXCEPT;
BC_API std::filesystem::path to_path(const std::string& value) NOEXCEPT;

/// Get the default configuration file path with subdirectory.
BC_API std::filesystem::path default_config_path(
    const std::filesystem::path& subdirectory) NOEXCEPT;

/// Extend the path and return it as another.
/// std::filesystem::path does not extend long paths, so we provide this.
/// Not thread safe. If another thread calls SetCurrentDirectory during this
/// call, the result may be corrupted as static storage is used for directory.
BC_API std::filesystem::path extended_path(
    const std::filesystem::path& path) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
