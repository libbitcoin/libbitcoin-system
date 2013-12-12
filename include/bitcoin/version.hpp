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
#ifndef LIBBITCOIN_VERSION_HPP
#define LIBBITCOIN_VERSION_HPP

// Caution, this is the only libbitcoin header that is guarenteed
// to change with every libbitcoin release, including this header
// will cause a recompile every time a new libbitcoin version is
// released.
//
// LIBBITCOIN_VERSION % 100 is the patch level
// LIBBITCOIN_VERSION / 100 % 1000 is the minor version
// LIBBITCOIN_VERSION / 100000 is the major version

#define LIBBITCOIN_VERSION 100400

// LIBBITCOIN_LIB_VERSION is defined to be the same as LIBBITCOIN_VERSION
// but as a *string* in the form "x.y[.z]" where x is the major version
// number, y is the minor version number, and z is the patch level if not 0.
// This is used for the user agent string during the connection handshake.

#define LIBBITCOIN_LIB_VERSION "1.4.0"

#endif

