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

#define LIBBITCOIN_VERSION 100010

// LIBBITCOIN_LIB_VERSION is defined to be the same as LIBBITCOIN_VERSION
// but as a *string* in the form "x.y[.z]" where x is the major version
// number, y is the minor version number, and z is the patch level if not 0.
// This is used for the user agent string during the connection handshake.

#define LIBBITCOIN_LIB_VERSION "1.0.10"

#endif

