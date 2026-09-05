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
#include <bitcoin/system/unicode/utf8_everywhere/paths.hpp>

#ifdef HAVE_MSC
    #include <shlobj.h>
    #include <windows.h>
#endif
#ifdef HAVE_LINUX
    #include <climits>
    #include <unistd.h>
#endif
#ifdef HAVE_APPLE
    #include <mach-o/dyld.h>
#endif
#include <filesystem>
#include <system_error>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/conversion.hpp>

namespace libbitcoin {
namespace system {

std::string cast_to_string(const std::u8string& value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return { reinterpret_cast<const char*>(value.c_str()) };
    BC_POP_WARNING()
}

std::u8string cast_to_u8string(const std::string& value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return { reinterpret_cast<const char8_t*>(value.c_str()) };
    BC_POP_WARNING()
}

std::string from_path(const std::filesystem::path& value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return cast_to_string(value.u8string());
    BC_POP_WARNING()
}

std::filesystem::path to_path(const std::string& value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return { cast_to_u8string(value) };
    BC_POP_WARNING()
}

#if defined(HAVE_MSC)
static std::wstring config_directory() NOEXCEPT
{
    wchar_t directory[MAX_PATH];
    const auto result = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL,
        SHGFP_TYPE_CURRENT, &directory[0]);
    return SUCCEEDED(result) ? &directory[0] : L"";
}
#elif defined(SYSCONFDIR)
static std::string config_directory() NOEXCEPT { return SYSCONFDIR; }
#else
static std::string config_directory() NOEXCEPT { return {}; }
#endif

std::filesystem::path default_config_path(
    const std::filesystem::path& subdirectory) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    static const auto folder = std::filesystem::path{ config_directory() };
    return folder / subdirectory;
    BC_POP_WARNING()
}

#if defined(HAVE_MSC)

// Helper for qualified_path.
inline auto replace_all(std::wstring text, wchar_t from, wchar_t to) NOEXCEPT
{
    for (auto position = text.find(from); position != std::string::npos;
        position = text.find(from, add1(position)))
    {
        text.replace(position, one, { to });
    }

    return text;
};

// docs.microsoft.com/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew
std::filesystem::path qualified_path(const std::filesystem::path& path) NOEXCEPT
{
    // Separator normalization required by use of length extender.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    const auto normal = replace_all(path.wstring(), '/', '\\');
    BC_POP_WARNING()

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
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return { directory.begin(), std::next(directory.begin(), size) };
    BC_POP_WARNING()
}

std::filesystem::path extended_path(const std::filesystem::path& path) NOEXCEPT
{
    // The length extension prefix works only with a fully-qualified path.
    // However this includes "considered relative" paths (with ".." segments).
    // That is of no consequence here because those will also be converted.
    // MAX_PATH includes the terminator, so a path of that length requires it.
    // An already extended path (e.g. by a shell) is not extended again.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    constexpr std::wstring_view prefix{ L"\\\\?\\" };
    const auto full = qualified_path(path).wstring();
    if (full.starts_with(prefix) || full.length() < MAX_PATH)
        return { full };

    return { std::wstring{ prefix } + full };
    BC_POP_WARNING()
}

std::filesystem::path module_path() NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    constexpr auto max_path = static_cast<DWORD>(MAX_PATH);
    BC_POP_WARNING()

    for (auto size = max_path; !is_zero(size); size *= 2u)
    {
        std::vector<wchar_t> buffer(size);
        const auto length = ::GetModuleFileNameW(NULL, buffer.data(), size);
        if (is_zero(length))
            break;

        if (length < size)
        {
            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            return { buffer.begin(), std::next(buffer.begin(), length) };
            BC_POP_WARNING()
        }
    }

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return {};
    BC_POP_WARNING()
}

#else

std::filesystem::path qualified_path(const std::filesystem::path& path) NOEXCEPT
{
    std::error_code ec{};
    const auto full = std::filesystem::absolute(path, ec);
    return ec ? path : full;
}

std::filesystem::path extended_path(const std::filesystem::path& path) NOEXCEPT
{
    return path;
}

std::filesystem::path module_path() NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
#if defined(HAVE_LINUX)
    // The link is not known to fit, so grow the buffer until it does.
    for (auto size = static_cast<size_t>(PATH_MAX); !is_zero(size); size *= 2u)
    {
        std::vector<char> buffer(size);
        const auto length = ::readlink("/proc/self/exe", buffer.data(), size);
        if (is_negative(length))
            break;

        // The link is not null terminated, and is truncated when it fills.
        const auto chars = possible_narrow_sign_cast<size_t>(length);
        if (chars < size)
            return { std::string{ buffer.begin(),
                std::next(buffer.begin(), length) } };
    }

    return {};
#elif defined(HAVE_APPLE)
    // The call populates the size when the buffer is insufficient.
    uint32_t size{};
    if (is_zero(_NSGetExecutablePath(nullptr, &size)))
        return {};

    std::vector<char> buffer(size);
    if (!is_zero(_NSGetExecutablePath(buffer.data(), &size)))
        return {};

    // The path may be a symlink or contain relative segments, so qualify it.
    return qualified_path({ std::string{ buffer.data() } });
#else
    return {};
#endif
    BC_POP_WARNING()
}

#endif // HAVE_MSC

} // namespace system
} // namespace libbitcoin
