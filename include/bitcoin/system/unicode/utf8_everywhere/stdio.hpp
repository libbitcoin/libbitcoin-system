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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_STDIO_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_STDIO_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Win32 UTF8 console streams.
/// ---------------------------------------------------------------------------
/// Use for cross-platform console I/O, in place of std::cin/cout/cerr.

BC_API std::istream& cin_stream() THROWS;
BC_API std::ostream& cout_stream() THROWS;
BC_API std::ostream& cerr_stream() THROWS;

/// Win32 UTF8 standard I/O utilities.
/// ---------------------------------------------------------------------------
/// These are no-ops unless called on Win32 builds.

/// Initialize windows to use UTF8 for stdio. This cannot be uninitialized and
/// once set bc stdio must be used in place of std stdio.
BC_API void set_utf8_stdio() THROWS;

/// Initialize windows to use UTF8 for stdin. This cannot be uninitialized and
/// once set bc::system::cin must be used in place of std::cin.
BC_API void set_utf8_stdin() THROWS;

/// Initialize windows to use UTF8 for stdout. This cannot be uninitialized and
/// once set bc::system::cout must be used in place of std::cout.
BC_API void set_utf8_stdout() THROWS;

/// Initialize windows to use UTF8 for stderr. This cannot be uninitialized and
/// once set bc::system::cerr must be used in place of std::cerr.
BC_API void set_utf8_stderr() THROWS;

/// Initialize windows to use binary for stdin. This cannot be uninitialized.
BC_API void set_binary_stdin() THROWS;

/// Initialize windows to use binary for stdout. This cannot be uninitialized.
BC_API void set_binary_stdout() THROWS;

/// Console/terminal mode utilities.
/// ---------------------------------------------------------------------------

BC_API void set_console_echo() NOEXCEPT;
BC_API void unset_console_echo() NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
