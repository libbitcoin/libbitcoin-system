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
#ifndef LIBBITCOIN_DEFINE_HPP
#define LIBBITCOIN_DEFINE_HPP

// See http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define BC_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
        #define BC_HELPER_DLL_IMPORT
        #define BC_HELPER_DLL_EXPORT
        #define BC_HELPER_DLL_LOCAL
    #endif
#endif

// Now we use the generic helper definitions above to
// define BC_PUBLIC and BC_PRIVATE.
// BC_PUBLIC is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)
// BC_PRIVATE is used for non-api symbols.

#ifdef BC_DLL
    #ifdef BC_DLL_EXPORTS
        // Being compiled as a DLL (instead of using it)
        #define BC_PUBLIC BC_HELPER_DLL_EXPORT
    #else
        #define BC_PUBLIC BC_HELPER_DLL_IMPORT
    #endif
    #define BC_PRIVATE BC_HELPER_DLL_LOCAL
#else
    // BC_DLL is not defined: this means this is a static lib.
    #define BC_PUBLIC
    #define BC_PRIVATE
#endif

#endif

