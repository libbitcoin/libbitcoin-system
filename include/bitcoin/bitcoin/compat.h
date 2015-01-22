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
#ifndef LIBBITCOIN_COMPAT_H
#define LIBBITCOIN_COMPAT_H

#ifdef _MSC_VER
    // There is no <endian.h> for MSVC but it is always little endian.
    #ifndef __LITTLE_ENDIAN__
        # undef __BIG_ENDIAN__
        # define __LITTLE_ENDIAN__
    #endif
#endif

#ifdef _MSC_VER
    // For SET_BINARY_FILE_MODE
    #include <fcntl.h>
    #include <io.h>
    #include <stdio.h>
#endif

// Sets the _fmode global variable, which controls the default translation
// mode for file I/O operations.
#ifdef _MSC_VER
    #define BC_SET_BINARY_FILE_MODE(mode) \
        _setmode(_fileno(stdin), if_else(mode, _O_BINARY, _O_TEXT))
#else
    #define BC_SET_BINARY_FILE_MODE(mode)
#endif

#endif