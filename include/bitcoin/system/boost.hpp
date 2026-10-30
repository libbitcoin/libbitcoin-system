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
#ifndef LIBBITCOIN_SYSTEM_BOOST_HPP
#define LIBBITCOIN_SYSTEM_BOOST_HPP

///////////////////////////////////////////////////////////////////////////////
// Include boost only here, so these definitions take hold within boost.
// Dependent projects must include <bitcoin/system.hpp> before any boost.
///////////////////////////////////////////////////////////////////////////////

#include <utility>

// Apply any warning suppressions to boost.
#include <bitcoin/system/warnings.hpp>

// Avoid namespace conflict between boost::placeholders and std::placeholders.
// This arises when including <functional>, which declares std::placeholders.
// www.boost.org/doc/libs/1_78_0/boost/bind.hpp
#define BOOST_BIND_NO_PLACEHOLDERS

#include <boost/version.hpp>

#endif
