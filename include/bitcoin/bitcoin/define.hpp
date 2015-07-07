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

// Create bc namespace alias.
namespace libbitcoin {
} // namespace libbitcoin

namespace bc = libbitcoin;

// Logger definitions. You can add your own to the logger.
// Better to use a define because you get type safety.
#define LOG_NETWORK     "network"
#define LOG_DATABASE    "database"
#define LOG_BLOCKCHAIN  "blockchain"
#define LOG_VALIDATE    "validate"
#define LOG_PROTOCOL    "protocol"
#define LOG_POLLER      "poller"
#define LOG_SESSION     "session"
#define LOG_RESPONDER   "responder"
#define LOG_SCRIPT      "script"
#define LOG_TXPOOL      "transaction_pool"
#define LOG_TXIDX       "transaction_indexer"

// See http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define BC_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_LOCAL  __attribute__ ((visibility ("internal")))
    #else
        #define BC_HELPER_DLL_IMPORT
        #define BC_HELPER_DLL_EXPORT
        #define BC_HELPER_DLL_LOCAL
    #endif
#endif

// Now we use the generic helper definitions above to
// define BC_API and BC_INTERNAL.
// BC_API is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)
// BC_INTERNAL is used for non-api symbols.

#if defined BC_STATIC
    #define BC_API
    #define BC_INTERNAL
#elif defined BC_DLL
    #define BC_API      BC_HELPER_DLL_EXPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BC_API      BC_HELPER_DLL_IMPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Tag to deprecate functions and methods.
// Gives a compiler warning when they are used.
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_DEPRECATED __declspec(deprecated)
#else
    #if __GNUC__ >= 4
        #define BC_DEPRECATED __attribute__((deprecated))
    #else
        #define BC_DEPRECATED
    #endif
#endif

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#endif

