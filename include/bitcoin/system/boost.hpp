/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_BOOST_HPP
#define LIBBITCOIN_SYSTEM_BOOST_HPP

///////////////////////////////////////////////////////////////////////////////
// Include boost only here, so these definitions take hold within boost.
// Dependent projects must include <bitcoin/system.hpp> before any boost.
///////////////////////////////////////////////////////////////////////////////

#include <utility>

// Apply any warning suppressions to boost.
#include <bitcoin/system/warnings.hpp>

// Headers only containers.
#define BOOST_CONTAINER_NO_LIB

// Avoid namespace conflict between boost::placeholders and std::placeholders.
// This arises when including <functional>, which declares std::placeholders.
// www.boost.org/doc/libs/1_78_0/boost/bind.hpp
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/bind.hpp>

// Boost.Locale is still used by conversion.cpp and environment.cpp.
// ICU backend is no longer required — normalization uses embedded tables.
#include <boost/locale.hpp>

// address::from_string
////#define BOOST_ASIO_NO_DEPRECATED
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/stream.hpp>

// Headers only json.
#define BOOST_JSON_NO_LIB
#include <boost/json.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>
#include <boost/url.hpp>

/// boost aliases
/// ---------------------------------------------------------------------------

namespace libbitcoin
{
    namespace system
    {
        namespace asio
        {
            typedef boost::asio::ip::address address;
            typedef boost::asio::ip::tcp::endpoint endpoint;
        }
    }

    typedef boost::program_options::variables_map variables_map;
    typedef boost::program_options::option_description option_metadata;
    typedef boost::program_options::options_description options_metadata;
    typedef boost::program_options::positional_options_description arguments_metadata;

}

#endif
