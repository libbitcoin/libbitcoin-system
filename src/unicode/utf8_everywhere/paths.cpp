/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/unicode/utf8_everywhere/environment.hpp>

#ifdef HAVE_MSC
    #include <shlobj.h>
    #include <windows.h>
#endif
#include <filesystem>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/conversion.hpp>

namespace libbitcoin {
namespace system {

#ifdef HAVE_MSC
static std::string config_directory() NOEXCEPT
{
    wchar_t directory[MAX_PATH];
    const auto result = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL,
        SHGFP_TYPE_CURRENT, &directory[0]);
    return SUCCEEDED(result) ? to_utf8(&directory[0]) : "";
}
#elif defined(SYSCONFDIR)
static std::string config_directory() NOEXCEPT { return SYSCONFDIR; }
#else
static std::string config_directory() NOEXCEPT { return SYSCONFDIR; }
#endif

std::filesystem::path default_config_path(
    const std::filesystem::path& subdirectory) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    static const auto directory = std::filesystem::path{ config_directory() };
    return directory / subdirectory;
    BC_POP_WARNING()
}

#ifdef HAVE_MSC

// docs.microsoft.com/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew
static std::wstring to_fully_qualified_path(
    const std::filesystem::path& path) NOEXCEPT
{
    const auto replace_all = [](std::string text, char from, char to) NOEXCEPT
    {
        for (auto position = text.find(from); position != std::string::npos;
            position = text.find(from, position + sizeof(char)))
        {
            text.replace(position, sizeof(char), { to });
        }

        return text;
    };

    // Separator normalization required by use of length extender.
    const auto normal = to_utf16(replace_all(path.string(), '/', '\\'));

    // GetFullPathName is not thread safe. If another thread calls
    // SetCurrentDirectory during a call of GetFullPathName the value may be
    // corrupted as process-static storage is used to retain the directory.
    auto size = GetFullPathNameW(normal.c_str(), 0, NULL, NULL);
    if (is_zero(size))
        return {};

    // Despite contradictory documentation, this accepts long relative paths
    // and converts them to fully-qualified, without an extension prefix.
    // This also converts "considered relative" paths (with ".." segments).
    // Add the prefix after calling as required in order to use a long path.
    std::vector<wchar_t> directory(size);
    size = GetFullPathNameW(normal.c_str(), size, directory.data(), NULL);
    if (is_zero(size))
        return {};

    // The returned size does not include the null terminator, and cannot
    // exceed the original, but does become smaller, so resize accordingly.
    return { directory.begin(), std::next(directory.begin(), size) };
}

// Use to_extended_path with APIs that compile to wide with HAVE_MSC defined
// and to UTF8 with HAVE_MSC undefined. This includes some boost APIs and some
// Win32 API extensions to std libs - such as std::ofstream and std::ifstream.
// Otherwise use in any Win32 (W) APIs with HAVE_MSC defined, such as we do in 
// interprocess_lock::open_file -> CreateFileW, since the boost wrapper only
// calls CreateFileA. The length extension prefix requires Win32 (W) APIs.
std::wstring to_extended_path(const std::filesystem::path& path) NOEXCEPT
{
    // The length extension prefix works only with a fully-qualified path.
    // However this includes "considered relative" paths (with ".." segments).
    // That is of no consequence here because those will also be converted.
    const auto full = to_fully_qualified_path(path);
    return (full.length() > MAX_PATH) ? L"\\\\?\\" + full : full;
}

#else

std::string to_extended_path(const std::filesystem::path& path) NOEXCEPT
{
    return path.string();
}

#endif // HAVE_MSC

} // namespace system
} // namespace libbitcoin
