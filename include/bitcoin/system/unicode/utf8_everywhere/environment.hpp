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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_ENVIRONMENT_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_ENVIRONMENT_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// This is for Win32 environment support, exposed for test.
/// Use /unicode/conversion.hpp for unicode conversions.
constexpr size_t utf8_max_character_size = 4;
BC_API size_t utf8_remainder_size(const char text[], size_t size) NOEXCEPT;
BC_API size_t to_utf8(char out_to[], size_t to_bytes, const wchar_t from[],
    size_t from_chars) NOEXCEPT;
BC_API size_t to_utf16(size_t& remainder, wchar_t out_to[], size_t to_chars,
    const char from[], size_t from_bytes) NOEXCEPT;

#ifdef HAVE_MSC

/// This is for Win32 environment support, exposed for test.
BC_API void free_environment(char* environment[]) NOEXCEPT;
BC_API char** allocate_environment(wchar_t* environment[]) NOEXCEPT;
BC_API char** allocate_environment(int argc, wchar_t* argv[]) NOEXCEPT;
BC_API int call_utf8_main(int argc, wchar_t* argv[],
    int(*main)(int argc, char* argv[])) NOEXCEPT;

#define BC_USE_LIBBITCOIN_MAIN \
    namespace libbitcoin \
    { \
    namespace system \
    { \
        auto& cin = cin_stream(); \
        auto& cout = cout_stream(); \
        auto& cerr = cerr_stream(); \
        int main(int argc, char* argv[]); \
    } \
    } \
    \
    int wmain(int argc, wchar_t* argv[]) \
    { \
        return libbitcoin::system::call_utf8_main(argc, argv, \
            &libbitcoin::system::main); \
    }

#else

#define BC_USE_LIBBITCOIN_MAIN \
    namespace libbitcoin \
    { \
    namespace system \
    { \
        auto& cin = std::cin; \
        auto& cout = std::cout; \
        auto& cerr = std::cerr; \
        int main(int argc, char* argv[]); \
    } \
    } \
    \
    int main(int argc, char* argv[]) \
    { \
        return libbitcoin::system::main(argc, argv); \
    }

#endif

} // namespace system
} // namespace libbitcoin

#endif
